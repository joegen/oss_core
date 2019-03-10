// OSS Software Solutions Application Programmer Interface
// Package: Karoo
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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


#ifndef _SBCEXEC_H
#define	_SBCEXEC_H


#include "OSS/OSS.h"
#include "OSS/SIP/SIPStack.h"
#include "OSS/UTL/Thread.h"
#include <iostream>
#include <sstream>

namespace OSS {
namespace SIP {
namespace SBC {


class SBCExec
  /// This is a helper class to easily construct a client UA
  /// capable of sending EXEC proprietary transactions to the SBC
{
public:
  SBCExec();

  ~SBCExec();

  void handleResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransacion);

  void initialize(OSS::Net::IPAddress& localAddress, const std::string& transport = "udp");

  void initializeFromConfig(const boost::filesystem::path& cfgFile, const std::string& transport = "udp");

  OSS::SIP::SIPMessage::Ptr execute(const OSS::Net::IPAddress& target, const std::string& cmd, const std::string& args);

  OSS::SIP::SIPMessage::Ptr execute(const std::string& cmd, const std::string& args);

protected:
  OSS::SIP::SIPStack _sip;
  boost::thread* _pThread;
  OSS::SIP::SIPMessage::Ptr _pResponse;
  OSS::SIP::SIPTransaction::Error _error;
  OSS::Net::IPAddress _localAddress;
  OSS::Net::IPAddress _targetAddress;
  std::string _transport;
  OSS::semaphore _sem;
};

} } } // OSS::SIP::SBC

#endif // _SBCEXEC_H



