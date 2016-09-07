#include "OSS/UTL/Application.h"
#include "OSS/UTL/ServiceOptions.h"
#include "OSS/JS/DUK/DuktapeContext.h"

using OSS::JS::DUK::DuktapeContext;
using OSS::JS::DUK::DuktapeModule;

#define LOHIKA_DAEMON_NAME "Lohika"
#define LOHIKA_MAJOR_VERSION "0"
#define LOHIKA_MINOR_VERSION "0"
#define LOHIKA_BUILD_NUMBER "1"
#define LOHIKA_VERSION LOHIKA_MAJOR_VERSION "." LOHIKA_MINOR_VERSION "." LOHIKA_BUILD_NUMBER
#define LOHIKA_COPYRIGHT "OSS Software Solutions.  All Rights Reserved"

static DuktapeContext* gInstance = 0; 
static OSS::ServiceOptions* gOptions = 0;

static bool init_program_options(int argc, char** argv)
{
  gOptions = new OSS::ServiceOptions(argc, argv, LOHIKA_DAEMON_NAME, LOHIKA_VERSION, LOHIKA_COPYRIGHT, OSS::ServiceOptions::Daemon);
  gOptions->addOptionStringVector('m', "module-dir", "Module Directory");
  gOptions->addOptionString('j', "js", "JS file to execute");
  return gOptions->parseOptions(false);
}

static bool init_module_directories()
{
  assert(gInstance);
  gInstance->addModuleDirectory("mod");
  gInstance->addModuleDirectory("/usr/local/lib/lohika/mod");
  gInstance->addModuleDirectory("/usr/lib/lohika/mod");
  
  if (gOptions->hasOption("module-dir"))
  {
    std::vector<std::string> modules;
    gOptions->getOption("module-dir", modules);
    for (std::vector<std::string>::const_iterator iter = modules.begin(); iter != modules.end(); iter++)
    {
      gInstance->addModuleDirectory(*iter);
    }
  }
  return true;
}

duk_ret_t printHello(duk_context* ctx) 
{
  std::cout << "Hello Internal Module!" << std::endl;
  return 0;
}

int main(int argc, char** argv)
{
  bool isDaemon = false;

  OSS::ServiceOptions::daemonize(argc, argv, isDaemon);
  std::set_terminate(&OSS::ServiceOptions::catch_global);
  OSS::OSS_init();

  int ret = 0;
  std::string inputFile;
  
  if (!init_program_options(argc, argv))
  {
    ret = -1;
    goto exit;
  }
  
  //
  // Instantiate the root instance
  //
  gInstance = DuktapeContext::rootInstance();
  
  if (!init_module_directories())
  {
    ret = -1;
    goto exit;
  }
  
  if (!gOptions->hasOption("js"))
  {
    ret = -1;
    std::cout << "input-file missing" << std::endl;
    goto exit;
  }
  
  gOptions->getOption("js", inputFile);
  
  if (!inputFile.empty())
  {
    gInstance->evalFile(inputFile, 0, 0);
  }
  
exit:  
  OSS::OSS_deinit();
  delete gInstance;
  delete gOptions;
  return ret;
}