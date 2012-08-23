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
  };

 
  SIPB2BDialogData()
  {
    timeStamp = OSS::getTime();
    connectTime = timeStamp;
    disconnectTime = timeStamp;
    sessionAge = timeStamp;
  }

  std::string sessionId;
  LegInfo leg1;
  LegInfo leg2;
  OSS::UInt64 timeStamp;
  OSS::UInt64 connectTime;
  OSS::UInt64 disconnectTime;
  OSS::UInt64 sessionAge;
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
};

typedef SIPB2BRegData RegData;
typedef std::vector<RegData> RegList;

} } }// OSS::SIP::B2BUA

#endif	/* SIPB2BDIALOGDATA_H */

