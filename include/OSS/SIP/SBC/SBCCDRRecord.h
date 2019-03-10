// OSS Software Solutions Application Programmer Interface
// Package: SBC
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef SBCCDRRECORD_H_INCLUDED
#define	SBCCDRRECORD_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/Persistent/RedisClient.h"
#include "OSS/UTL/LogFile.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"

namespace OSS {
namespace SIP {
namespace SBC {

  
class SBCCDRRecord
{
public:
  SBCCDRRecord();
  SBCCDRRecord(const SBCCDRRecord& copy);
  ~SBCCDRRecord();
  
  void swap(SBCCDRRecord& swappable);
  SBCCDRRecord& operator=(const SBCCDRRecord& copy);
  
  std::string& date();
  std::string& fromUri();
  std::string& toUri();
  std::string& referUri();
  std::string& callerContact();
  std::string& calledContact();
  std::string& srcAddress();
  std::string& dstAddress();
  std::string& requestUri();
  std::string& errorResponse();
  std::string& callId();
  std::string& note();
  OSS::UInt64& setupTime();
  OSS::UInt64& alertingTime();
  OSS::UInt64& connectTime();
  OSS::UInt64& disconnectTime();
  std::string& sessionId();
  
  bool writeToRedis(Persistent::RedisBroadcastClient& redis, const std::string& key, unsigned int expire);
  bool readFromRedis(Persistent::RedisBroadcastClient& redis, const std::string& key);
  bool writeToLogFile(OSS::UTL::LogFile& logFile);
  void toJson(json::Object& object);
  
protected:
  std::string _date;
  std::string _fromUri;
  std::string _toUri;
  std::string _referUri;
  std::string _callerContact;
  std::string _calledContact;
  std::string _srcAddress;
  std::string _dstAddress;
  std::string _requestUri;
  std::string _errorResponse;
  std::string _callId;
  std::string _note;
  OSS::UInt64 _setupTime;
  OSS::UInt64 _alertingTime;
  OSS::UInt64 _connectTime;
  OSS::UInt64 _disconnectTime;
  std::string _sessionId;
};
  
//
// Inlines
//


inline std::string& SBCCDRRecord::date()
{
  return _date;
}

inline std::string& SBCCDRRecord::fromUri()
{
  return _fromUri;
}

inline std::string& SBCCDRRecord::toUri()
{
  return _toUri;
}

inline std::string& SBCCDRRecord::referUri()
{
  return _referUri;
}

inline std::string& SBCCDRRecord::callerContact()
{
  return _callerContact;
}

inline std::string& SBCCDRRecord::calledContact()
{
  return _calledContact;
}

inline std::string& SBCCDRRecord::srcAddress()
{
  return _srcAddress;
}

inline std::string& SBCCDRRecord::dstAddress()
{
  return _dstAddress;
}

inline std::string& SBCCDRRecord::requestUri()
{
  return _requestUri;
}

inline std::string& SBCCDRRecord::errorResponse()
{
  return _errorResponse;
}

inline std::string& SBCCDRRecord::callId()
{
  return _callId;
}

inline std::string& SBCCDRRecord::note()
{
  return _note;
}

inline OSS::UInt64& SBCCDRRecord::setupTime()
{
  return _setupTime;
}

inline OSS::UInt64& SBCCDRRecord::alertingTime()
{
  return _alertingTime;
}

inline OSS::UInt64& SBCCDRRecord::connectTime()
{
  return _connectTime;
}

inline OSS::UInt64& SBCCDRRecord::disconnectTime()
{
  return _disconnectTime;
}

inline std::string& SBCCDRRecord::sessionId()
{
  return _sessionId;
}


} } } // OSS::SIP::SBC


#endif	// SBCCDRRECORD_H_INCLUDED

