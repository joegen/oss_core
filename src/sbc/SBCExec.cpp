
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


#include "OSS/SIP/SBC/SBCExec.h"

#include "OSS/Net/IPAddress.h"
#include "OSS/SIP/SIPStack.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {


SBCExec::SBCExec() :
  _pThread(0),
  _sem(0, 0xFFFF)
{
}

SBCExec::~SBCExec()
{
  //_sip.stop();
  
  if (_pThread)
  {
    _pThread->join();
    delete _pThread;
  }
}

void SBCExec::handleResponse(
  const OSS::SIP::SIPTransaction::Error& e,
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  const OSS::SIP::SIPTransaction::Ptr& pTransacion)
{
  if (e)
  {
    std::cerr << "Error Encountered:  " << e->what() << std::endl;
  }
  else if (pMsg)
  {
    _pResponse = pMsg;
   
  }
  _sem.set();
}

void SBCExec::initializeFromConfig(const boost::filesystem::path& cfgFile, const std::string& transport)
{
  OSS::Persistent::ClassType config;

  if (!config.load(boost_path(cfgFile)))
    return;
  OSS::Persistent::DataType root = config.self();
  OSS::Persistent::DataType listeners = root["listeners"];
  OSS::Persistent::DataType defaultIface = listeners["default-interface-address"];
  OSS::Persistent::DataType defaultPort = listeners["default-interface-port"];
  
  OSS::Net::IPAddress localAddress((const char*)defaultIface);
  _targetAddress = localAddress;
  _targetAddress.setPort((int)defaultPort);
  //std::cout << "SBCExec using localAddress " << (const char*)defaultIface << std::endl;
  initialize(localAddress, transport);
}

void SBCExec::initialize(OSS::Net::IPAddress& localAddress,
  const std::string& transport)
{
  if (!localAddress.isLocalAddress())
  {
    OSS_LOG_ERROR("SBCExec encountered an exception while intitializing SIP transport. " << localAddress.toIpPortString() << " is not a valid address");
    //std::cerr << "SBCExec configured local address is invalid!" << std::endl;
    return;
  }

  _sip.enableUDP() = false;
  _sip.enableTCP() = false;
  _sip.enableTLS() = false;

  if (transport == "udp")
    _sip.enableUDP() = true;
  else if(transport == "tcp")
    _sip.enableTCP() = true;
  else if (transport == "tls")
    _sip.enableTLS() = true;
  else
    _sip.enableUDP() = true;

  //
  // Check if this is a real interface on this server
  //


  _sip.udpListeners().push_back(localAddress);

  try
  {
    _sip.transportInit(15060, 16060, 15060, 16060, 15062, 15062, 15061, 16061);
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR("SBCExec encountered an exception while intitializing SIP transport " << e.what());
  }

  if (_sip.udpListeners().size() > 0)
  {
    _localAddress = _sip.udpListeners().front();
    _transport = "UDP";
  }
  else if (_sip.tcpListeners().size() > 0)
  {
    _localAddress = _sip.tcpListeners().front();
    _transport = "TCP";
  }
  else if (_sip.tlsListeners().size() > 0)
  {
    _localAddress = _sip.tlsListeners().front();
    _transport = "TLS";
  }

  _pThread = new boost::thread(boost::bind(&OSS::SIP::SIPStack::run, &_sip));
  OSS::thread_sleep(1000); /// yield so SIP stack initializes
}

OSS::SIP::SIPMessage::Ptr SBCExec::execute(const std::string& cmd, const std::string& args)
{
  return execute(_targetAddress, cmd, args);
}

OSS::SIP::SIPMessage::Ptr SBCExec::execute(const OSS::Net::IPAddress& target,
  const std::string& cmd, const std::string& args)
{
  std::ostringstream msg;

  OSS_LOG_NOTICE("SBCExec sending RPC to " << target.toIpPortString() << " via local transport " << _transport << " " << _localAddress.toIpPortString());


  try
  {
    if (_transport == "TLS")
      msg << "EXEC sips:" << target.toIpPortString() << " SIP/2.0\r\n";
    else if (_transport == "TCP")
      msg << "EXEC sip:" << target.toIpPortString() << ";transport=tcp SIP/2.0\r\n";
    else
      msg << "EXEC sip:" << target.toIpPortString() << " SIP/2.0\r\n";

    msg << "To: <sip:" << target.toIpPortString() << ">\r\n";

    msg << "From: <sip:" << _localAddress.toIpPortString() << ">;tag="
            << OSS::SIP::SIPParser::createTagString() << "\r\n";

    msg << "Via: SIP/2.0/" << _transport << " " << _localAddress.toIpPortString()
            << ";branch=" << OSS::SIP::SIPParser::createBranchString()
            << "\r\n";

    msg << "Call-ID: " << OSS::SIP::SIPParser::createTagString() << "\r\n";

    msg << "CSeq: 1 EXEC\r\n";

    msg << "Contact: <sip:" << _localAddress.toIpPortString() << ">\r\n";

    msg << "Cmd: " << cmd << "\r\n";

    if (!args.empty())
      msg << "Args: " << args << "\r\n";

    msg << "Content-Length: 0\r\n\r\n";

    OSS::SIP::SIPMessage::Ptr exec(new OSS::SIP::SIPMessage(msg.str()));
    exec->setProperty("transaction-timeout", "10000");
    _sip.sendRequest(exec, _localAddress, target,
            boost::bind(&SBCExec::handleResponse, this, _1, _2, _3, _4), OSS::SIP::SIPTransaction::TerminateCallback());
  }
  catch(std::exception& e)
  {
    std::cout << "SBCExec caught and exception while sending request:  " << e.what();
  }

  _sem.wait();
  _sip.stop();

  return _pResponse;
}

} } } // OSS::SIP::SBC




