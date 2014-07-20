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


#ifndef OSS_LOGGER_H_INCLUDED
#define OSS_LOGGER_H_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>

#include "OSS/OSS.h"


namespace OSS {

enum LogPriority
{
  PRIO_NONE,
	PRIO_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
	PRIO_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
	PRIO_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
	PRIO_WARNING,     /// A warning. An operation completed with an unexpected result.
	PRIO_NOTICE,      /// A notice, which is an information with just a higher priority.
	PRIO_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
	PRIO_DEBUG,       /// A debugging message.
	PRIO_TRACE        /// A tracing message. This is the lowest priority.
};

void OSS_API logger_init(
  const std::string& path,
  LogPriority level = OSS::PRIO_INFORMATION,
  const std::string& format = "%h-%M-%S.%i: %t",
  const std::string& compress = "true",
  const std::string& purgeCount = "7");
  /// Initialize the logging subsystem from the config specified

void OSS_API logger_deinit();
  /// Deinitialize the logger

void OSS_API log_reset_level(LogPriority level);
  /// Reset the the log level

LogPriority OSS_API log_get_level();
  /// Return the the log level

const boost::filesystem::path& OSS_API logger_get_path();

void OSS_API logger_set_directory(const boost::filesystem::path& directory);

void OSS_API log(const std::string& log, LogPriority priority = OSS::PRIO_INFORMATION);
  /// Log a message with prioty specified

void OSS_API log_fatal(const std::string& log);
  /// Log a fatal error. The application will most likely terminate. This is the highest priority.

#define OSS_LOG_FATAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_fatal(strm.str()); \
}

void OSS_API log_critical(const std::string& log);
  /// Log a critical error. The application might not be able to continue running successfully.

#define OSS_LOG_CRITICAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_critical(strm.str()); \
}

void OSS_API log_error(const std::string& log);
  /// Log an error. An operation did not complete successfully, but the application as a whole is not affected.

void OSS_API log_enable_console(bool yes = true);
  /// Enable console logging if file output is not specified

void OSS_API log_enable_logging(bool yes);
  /// Enabel or disable logging.  If set to false, no log output will be written to either console or file


#define OSS_LOG_ERROR(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_error(strm.str()); \
}

void OSS_API log_warning(const std::string& log);
  /// Log a warning. An operation completed with an unexpected result.

#define OSS_LOG_WARNING(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_warning(strm.str()); \
}

void OSS_API log_notice(const std::string& log);
  /// Log a notice, which is an information with just a higher priority.

#define OSS_LOG_NOTICE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_notice(strm.str()); \
}

void OSS_API log_information(const std::string& log);
  /// Log an informational message, usually denoting the successful completion of an operation.

#define OSS_LOG_INFO(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_information(strm.str()); \
}


void OSS_API log_debug(const std::string& log);
  /// Log a debugging message.

#define OSS_LOG_DEBUG(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_debug(strm.str()); \
}

void OSS_API log_trace(const std::string& log);
  /// Log a tracing message. This is the lowest priority.

#define OSS_LOG_TRACE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::log_trace(strm.str()); \
}

#ifdef _DEBUG
#define OSS_LOG_DEV_TRACE OSS_LOG_NOTICE
#else
#define OSS_LOG_DEV_TRACE
#endif


} // OSS
#endif // OSS_LOGGER_H_INCLUDED
