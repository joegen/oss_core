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

#ifndef SIPB2BDIALOGDATA_H
#define	SIPB2BDIALOGDATA_H


#include "OSS/build.h"
#if ENABLE_FEATURE_B2BUA

#include <string>
#include <vector>
#include <list>

#include "OSS/OSS.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
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
  
  static std::string escapeString(const std::string& s)
  {
    std::ostringstream strm;

    std::string::const_iterator it(s.begin()),
                                itEnd(s.end());
    for (; it != itEnd; ++it)
    {
       switch (*it)
       {
          case '"':         strm << "\\\"";   break;
          case '\\':        strm << "\\\\";   break;
          case '\b':        strm << "\\b";    break;
          case '\f':        strm << "\\f";    break;
          case '\n':        strm << "\\n";    break;
          case '\r':        strm << "\\r";    break;
          case '\t':        strm << "\\t";    break;
          default:          strm << *it;       break;
       }
    }
    return strm.str();
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

#if 0
    [DEBUG] SIPB2BDialogStateManager::updateDialog: 
{
	"sessionId" : "4147414987334546318926127303",
	"leg1" : {
		"dialogId" : "",
		"callId" : "OTU2YWMzOGZiMDJkM2Q2NmRiZmNhNTk0OTQ1MDk3ZTA.",
		"from" : "<sip:32017@ezuce.com;transport=UDP>;tag=Xg67jHjDemXjF",
		"to" : "<sip:2017@ezuce.com;transport=UDP>;tag=2ed94c23",
		"remoteContact" : "<sip:2017@192.168.1.10:58959;transport=UDP>",
		"localContact" : "2017 <sip:4147414987334546318926127303-1@192.168.1.10:5060;transport=udp>",
		"localRecordRoute" : "",
		"remoteIp" : "192.168.1.10:58959",
		"transportId" : "0",
		"targetTransport" : "udp",
		"localSdp" : "v=0\r\no=FreeSWITCH 1410664034 1410664035 IN IP4 172.31.1.9\r\ns=FreeSWITCH\r\nc=IN IP4 107.23.34.40\r\nt=0 0\r\nm=audio 30466 RTP/AVP 0 101\r\nc=IN IP4 107.23.34.40\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\na=ptime:20\r\na=x-sipx-ntap:X172.31.1.9-107.23.34.40;2296\r\n",
		"remoteSdp" : "v=0\r\no=Z 0 0 IN IP4 192.168.1.10\r\ns=Z\r\nc=IN IP4 192.168.1.10\r\nt=0 0\r\nm=audio 8000 RTP/AVP 3 110 8 0 98 101\r\na=rtpmap:110 speex/8000\r\na=rtpmap:98 iLBC/8000\r\na=fmtp:98 mode=20\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-15\r\na=sendrecv\r\n",
		"encryption" : "",
		"noRtpProxy" : false,
		"localCSeq" : 0,
		"routeSet" : []
	},
	"leg2" : {
		"dialogId" : "",
		"callId" : "OTU2YWMzOGZiMDJkM2Q2NmRiZmNhNTk0OTQ1MDk3ZTA.",
		"from" : "<sip:2017@ezuce.com;transport=UDP>;tag=2ed94c23",
		"to" : "<sip:32017@ezuce.com;transport=UDP>;tag=Xg67jHjDemXjF",
		"remoteContact" : "<sip:32017@172.31.1.9:15060;transport=udp>",
		"localContact" : "2017 <sip:4147414987334546318926127303-2@192.168.1.10:5060;transport=udp>",
		"localRecordRoute" : "",
		"remoteIp" : "107.23.34.40:5060",
		"transportId" : "0",
		"targetTransport" : "udp",
		"localSdp" : "v=0\r\no=Z 0 0 IN IP4 192.168.1.10\r\ns=Z\r\nc=IN IP4 192.168.1.10\r\nt=0 0\r\nm=audio 8000 RTP/AVP 3 110 8 0 98 101\r\na=rtpmap:110 speex/8000\r\na=rtpmap:98 iLBC/8000\r\na=fmtp:98 mode=20\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-15\r\na=sendrecv\r\n",
		"remoteSdp" : "v=0\r\no=FreeSWITCH 1410664034 1410664035 IN IP4 172.31.1.9\r\ns=FreeSWITCH\r\nc=IN IP4 107.23.34.40\r\nt=0 0\r\nm=audio 30466 RTP/AVP 0 101\r\nc=IN IP4 107.23.34.40\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\na=ptime:20\r\na=x-sipx-ntap:X172.31.1.9-107.23.34.40;2296\r\n",
		"encryption" : "",
		"noRtpProxy" : false,
		"localCSeq" : 2,
		"routeSet" : [
			"<sip:107.23.34.40:5060;lr;sipXecs-CallDest=AL%2CAL;sipXecs-rs=%2Aauth%7E.%2Afrom%7EMmVkOTRjMjM%60.900_ntap%2Aid%7EMjYzNzctMjYyMg%60%60%21e5cb43f4b1b2d273f28cf762d29bc25f;sipXecs-User=joegenconf;x-sipX-done>"
		]
	},
	"timeStamp" : 1410675320642,
	"connectTime" : 1410675320642,
	"disconnectTime" : 1410675320642,
	"sessionAge" : 1410675320642
}

    
#endif
    
    void toJsonString(std::string& json) const
    {
      std::ostringstream strm;
      strm << "{" ;
      strm << "dialogId : " << "\"" << dialogId << "\"" ;
      strm << "," << "callId : " << "\"" << callId << "\"" ;
      strm << "," << "from : " << "\"" << JsonHelper::escapeString(from) << "\"" ;
      strm << "," << "to : " << "\"" << JsonHelper::escapeString(to) << "\"" ;
      strm << "," << "remoteContact : " << "\"" << JsonHelper::escapeString(remoteContact) << "\"" ;
      strm << "," << "localContact : " << "\"" << JsonHelper::escapeString(localContact) << "\"" ;
      strm << "," << "localRecordRoute : " << "\"" << localRecordRoute << "\"" ;
      strm << "," << "remoteIp : " << "\"" << remoteIp << "\"" ;
      strm << "," << "transportId : " << "\"" << transportId << "\"" ;
      strm << "," << "targetTransport : " << "\"" << targetTransport << "\"" ;
      strm << "," << "localSdp : " << "\"" << JsonHelper::escapeString(localSdp) << "\"" ;
      strm << "," << "remoteSdp : " << "\"" << JsonHelper::escapeString(remoteSdp) << "\"" ;
      strm << "," << "encryption : " << "\"" << encryption << "\"" ;
      strm << "," << "noRtpProxy : " << (noRtpProxy ? "true" : "false");
      strm << "," << "localCSeq : " << localCSeq;
      
      if (!routeSet.empty())
      {
        strm << "," << "routeSet : [";
        std::vector<std::string>::const_iterator iter = routeSet.begin();
        strm << "" << "\"" << JsonHelper::escapeString(*iter) << "\"";
        iter++;
        for (;iter != routeSet.end(); iter++)
          strm << "," << "\"" << JsonHelper::escapeString(*iter) << "\"";
        strm << "]";
      }
      
      strm << "}";
      json = strm.str();
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
      catch(json::Exception& e)
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
      catch(json::Exception& e)
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
    expires = copy.expires;
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
    std::swap(expires, copy.expires);
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
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
  }

  void toJsonString(std::string& object) const
  {
#if 0
    json::Object jObject;
    toJsonObject(jObject);
    try
    {
      std::ostringstream strm;
      json::Writer::Write(jObject, strm);
      object = strm.str();
    }
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
#endif
    
    std::string sleg1;
    std::string sleg2;
    leg1.toJsonString(sleg1);
    leg2.toJsonString(sleg2);
    
    std::ostringstream strm;
      strm << "{" ;
      strm << "" << "sessionId : " << "\"" << sessionId << "\"" ;
      strm << "," << "timeStamp : " << timeStamp;
      strm << "," << "connectTime : " << connectTime;
      strm << "," << "disconnectTime : " << disconnectTime;
      strm << "," << "sessionAge : " << sessionAge;
      strm << "," << "leg1 : " << sleg1;
      strm << "," << "leg2 : " << sleg2;
        
    strm << "}";
    object = strm.str();
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
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BDialogData: Unable to parse json object - " << e.what());
    }
  }

  void fromJsonString(const std::string& jsonString)
  {
    try
    {
      json::Object object;
      std::stringstream strm;
      strm << jsonString;
      json::Reader::Read(object, strm);
      fromJsonObject(object);
    }
    catch(json::Exception& e)
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
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BRegData: Unable to parse json object - " << e.what());
    }
  }

  void toJsonString(std::string& object) const
  {
    json::Object jObject;
    toJsonObject(jObject);
    try
    {
      std::ostringstream strm;
      json::Writer::Write(jObject, strm);
      object = strm.str();
    }
    catch(json::Exception& e)
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
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BRegData: Unable to parse json object - " << e.what());
    }
  }

  void fromJsonString(const std::string& jsonString)
  {
    try
    {
      json::Object object;
      std::stringstream strm;
      strm << jsonString;
      json::Reader::Read(object, strm);
      fromJsonObject(object);
    }
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("SIPB2BRegData: Unable to parse json object - " << e.what());
    }
  }
};

typedef SIPB2BRegData RegData;
typedef std::vector<RegData> RegList;


} } }// OSS::SIP::B2BUA


#endif // ENABLE_FEATURE_B2BUA

#endif	/* SIPB2BDIALOGDATA_H */

