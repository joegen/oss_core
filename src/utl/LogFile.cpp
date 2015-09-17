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


#include <boost/thread.hpp>
#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Message.h"
#include "Poco/LogFile.h"
#include "Poco/Timestamp.h"
#include "Poco/LogStream.h"
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

#include "OSS/UTL/LogFile.h"

namespace OSS {
namespace UTL {
 
  static const std::string LOGGER_DEFAULT_NAME = "Core Logger";
  static const std::string LOGGER_DEFAULT_FORMAT = "%h-%M-%S.%i: %t"; 
  static const LogFile::Priority LOGGER_DEFAULT_PRIORITY = LogFile::PRIO_INFORMATION;
  static const unsigned int LOGGER_DEFAULT_PURGE_COUNT = 0; /// Disable log rotatepoco
  static unsigned int DEFAULT_VERIFY_TTL = 5; /// TTL in seconds for verification to kick in

    
  static Poco::Message::Priority poco_priority(OSS::UTL::LogFile::Priority priority)
  {
    switch (priority)
    {
      case OSS::UTL::LogFile::PRIO_FATAL:
        return Poco::Message::PRIO_FATAL;
      case OSS::UTL::LogFile::PRIO_CRITICAL:
        return Poco::Message::PRIO_CRITICAL;
      case OSS::UTL::LogFile::PRIO_ERROR:
        return Poco::Message::PRIO_ERROR;
      case OSS::UTL::LogFile::PRIO_WARNING:
        return Poco::Message::PRIO_WARNING;
      case OSS::UTL::LogFile::PRIO_NOTICE:
        return Poco::Message::PRIO_NOTICE;
      case OSS::UTL::LogFile::PRIO_INFORMATION:
        return Poco::Message::PRIO_INFORMATION;
      case OSS::UTL::LogFile::PRIO_DEBUG:
        return Poco::Message::PRIO_DEBUG;
      case OSS::UTL::LogFile::PRIO_TRACE:
        return Poco::Message::PRIO_TRACE;
    }
    
    //
    // Someone is too drunk to code.  Should never get here.
    //
    assert(false);
  }
    
  LogFile::LogFile(const std::string& name) :
    _name(name),
    _instanceCount(0),
    _lastVerifyTime(0),
    _enableVerification(true),
    _verificationInterval(DEFAULT_VERIFY_TTL),
    _isOpen(false)
  {
    std::ostringstream strm;
    strm << _name << "-" << _instanceCount;
    _internalName = strm.str();
  }

  LogFile::~LogFile()
  {
    //
    // Grab the mutex before calling close to make sure we do not corrupt
    // any pointers within the current executing log message
    //
    mutex_lock lock(_mutex);
    close();
  }


  bool LogFile::open(
    const std::string& path 
  )
  {
    return open(path, LOGGER_DEFAULT_PRIORITY);
  }
  
  bool LogFile::open(
    const std::string& path,
    Priority priority
  )
  {
    return open(path, priority, LOGGER_DEFAULT_FORMAT);
  }

  bool LogFile::open(
    const std::string& path,  
    Priority priority,
    const std::string& format 
  )
  {
    return open(path, priority, format, LOGGER_DEFAULT_PURGE_COUNT);
  }

  bool LogFile::open(
    const std::string& path,  
    Priority priority,
    const std::string& format, 
    unsigned int purgeCount 
  )
  {
    if (_isOpen)
    {
      warning("LogFile::open invoked while already in open state.  Close the logger first by calling LogFile::close()");
      return false;
    }
    
    try
    {
      _path = path;
      _priority = priority;
      _format = format;
      _purgeCount = purgeCount;

      bool enableLogRotate = LOGGER_DEFAULT_PURGE_COUNT > 0;
      std::string strPurgeCount = boost::lexical_cast<std::string>(purgeCount);
      Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel(path));

      if (enableLogRotate)
      {
        fileChannel->setProperty("rotation", "daily");
        fileChannel->setProperty("archive", "timestamp");
        fileChannel->setProperty("compress", "true");
        fileChannel->setProperty("purgeCount", strPurgeCount);
      }

      //
      // increment the instance name so that we use a 
      // new logger instance when we open/reopen the channel
      //
      std::ostringstream strmName;
      strmName << _name << "-" << ++_instanceCount;
      _internalName = strmName.str();
      
      Poco::AutoPtr<Poco::Formatter> formatter(new Poco::PatternFormatter(format.c_str()));
      Poco::AutoPtr<Poco::Channel> formattingChannel(new Poco::FormattingChannel(formatter, fileChannel));
      Poco::Logger::create(_internalName, formattingChannel, poco_priority(priority));
      
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        Poco::LogStream logStrm(*pLogFile);
        logStrm.notice() << "LogFile::open(" << _internalName << ") path: " << _path << std::endl;
          
        _lastError = "";
        _isOpen = true;
      }
      else
      {
        _lastError = "LogFile::open - Poco::Logger is null";
        _isOpen = false;
      }
    }
    catch(const std::exception& e)
    {
      _lastError = "LogFile::open - ";
      _lastError += e.what();
      close();
      _isOpen = false;
    }
    catch(...)
    {
      _lastError = "LogFile::open unknown exception";
      close();
      _isOpen = false;
    }
    
    return _isOpen;
  }

  void LogFile::close()
  {
    if (Poco::Logger::has(_internalName))
      Poco::Logger::destroy(_internalName);
    
    _isOpen = Poco::Logger::has(_internalName);
  }
 
  void LogFile::setPriority(LogFile::Priority priority)
  {
    _priority = priority;
    Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
    if (pLogFile)
    {
      pLogFile->setLevel(poco_priority(priority));
    }
  }
  
  bool LogFile::willLog(Priority priority) const
  {
    return priority <= _priority;
  }

  void LogFile::fatal(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_FATAL) && (_enableVerification ? verifyLogFile(false) : isOpen()) )
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->fatal(log);
      }
    }
  }

  void LogFile::critical(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_CRITICAL) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->critical(log);
      }
    }
  }

  void LogFile::error(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_ERROR) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->error(log);
      }
    }
  }

  void LogFile::warning(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_WARNING) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->warning(log);
      }
    }
  }

  void LogFile::notice(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_NOTICE) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->notice(log);
      }
    }
  }

  void LogFile::information(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_INFORMATION) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->information(log);
      }
    }
  }

  void LogFile::debug(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_DEBUG) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->debug(log);
      }
    }
  }

  void LogFile::trace(const std::string& log)
  {
    //
    // We need to make this thread safe or calls from 
    // different thread might try to reopen the logger
    // at the same time when calling verifyLogFile.
    // This can result to a segmentation fault if pLogFile
    // is released from another thread
    //
    mutex_lock lock(_mutex);
    
    if (willLog(PRIO_TRACE) && (_enableVerification ? verifyLogFile(false) : isOpen()))
    {
      Poco::Logger* pLogFile = Poco::Logger::has(_internalName);
      if (pLogFile)
      {
        pLogFile->trace(log);
      }
    }
  }
  
  bool LogFile::verifyLogFile(bool force)
  {    
    if (!_isOpen)
    {
      //
      // log file cannot be verified because logger is not open
      //
      return false;
    }
    
    Poco::Timestamp timeStamp;
    std::time_t now = timeStamp.epochTime();
    
    if (!force && (now - _lastVerifyTime < _verificationInterval))
    {
      return true; // TTL for this file has not yet expired
    }
    
    _lastVerifyTime = now;
    
    if (!_path.empty() && !boost::filesystem::exists(_path))
    {
      //
      // Close the old logger.  We are about to reopen a new one
      //
      close();
      
      return open(_path, _priority, _format, _purgeCount); 
    }

    return _isOpen;
  }

} } // OSS::UTL