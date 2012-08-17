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

  std::string sessionId;
  LegInfo leg1;
  LegInfo leg2;
};

} } // OSS::SIP::B2BUA

#endif	/* SIPB2BDIALOGDATA_H */

