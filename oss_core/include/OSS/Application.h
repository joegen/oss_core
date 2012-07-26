// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
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


#ifndef OSS_APPLICATION_H_INCLUDED
#define OSS_APPLICATION_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/Logger.h"

#include <iostream>
#include <sstream>
#include <vector>


namespace OSS {

typedef boost::function<void()> app_init_func;
typedef boost::function<void()> app_deinit_func;
typedef boost::function<int(const std::vector<std::string>&)> app_main_func;  

enum app_exit_code
	/// Commonly used exit status codes.
	/// Based on the definitions in the 4.3BSD <sysexits.h> header file.
{
	APP_EXIT_OK          = 0,  /// successful termination
	APP_EXIT_USAGE	     = 64, /// command line usage error
	APP_EXIT_DATAERR     = 65, /// data format error
	APP_EXIT_NOINPUT     = 66, /// cannot open input
	APP_EXIT_NOUSER      = 67, /// addressee unknown
	APP_EXIT_NOHOST      = 68, /// host name unknown
	APP_EXIT_UNAVAILABLE = 69, /// service unavailable
	APP_EXIT_SOFTWARE    = 70, /// internal software error
	APP_EXIT_OSERR	     = 71, /// system error (e.g., can't fork)
	APP_EXIT_OSFILE      = 72, /// critical OS file missing
	APP_EXIT_CANTCREAT   = 73, /// can't create (user) output file
	APP_EXIT_IOERR       = 74, /// input/output error
	APP_EXIT_TEMPFAIL    = 75, /// temp failure; user is invited to retry
	APP_EXIT_PROTOCOL    = 76, /// remote error in protocol
	APP_EXIT_NOPERM      = 77, /// permission denied
	APP_EXIT_CONFIG      = 78  /// configuration error
};

void OSS_API app_set_main_handler(app_main_func handler);
  /// Set the call back for the main handler

void OSS_API app_set_init_handler(app_init_func handler);
  /// Set the callback function for the init handler

void OSS_API app_set_deinit_handler(app_deinit_func handler);
  /// Set the callback function for the deinit handler

int OSS_API app_run(int argc, char** argv);
  /// Run the application

bool OSS_API app_is_running();
  /// Return true if the application is already running.
  /// This flag would be set after the first call to app_run();

void OSS_API app_terminate();
  /// Terminate the application.  This will signal app_wait_for_termination_request() if called

void OSS_API app_set_logger_file(const std::string& path);
  /// Set the path for the application log file

void OSS_API app_set_config_file(const std::string& path);
  /// Set the path for the application config file

void OSS_API app_set_pid_file(const std::string& path);
  /// Set the path for the application pidfile file will be stored

unsigned long OSS_API app_get_pid();
  /// Return the current Process ID

void OSS_API app_log_reset_level(LogPriority level);
  /// Reset the the log level

void OSS_API app_log(const std::string& log, LogPriority priority = OSS::PRIO_INFORMATION);
  /// Log a message with prioty specified

void OSS_API app_log_fatal(const std::string& log);
  /// Log a fatal error. The application will most likely terminate. This is the highest priority.

#define OSS_APP_LOG_FATAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_fata(strm.str()); \
}

void OSS_API app_log_critical(const std::string& log);
  /// Log a critical error. The application might not be able to continue running successfully.

#define OSS_APP_LOG_CRITICAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_critical(strm.str()); \
}

void OSS_API app_log_error(const std::string& log);
  /// Log an error. An operation did not complete successfully, but the application as a whole is not affected.

#define OSS_APP_LOG_ERROR(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_error(strm.str()); \
}

void OSS_API app_log_warning(const std::string& log);
  /// Log a warning. An operation completed with an unexpected result.

#define OSS_APP_LOG_WARNING(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_warning(strm.str()); \
}

void OSS_API app_log_notice(const std::string& log);
  /// Log a notice, which is an information with just a higher priority.

#define OSS_APP_LOG_NOTICE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_notice(strm.str()); \
}

void OSS_API app_log_information(const std::string& log);
  /// Log an informational message, usually denoting the successful completion of an operation.

#define OSS_APP_LOG_INFO(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_information(strm.str()); \
}

void OSS_API app_log_debug(const std::string& log);
  /// Log a debugging message.

#define OSS_APP_LOG_DEBUG(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_debug(strm.str()); \
}

void OSS_API app_log_trace(const std::string& log);
  /// Log a tracing message. This is the lowest priority.

#define OSS_APP_LOG_TRACE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  OSS::app_log_trace(strm.str()); \
}

std::string OSS_API app_config_get_string(const std::string& key) ;
  /// Returns the string value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.

std::string OSS_API app_config_get_string(const std::string& key, const std::string& defaultValue) ;
  /// If a property with the given key exists, returns the property's string value,
  /// otherwise returns the given default value.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.

std::string OSS_API app_config_get_raw_string(const std::string& key) ;
  /// Returns the raw string value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// References to other properties are not expanded.
	
std::string OSS_API app_config_get_raw_string(const std::string& key, const std::string& defaultValue) ;
  /// If a property with the given key exists, returns the property's raw string value,
  /// otherwise returns the given default value.
  /// References to other properties are not expanded.
	
int OSS_API app_config_get_int(const std::string& key) ;
  /// Returns the int value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to an int.
  /// Numbers starting with 0x are treated as hexadecimal.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
	
int OSS_API app_config_get_int(const std::string& key, int defaultValue) ;
  /// If a property with the given key exists, returns the property's int value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to an int.
  /// Numbers starting with 0x are treated as hexadecimal.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.

double OSS_API app_config_get_double(const std::string& key) ;
  /// Returns the double value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to a double.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
	
double OSS_API app_config_get_double(const std::string& key, double defaultValue) ;
  /// If a property with the given key exists, returns the property's double value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to an double.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.

bool OSS_API app_config_get_bool(const std::string& key) ;
  /// Returns the double value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to a double.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
	
bool OSS_API app_config_get_bool(const std::string& key, bool defaultValue) ;
  /// If a property with the given key exists, returns the property's bool value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to a boolean.
  /// The following string values can be converted into a boolean:
  ///   - numerical values: non zero becomes true, zero becomes false
  ///   - strings: true, yes, on become true, false, no, off become false
  /// Case does not matter.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
	
void OSS_API app_config_set_string(const std::string& key, const std::string& value);
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.
	
void OSS_API app_config_set_int(const std::string& key, int value);
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.

void OSS_API app_config_set_double(const std::string& key, double value);
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.

void OSS_API app_config_set_bool(const std::string& key, bool value);
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.

void OSS_API app_wait_for_termination_request();
  /// Wait for a termination signal

void OSS_API app_set_exit_code(app_exit_code code);
  /// Set the exit code for the application

int OSS_API app_get_argc();
  /// Return the value of the command line argument count

//
// Shell related functions
//
app_exit_code OSS_API app_get_exit_code();
  /// Get the exit code set by the application.  Defaults to APP_EXIT_OK if none is set

bool OSS_API app_is_process_exist(int pid);
  /// Check if a process is running with this PID

bool OSS_API app_process_kill(int pid, int sig);
  /// Kill a running process

bool OSS_API app_shell_execute(const std::string& app, const std::string& args, bool hidden = true);
  /// Execute a process within the active shell

bool OSS_API app_shell_command(const std::string& command, std::string& result);
  /// Execute a shell commad.  This function is handy for use with shell commands
  /// that produces printable results such as `pwd`.  The result of the commands
  /// output will be stored in the result variable upon successful completion.
  ///
  /// Note: this function uses ::popen() which apparently behaves differently between
  /// unix and windows.  For windows, the command must be enclosed in double quotes `"`.
  /// Example: app_shell_command("\"\"C:\\some dir\\somecommand.exe\" \"input file\"\"", result);


  std::string OSS_API app_environment_get(const std::string& name);
    /// Returns the value of the environment variable
    /// with the given name. Throws a NotFoundException
    /// if the variable does not exist.

  std::string OSS_API app_environment_get(const std::string& name, const std::string& defaultValue);
    /// Returns the value of the environment variable
    /// with the given name. If the environment variable
    /// is undefined, returns defaultValue instead.

  bool OSS_API app_environment_has(const std::string& name);
    /// Returns true iff an environment variable
    /// with the given name is defined.

  void OSS_API app_environment_set(const std::string& name, const std::string& value);
    /// Sets the environment variable with the given name
    /// to the given value.

  std::string OSS_API app_environment_os_name();
    /// Returns the operating system name.

  std::string OSS_API app_environment_os_version();
    /// Returns the operating system version.

  std::string OSS_API app_environment_os_architecture();
    /// Returns the operating system architecture.

  std::string OSS_API app_environment_node_name();
    /// Returns the node (or host) name.

  std::string OSS_API app_environment_node_id();
    /// Returns the Ethernet address (format "xx:xx:xx:xx:xx:xx")
    /// of the first Ethernet adapter found on the system.
    ///
    /// Throws a SystemException if no Ethernet adapter is available.

  unsigned OSS_API app_environment_processor_count();
    /// Returns the number of processors installed in the system.
    ///
    /// If the number of processors cannot be determined, returns 1.


} // OSS
#endif // OSS_APPLICATION_H_INCLUDED


