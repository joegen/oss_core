// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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

#ifndef SIPB2BDIALOGDATA_H
#define	SIPB2BDIALOGDATA_H


#include <string>
#include <vector>
#include <list>

#include "OSS/OSS.h"
#include "OSS/Core.h"
#include "OSS/Logger.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"



namespace OSS {
namespace SIP {
namespace B2BUA {

struct JsonHelper
{
  static std::string toString(const json::UnknownElement& obj)
  {
    json::String jsonString = obj;
    return jsonString.Value();
  }

  static double toNumber(const json::UnknownElement& obj)
  {
    json::Number jsonNumber = obj;
    return jsonNumber.Value();
  }

  static double toBoolean(const json::UnknownElement& obj)
  {
    json::Boolean jsonBool = obj;
    return jsonBool.Value();
  }
};

struct SIPB2BDialogData
{
public:
  struct LegInfo
  {
    std::string dialogId;
    std::string callId;
    std::string from;
    std::string to;
    std::string remoteContact;
    std::string localContact;
    std::string localRecordRoute;
    std::string remoteIp;
    std::string transportId;
    std::string targetTransport;
    std::string localSdp;
    std::string remoteSdp;
    std::vector<std::string> routeSet;
    std::string encryption;
    bool noRtpProxy;
    unsigned long localCSeq;

    LegInfo()
    {
      noRtpProxy = false;
      localCSeq = 0;
    }

    LegInfo(const LegInfo& copy)
    {
      dialogId = copy.dialogId;
      callId = copy.callId;
      from = copy.from;
      to = copy.to;
      remoteContact = copy.remoteContact;
      localContact = copy.localContact;
      localRecordRoute = copy.localRecordRoute;
      remoteIp = copy.remoteIp;
      transportId = copy.transportId;
      targetTransport = copy.targetTransport;
      localSdp = copy.localSdp;
      remoteSdp = copy.remoteSdp;
      routeSet = copy.routeSet;
      encryption = copy.encryption;
      noRtpProxy = copy.noRtpProxy;
      localCSeq = copy.localCSeq;
    }

    void swap(LegInfo& copy)
    {
      std::swap(dialogId, copy.dialogId);
      std::swap(callId, copy.callId);
      std::swap(from, copy.from);
      std::swap(to, copy.to);
      std::swap(remoteContact, copy.remoteContact);
      std::swap(localContact, copy.localContact);
      std::swap(localRecordRoute, copy.localRecordRoute);
      std::swap(remoteIp, copy.remoteIp);
      std::swap(transportId, copy.transportId);
      std::swap(targetTransport, copy.targetTransport);
      std::swap(localSdp, copy.localSdp);
      std::swap(remoteSdp, copy.remoteSdp);
      std::swap(routeSet, copy.routeSet);
      std::swap(encryption, copy.encryption);
      std::swap(noRtpProxy, copy.noRtpProxy);
      std::swap(localCSeq, copy.localCSeq);
    }

    LegInfo& operator=(const LegInfo& copy)
    {
      LegInfo clone(copy);
      swap(clone);
      return *this;
    }

    void toJsonObject(json::Object& object) const
    {
      try
      {
        object["dialogId"] = json::String(dialogId);
        object["callId"] = json::String(callId);
        object["from"] = json::String(from);
        object["to"] = json::String(to);
        object["remoteContact"] = json::String(remoteContact);
        object["localContact"] = json::String(localContact);
        object["localRecordRoute"] = json::String(localRecordRoute);
        object["remoteIp"] = json::String(remoteIp);
        object["transportId"] = json::String(transportId);
        object["targetTransport"] = json::String(targetTransport);
        object["localSdp"] = json::String(localSdp);
        object["remoteSdp"] = json::String(remoteSdp);
        object["encryption"] = json::String(encryption);
        object["noRtpProxy"] = json::Boolean(noRtpProxy);
        object["localCSeq"] = json::Number(localCSeq);

        json::Array routes;
        for (std::vector<std::string>::const_iterator iter = routeSet.begin(); iter != routeSet.end(); iter++)
          routes.Insert(json::String(*iter));
        object["routeSet"] = routes;
      }
      catch(json::Exception e)
      {
        OSS_LOG_ERROR("SIPB2BDialogData::LegInfo: Unable to parse json object - " << e.what());
      }
    }

    void fromJsonObject(const json::Object& object)
    {
      try
      {
        dialogId = JsonHelper::toString(object["dialogId"]);
        callId = JsonHelper::toString(object["callId"]);
        from = JsonHelper::toString(object["from"]);
        to = JsonHelper::toString(object["to"]);
        remoteContact = JsonHelper::toString(object["remoteContact"]);
        localContact = JsonHelper::toString(object["localContact"]);
        localRecordRoute = JsonHelper::toString(object["localRecordRoute"]);
        remoteIp = JsonHelper::toString(object["remoteIp"]);
        transportId = JsonHelper::toString(object["transportId"]);
        targetTransport = JsonHelper::toString(object["targetTransport"]);
        localSdp = JsonHelper::toString(object["localSdp"]);
        remoteSdp = JsonHelper::toString(object["remoteSdp"]);
        encryption = JsonHelper::toString(object["encryption"]);
        noRtpProxy = JsonHelper::toBoolean(object["noRtpProxy"]);
        localCSeq = JsonHelper::toNumber(object["localCSeq"]);

        json::Array routes = object["routeSet"];
        for (json::Array::iterator iter = routes.Begin(); iter != routes.End(); iter++)
          routeSet.push_back(((json::String)*iter).Value());
      }
      catch(json::Exception e)
      {
        OSS_LOG_ERROR("SIPB2BDialogData::LegInfo: Unable to parse json object - " << e.what());
      }
    }
  };


  std::string sessionId;
  LegInfo leg1;
  LegInfo leg2;
  OSS::UInt64 timeStamp;
  OSS::UInt64 connectTime;
  OSS::UInt64 disconnectTime;
  OSS::UInt64 sessionAge;
  //
  // Subscription dialog
  //
  std::string event;
  int expires;

  SIPB2BDialogData()
  {
    timeStamp = OSS::getTime();
    connectTime = timeStamp;
    disconnectTime = timeStamp;
    sessionAge = timeStamp;
    expires = 0;
  }

  SIPB2BDialogData(const SIPB2BDialogData& copy)
  {
    sessionId = copy.sessionId;
    leg1 = copy.leg1;
    leg2 = copy.leg2;
    timeStamp = copy.timeStamp;
    connectTime = copy.connectTime;
    disconnectTime = copy.disconnectTime;
    sessionAge = copy.sessionAge;
  }

  void swap(SIPB2BDialogData& copy)
  {
    std::swap(sessionId, copy.sessionId);
    std::swap(leg1, copy.leg1);
    std::swap(leg2, copy.leg2);
    std::swap(timeStamp, copy.timeStamp);
    std::swap(connectTime, copy.connectTime);
    std::swap(disconnectTime, copy.disconnectTime);
    std::swap(sessionAge, copy.sessionAge);
  }

  SIPB2BDialogData& operator=(const SIPB2BDialogData& copy)
  {
    SIPB2BDialogData clone(copy);
    swap(clone);
    return *this;
  }

  void toJsonObject(json::Object& object) const
  {
    try
    {
      json::Object leg1Object;
      leg1.toJsonObject(leg1Object);
      json::Object leg2Object;
      leg2.toJsonObject(leg2Object);

      object["sessionId"] = json::String(sessionId);
      object["leg1"] = leg1Object;
      object["leg2"] = leg2Object;
      object["timeStamp"] = json::Number(timeStamp);
      object["connectTime"] = json::Number(connectTime);
      object["disconnectTime"] = json::Number(disconnectTime);
      object["sessionAge"] = json::Number(sessionAge);
    }
    catch(json::Exception e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
  }
 
  void fromJsonObject(const json::Object& object)
  { 
    try
    {
      json::Object leg1Object = object["leg1"];
      json::Object leg2Object = object["leg2"];
      leg1.fromJsonObject(leg1Object);
      leg2.fromJsonObject(leg2Object);
      sessionId = JsonHelper::toString(object["sessionId"]);
      timeStamp = JsonHelper::toNumber(object["timeStamp"]);
      connectTime = JsonHelper::toNumber(object["connectTime"]);
      disconnectTime = JsonHelper::toNumber(object["disconnectTime"]);
      sessionAge = JsonHelper::toNumber(object["sessionAge"]);
    }
    catch(json::Exception e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
  }
};

typedef SIPB2BDialogData DialogData;
typedef std::list<DialogData> DialogList;

struct SIPB2BRegData
{
  std::string contact;
  std::string packetSource;
  std::string localInterface;
  std::string transportId;
  std::string targetTransport;
  std::string aor;
  int expires;
  bool enc;
  std::string key;
  std::string callId;

  SIPB2BRegData()
  {
    expires = 0;
    enc = false;
  }

  SIPB2BRegData(const SIPB2BRegData& copy)
  {
    contact = copy.contact;
    packetSource = copy.packetSource;
    localInterface = copy.localInterface;
    transportId = copy.transportId;
    targetTransport = copy.targetTransport;
    aor = copy.aor;
    expires = copy.expires;
    enc = copy.enc;
    key = copy.key;
    callId = copy.callId;
  }

  void swap(SIPB2BRegData& copy)
  {
    std::swap(contact, copy.contact);
    std::swap(packetSource, copy.packetSource);
    std::swap(localInterface, copy.localInterface);
    std::swap(transportId, copy.transportId);
    std::swap(targetTransport, copy.targetTransport);
    std::swap(aor, copy.aor);
    std::swap(expires, copy.expires);
    std::swap(enc, copy.enc);
    std::swap(key, copy.key);
    std::swap(callId, copy.callId);
  }

  SIPB2BRegData& operator=(const SIPB2BRegData& copy)
  {
    SIPB2BRegData clone(copy);
    swap(clone);
    return *this;
  }

  void toJsonObject(json::Object& object) const
  {
    try
    {
      object["key"] = json::String(key);
      object["callId"] = json::String(callId);
      object["contact"] = json::String(contact);
      object["packetSource"] = json::String(packetSource);
      object["localInterface"] = json::String(localInterface);
      object["transportId"] = json::String(transportId);
      object["targetTransport"] = json::String(targetTransport);
      object["aor"] = json::String(aor);
      object["expires"] = json::Number(expires);
      object["enc"] = json::Boolean(enc);

    }
    catch(json::Exception e)
    {
      OSS_LOG_ERROR("SIPB2BRegData: Unable to parse json object - " << e.what());
    }
  }

  void fromJsonObject(const json::Object& object)
  {
    try
    {
      key = JsonHelper::toString(object["key"]);
      callId = JsonHelper::toString(object["callId"]);
      contact = JsonHelper::toString(object["contact"]);
      packetSource = JsonHelper::toString(object["packetSource"]);
      localInterface = JsonHelper::toString(object["localInterface"]);
      transportId = JsonHelper::toString(object["transportId"]);
      targetTransport = JsonHelper::toString(object["targetTransport"]);
      aor = JsonHelper::toString(object["aor"]);
      expires = JsonHelper::toNumber(object["expires"]);
      enc = JsonHelper::toBoolean(object["enc"]);
    }
    catch(json::Exception e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
  }
};

typedef SIPB2BRegData RegData;
typedef std::vector<RegData> RegList;




} } }// OSS::SIP::B2BUA

#endif	/* SIPB2BDIALOGDATA_H */

