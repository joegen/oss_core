#include "OSS/UTL/Application.h"
#include "OSS/UTL/ServiceOptions.h"

int main(int argc, char** argv)
{
  bool isDaemon = false;
  OSS::ServiceOptions::daemonize(argc, argv, isDaemon);
  std::set_terminate(&OSS::ServiceOptions::catch_global);
  OSS::OSS_init();
  
  OSS::OSS_deinit();
  return 0;
}