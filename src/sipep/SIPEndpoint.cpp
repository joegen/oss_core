
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


#include "OSS/SIP/EP/SIPEndpoint.h"


namespace OSS {
namespace SIP {
namespace EP {
  
  
using namespace OSS::SIP::B2BUA;
  
SIPEndpoint::SIPEndpoint() :
  _transactionManager(2, 1024)
{
  _transactionManager.addUserAgentHandler(this);
}

SIPEndpoint::~SIPEndpoint()
{
}

bool SIPEndpoint::addTransport(TransportType transportType, unsigned short port)
{
  return false;
}

bool SIPEndpoint::addTransport(TransportType transportType, const std::string& address, unsigned short port)
{
  return false;
}

bool SIPEndpoint::addTransport(TransportType transportType, const OSS::Net::IPAddress& address)
{
  return false;
}

bool SIPEndpoint::run()
{
  try 
  {
    _transactionManager.stack().run();
  }
  catch(...)
  {
    return false;
  }
}

void SIPEndpoint::stop()
{
  _transactionManager.stack().stop();
}

SIPB2BUserAgentHandler::Action SIPEndpoint::handleRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  return OSS::SIP::B2BUA::SIPB2BUserAgentHandler::Handled;
}


} } } // OSS::SIP::EP