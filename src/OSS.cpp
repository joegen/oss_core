// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
//
// Copyright (c) OSS Software Solutions
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
#include "OSS/OSS.h"
#include "OSS/Net.h"
#include "OSS/Logger.h"
#include "Poco/Stopwatch.h"


namespace OSS {

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

void OSS_API OSS_register_init(boost::function<void()> func)
{
  _initFuncs.push_back(func);
}

void OSS_API OSS_register_deinit(boost::function<void()> func)
{
  _deinitFuncs.push_back(func);
}

void OSS_init()
{
  OSS::Private::net_init();

  for (static std::vector<boost::function<void()> >::iterator iter = _initFuncs.begin();
    iter != _initFuncs.end(); iter++) (*iter)();
}

void OSS_deinit()
{
  for (static std::vector<boost::function<void()> >::iterator iter = _deinitFuncs.begin();
    iter != _deinitFuncs.end(); iter++) (*iter)();

  OSS::Private::net_deinit();
}



} // OSS

