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


#include "OSS/SIP/SBC/SBCCDRRecord.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {



SBCCDRRecord::SBCCDRRecord() :
  _setupTime(0),
  _alertingTime(0),
  _connectTime(0),
  _disconnectTime(0)
{
}

SBCCDRRecord::~SBCCDRRecord()
{
}

SBCCDRRecord::SBCCDRRecord(const SBCCDRRecord& copy)
{
  _date = copy._date;
  _fromUri = copy._fromUri;
  _toUri = copy._toUri;
  _referUri = copy._referUri;
  _callerContact = copy._callerContact;
  _calledContact = copy._calledContact;
  _srcAddress = copy._srcAddress;
  _dstAddress = copy._dstAddress;
  _requestUri = copy._requestUri;
  _errorResponse = copy._errorResponse;
  _callId = copy._callId;
  _note = copy._note;
  _setupTime = copy._setupTime;
  _alertingTime = copy._alertingTime;
  _connectTime = copy._connectTime;
  _disconnectTime = copy._disconnectTime;
  _sessionId = copy._sessionId;
}

void SBCCDRRecord::swap(SBCCDRRecord& swappable)
{
  std::swap(_date, swappable._date);  
  std::swap(_fromUri, swappable._fromUri);
  std::swap(_toUri, swappable._toUri);
  std::swap(_referUri, swappable._referUri);
  std::swap(_callerContact, swappable._callerContact);
  std::swap(_calledContact, swappable._calledContact);
  std::swap(_srcAddress, swappable._srcAddress);
  std::swap(_dstAddress, swappable._dstAddress);
  std::swap(_requestUri, swappable._requestUri);
  std::swap(_errorResponse, swappable._errorResponse);
  std::swap(_callId, swappable._callId);
  std::swap(_note, swappable._note);
  std::swap(_setupTime, swappable._setupTime);
  std::swap(_alertingTime, swappable._alertingTime);
  std::swap(_connectTime, swappable._connectTime);
  std::swap(_disconnectTime, swappable._disconnectTime);
  std::swap(_sessionId, swappable._sessionId);
}

SBCCDRRecord& SBCCDRRecord::operator=(const SBCCDRRecord& copy)
{
  SBCCDRRecord swappable(copy);
  swap(swappable);
  return *this;
}


void SBCCDRRecord::toJson(json::Object& params)
{
  if (!_date.empty())
  {
    params["date"] = json::String(_date);
  }
  
  if (!_fromUri.empty())
  {
    params["from-uri"] = json::String(_fromUri);
  }
  
  if (!_toUri.empty())
  {
    params["to-uri"] = json::String(_toUri);
  }
  
  if (!_referUri.empty())
  {
    params["refer-uri"] = json::String(_referUri);
  }
  
  if (!_callerContact.empty())
  {
    params["caller-contact"] = json::String(_callerContact);
  }
  
  if (!_calledContact.empty())
  {
    params["called-contact"] = json::String(_calledContact);
  }
  
  if (!_srcAddress.empty())
  {
    params["src-address"] = json::String(_srcAddress);
  }
  
  if (!_dstAddress.empty())
  {
    params["dst-address"] = json::String(_dstAddress);
  }
  
  if (!_requestUri.empty())
  {
    params["request-uri"] = json::String(_requestUri);
  }
  
  if (!_errorResponse.empty())
  {
    params["error-response"] = json::String(_errorResponse);
  }
  
  if (!_callId.empty())
  {
    params["call-id"] = json::String(_callId);
  }
  
  if (!_sessionId.empty())
  {
    params["session-id"] = json::String(_sessionId);
  }
  
  if (!_note.empty())
  {
    params["note"] = json::String(_note);
  }
  
  if (_setupTime)
  {
    params["setup-time"] = json::Number(_setupTime);
  }
  
  if (_alertingTime)
  {
    params["alerting-time"] = json::Number(_alertingTime);
  }
  
  if (_connectTime)
  {
    params["connect-time"] = json::Number(_connectTime);
  }
  
  if (_disconnectTime)
  {
    params["disconnect-time"] = json::Number(_disconnectTime);
  }
}

bool SBCCDRRecord::writeToRedis(Persistent::RedisBroadcastClient& redis, const std::string& key, unsigned int expire)
{
  json::Object params;
  toJson(params);
  return redis.set(key, params, expire);
}

bool SBCCDRRecord::writeToLogFile(OSS::UTL::LogFile& logFile)
{  
  std::ostringstream strm;
  strm << _date << ", ";
  strm << _srcAddress << ", ";
  strm << _callId << ", ";
  strm << _sessionId <<  ", ";
  strm << _fromUri << ", ";
  strm << _callerContact << ", ";
  strm << _toUri << ", ";
  strm << _requestUri << ", ";
  strm << _errorResponse << ", ";
  strm << _setupTime << ", ";
  strm << _connectTime << ", ";
  strm << _disconnectTime;
  
  logFile.notice(strm.str());
  
  return true;
}

bool SBCCDRRecord::readFromRedis(Persistent::RedisBroadcastClient& redis, const std::string& key)
{
  json::Object response;
  if (!redis.get(key, response))
    return false;
  
  json::Object::iterator date = response.Find("date");
  if (date != response.End())
  {
    json::String element = date->element;
    _date = element.Value();
  }
  
  json::Object::iterator fromUri = response.Find("from-uri");
  if (fromUri != response.End())
  {
    json::String element = fromUri->element;
    _fromUri = element.Value();
  }
  
  json::Object::iterator toUri = response.Find("to-uri");
  if (toUri != response.End())
  {
    json::String element = toUri->element;
    _toUri = element.Value();
  }
  
  json::Object::iterator referUri = response.Find("refer-uri");
  if (referUri != response.End())
  {
    json::String element = referUri->element;
    _referUri = element.Value();
  }
  
  json::Object::iterator callerContact = response.Find("caller-contact");
  if (callerContact != response.End())
  {
    json::String element = callerContact->element;
    _callerContact = element.Value();
  }
  
  json::Object::iterator calledContact = response.Find("called-contact");
  if (calledContact != response.End())
  {
    json::String element = calledContact->element;
    _calledContact = element.Value();
  }
  
  json::Object::iterator srcAddress = response.Find("src-address");
  if (srcAddress != response.End())
  {
    json::String element = srcAddress->element;
    _srcAddress = element.Value();
  }
  
  json::Object::iterator dstAddress = response.Find("dst-address");
  if (dstAddress != response.End())
  {
    json::String element = dstAddress->element;
    _dstAddress = element.Value();
  }
  
  json::Object::iterator requestUri = response.Find("request-uri");
  if (requestUri != response.End())
  {
    json::String element = requestUri->element;
    _requestUri = element.Value();
  }
  
  json::Object::iterator errorResponse = response.Find("error-response");
  if (errorResponse != response.End())
  {
    json::String element = errorResponse->element;
    _errorResponse = element.Value();
  }
  
  json::Object::iterator callId = response.Find("call-id");
  if (callId != response.End())
  {
    json::String element = callId->element;
    _callId = element.Value();
  }
  
  json::Object::iterator sessionId = response.Find("session-id");
  if (sessionId != response.End())
  {
    json::String element = sessionId->element;
    _sessionId = element.Value();
  }
  
  json::Object::iterator note = response.Find("note");
  if (note != response.End())
  {
    json::String element = note->element;
    _note = element.Value();
  }
  
  json::Object::iterator setupTime = response.Find("setup-time");
  if (setupTime != response.End())
  {
    json::Number element = setupTime->element;
    _setupTime = element.Value();
  }
  
  json::Object::iterator alertingTime = response.Find("alerting-time");
  if (alertingTime != response.End())
  {
    json::Number element = alertingTime->element;
    _alertingTime = element.Value();
  }
  
  json::Object::iterator connectTime = response.Find("connect-time");
  if (connectTime != response.End())
  {
    json::Number element = connectTime->element;
    _connectTime = element.Value();
  }
  
  json::Object::iterator disconnectTime = response.Find("disconnect-time");
  if (disconnectTime != response.End())
  {
    json::Number element = disconnectTime->element;
    _disconnectTime = element.Value();
  }

  return true;
}

} } } // OSS::SIP::SBC
  
