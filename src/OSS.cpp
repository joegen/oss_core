// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include <vector>
#include <map>
#include "OSS/OSS.h"
#include "OSS/build.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Logger.h"
#include "Poco/Stopwatch.h"
#include "Poco/Timestamp.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/CrashHandler.h"


namespace OSS {

static OSS::mutex_critic_sec gInitMutex;  
static bool gCalledInit = false;
static bool gCalledDeinit = false;
static char** gArgv = 0;
static int gArgc = 0;

namespace Private {

  
  
  TimedFuncTimer::TimedFuncTimer(const char* fileName, int lineNumber, const char* funcName)
  {
    _fileName = fileName;
    _lineNumber = lineNumber;
    _funcName = funcName;
    _stopWatch = new Poco::Stopwatch();
    _pitStop = 0;
    static_cast<Poco::Stopwatch*>(_stopWatch)->start();
  }

  TimedFuncTimer::~TimedFuncTimer()
  {
    Poco::Stopwatch* stopWatch = static_cast<Poco::Stopwatch*>(_stopWatch);
    Poco::Timestamp::TimeDiff diff = stopWatch->elapsed();
    OSS_LOG_NOTICE("Timed Function: " << _fileName << ":" << _lineNumber
            << ":" << _funcName << "() Total Elapsed = " << diff << " microseconds");
    delete stopWatch;
  }

  void TimedFuncTimer::flushElapsed(const char* label)
  {
    Poco::Stopwatch* stopWatch = static_cast<Poco::Stopwatch*>(_stopWatch);
    Poco::Timestamp::TimeDiff diff = stopWatch->elapsed() - _pitStop;
    _pitStop += diff;
    OSS_LOG_NOTICE("Timed Function Flush " << label << ": " << _fileName << ":" << _lineNumber
            << ":" << _funcName << "() Elapsed = " << diff << " microseconds");
  }
  
  
}

static std::vector<boost::function<void()> > _initFuncs;
static std::vector<boost::function<void()> > _deinitFuncs;
static OSS::Debug::CrashHandler* _crashHandler = 0;
static std::ostringstream _crashStream;

ssize_t crash_handler_output_callback(const char* msg, size_t size)
{
  _crashStream << std::string(msg, size);
  return size;
}

void crash_handler_signal_complete()
{
  Poco::LocalDateTime now;
  std::ostringstream preamble;
  preamble << "Date: " << Poco::DateTimeFormatter::format(now, "%Y/%m/%d %H:%M:%S") << std::endl;
  std::cerr << preamble.str() << std::endl;
  std::cerr << _crashStream.str() << std::endl;
  OSS_LOG_ERROR(_crashStream.str());
}

void OSS_API OSS_register_init(boost::function<void()> func)
{
  _initFuncs.push_back(func);
}

void OSS_API OSS_register_deinit(boost::function<void()> func)
{
  _deinitFuncs.push_back(func);
}

static void enable_crash_handler()
{
  if (!_crashHandler)
  {
    _crashHandler = new OSS::Debug::CrashHandler();
    _crashHandler->setThreadSafe(true); // resume the parent process or you will corrp code db during crash
    _crashHandler->setColorOutput(false);
    _crashHandler->setSignalComplete(crash_handler_signal_complete);
    _crashHandler->setOutputCallback(crash_handler_output_callback);
  }
}
void OSS_enable_crash_handler()
{
  OSS::mutex_critic_sec_lock lock(gInitMutex);
  enable_crash_handler();
}

void OSS_init(int argc, char** argv, bool enableCrashHandling)
{
  OSS::mutex_critic_sec_lock lock(gInitMutex);
  if (gCalledInit)
  {
    return;
  }
  
  if (enableCrashHandling)
  {
    enable_crash_handler();
  }
  
  gCalledInit = true;
  gArgc = argc;
  gArgv = argv;
  OSS::Private::net_init();
  OSS::__init_system_dir();

  for (static std::vector<boost::function<void()> >::iterator iter = _initFuncs.begin();
    iter != _initFuncs.end(); iter++) (*iter)();
}

void OSS_API OSS_init(bool enableCrashHandling)
{
  OSS_init(0, 0, enableCrashHandling);
}

void OSS_argv(int* argc, char*** argv)
{
  *argc = gArgc;
  *argv = gArgv;
}

void OSS_deinit()
{
  OSS::mutex_critic_sec_lock lock(gInitMutex);
  if (gCalledDeinit)
  {
    return;
  }
  
  gCalledDeinit = true;
  for (static std::vector<boost::function<void()> >::iterator iter = _deinitFuncs.begin();
    iter != _deinitFuncs.end(); iter++) (*iter)();

  OSS::Private::net_deinit();
  delete _crashHandler;
  _crashHandler = 0;
}



} // OSS

