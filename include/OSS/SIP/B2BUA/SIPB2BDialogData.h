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
#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"



namespace OSS {
namespace SIP {
namespace B2BUA {


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
        dialogId = ((json::String)object["dialogId"]).Value();
        callId = ((json::String)object["callId"]).Value();
        from = ((json::String)object["from"]).Value();
        to = ((json::String)object["to"]).Value();
        remoteContact = ((json::String)object["remoteContact"]).Value();
        localContact = ((json::String)object["localContact"]).Value();
        localRecordRoute = ((json::String)object["localRecordRoute"]).Value();
        remoteIp = ((json::String)object["remoteIp"]).Value();
        transportId = ((json::String)object["transportId"]).Value();
        targetTransport = ((json::String)object["targetTransport"]).Value();
        localSdp = ((json::String)object["localSdp"]).Value();
        remoteSdp = ((json::String)object["remoteSdp"]).Value();
        encryption = ((json::String)object["encryption"]).Value();
        noRtpProxy = ((json::Boolean)object["noRtpProxy"]).Value();
        localCSeq = ((json::Number)object["localCSeq"]).Value();

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
      sessionId = ((json::String)object["sessionId"]).Value();
      timeStamp = ((json::Number)object["timeStamp"]).Value();
      connectTime = ((json::Number)object["connectTime"]).Value();
      disconnectTime = ((json::Number)object["disconnectTime"]).Value();
      sessionAge = ((json::Number)object["sessionAge"]).Value();
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

  SIPB2BRegData()
  {
    expires = 0;
    enc = false;
  }

  void toJsonObject(json::Object& object) const
  {
    try
    {
      object["key"] = json::String(key);
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
      key = ((json::String&)object["key"]).Value();
      contact = ((json::String&)object["contact"]).Value();
      packetSource = ((json::String&)object["packetSource"]).Value();
      localInterface = ((json::String&)object["localInterface"]).Value();
      transportId = ((json::String&)object["transportId"]).Value();
      targetTransport = ((json::String&)object["targetTransport"]).Value();
      aor = ((json::String&)object["aor"]).Value();
      expires = ((json::Number&)object["expires"]).Value();
      enc = ((json::Boolean&)object["enc"]).Value();
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

