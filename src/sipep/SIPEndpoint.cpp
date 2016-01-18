
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
  
  
SIPEndpoint::SIPEndpoint()
{
  _stack.setRequestHandler(boost::bind(&SIPEndpoint::handleRequest, this, _1, _2, _3));
  _stack.setAckFor2xxTransactionHandler(boost::bind(&SIPEndpoint::handleAckFor2xxTransaction, this, _1, _2));
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

bool SIPEndpoint::runEndpoint()
{
  try 
  {
    _stack.run();
  }
  catch(...)
  {
    return false;
  }
  
  return true;
}

void SIPEndpoint::stopEndpoint()
{
  _stack.stop();
}

void SIPEndpoint::handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
    
}

 void SIPEndpoint::handleAckFor2xxTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport)
 {
     
 }



} } } // OSS::SIP::EP