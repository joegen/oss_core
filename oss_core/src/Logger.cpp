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
#include "OSS/OSS.h"

#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Message.h"
#include "Poco/Logger.h"
#include <iostream>
#include <sstream>

#include "OSS/Logger.h"
#include "OSS/Core.h"
#include "OSS/Thread.h"


using Poco::AutoPtr;
using Poco::Channel;
using Poco::ConsoleChannel;
using Poco::SplitterChannel;
using Poco::FileChannel;
using Poco::FormattingChannel;
using Poco::Formatter;
using Poco::PatternFormatter;
using Poco::Logger;
using Poco::Message;


namespace OSS {


static Poco::Logger* _pLogger = 0;
static boost::filesystem::path _logFile;
static boost::filesystem::path _logDirectory;;
static OSS::mutex_critic_sec _consoleMutex;
static bool _enableConsoleLogging = true;
static bool _enableLogging = true;

  /*
enum LogPriority
{
	PRIO_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
	PRIO_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
	PRIO_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
	PRIO_WARNING,     /// A warning. An operation completed with an unexpected result.
	PRIO_NOTICE,      /// A notice, which is an information with just a higher priority.
	PRIO_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
	PRIO_DEBUG,       /// A debugging message.
	PRIO_TRACE        /// A tracing message. This is the lowest priority.
};
*/
void log_enable_console(bool yes)
{
  _enableConsoleLogging = yes;
}

void log_enable_logging(bool yes)
{
  _enableLogging = yes;
}

void logger_init(
  const std::string& path,
  LogPriority level,
  const std::string& format,
  const std::string& compress,
  const std::string& purgeCount)
{
  if (_enableLogging)
  {
    _logFile = path;

    if (!_logDirectory.empty())
    {
      //
      // Application override for the directory
      //
      std::string lfile = OSS::boost_file_name(_logFile);
      _logFile = operator/(_logDirectory, lfile);
    }

    AutoPtr<FileChannel> rotatedFileChannel(new FileChannel(OSS::boost_path(_logFile)));
    rotatedFileChannel->setProperty("rotation", "daily");
    rotatedFileChannel->setProperty("archive", "timestamp");
    rotatedFileChannel->setProperty("compress", compress);
    rotatedFileChannel->setProperty("purgeCount", purgeCount);

    AutoPtr<Formatter> formatter(new PatternFormatter(format.c_str()));
    AutoPtr<Channel> formattingChannel(new FormattingChannel(formatter, rotatedFileChannel));
    _pLogger = &(Logger::create("OSS.logger", formattingChannel, level));
  }
}

void logger_deinit()
{
  if (_pLogger)
    _pLogger->destroy("OSS.logger");
}

void log_reset_level(LogPriority level)
{
  if (!_enableLogging)
    return;
  OSS_VERIFY_NULL(_pLogger);
  _pLogger->setLevel(level);
}

LogPriority log_get_level()
{
  if (!_enableLogging || !_pLogger)
    return PRIO_NONE;

  return static_cast<LogPriority>(_pLogger->getLevel());
}

const boost::filesystem::path& logger_get_path()
{
  return _logFile;
}

void logger_set_directory(const boost::filesystem::path& directory)
{
  _logDirectory = directory;
}

void log(const std::string& log, LogPriority priority)
{
  if (!_enableLogging)
    return;

  switch (priority)
  {
  case PRIO_FATAL :
    log_fatal(log);
    break;
  case PRIO_CRITICAL :
    log_critical(log);
    break;
  case PRIO_ERROR :
    log_error(log);
    break;
  case PRIO_WARNING :
    log_warning(log);
    break;
  case PRIO_NOTICE :
    log_notice(log);
    break;
  case PRIO_INFORMATION :
    log_information(log);
    break;
  case PRIO_DEBUG :
    log_debug(log);
    break;
  case PRIO_TRACE :
    log_trace(log);
    break;
  default:
    OSS_VERIFY(false);
  }
}

void log_fatal(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[FATAL] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  _pLogger->fatal(log);
}

void log_critical(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[CRITICAL] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }

  if(!_pLogger)
    return;
  _pLogger->critical(log);
}

void log_error(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[ERROR] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  _pLogger->error(log);
}

void log_warning(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[WARNING] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  _pLogger->warning(log);
}

void log_notice(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[NOTICE] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }

  _pLogger->notice(log);
}

void log_information(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[INFORMATION] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  if (_pLogger)
    _pLogger->information(log);
}

void log_debug(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[DEBUG] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  if (_pLogger)
    _pLogger->debug(log);
}

void log_trace(const std::string& log)
{
  if (!_enableLogging)
    return;

  if(!_pLogger && _enableConsoleLogging)
  {
    _consoleMutex.lock();
    std::cout << "[TRACE] " << log << std::endl;
    _consoleMutex.unlock();
    return;
  }
  if (_pLogger)
    _pLogger->trace(log);
}

} // OSS

