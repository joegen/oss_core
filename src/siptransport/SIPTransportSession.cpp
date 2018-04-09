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


#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPListener.h"


namespace OSS {
namespace SIP {



SIPTransportSession::SIPTransportRateLimitStrategy SIPTransportSession::_rateLimit;
SIPTransportSession::SIPTransportRateLimitStrategy& SIPTransportSession::rateLimit()
{
  return SIPTransportSession::_rateLimit;
}

SIPTransportSession::SIPTransportSession(SIPListener* pListener) :
  _isReliableTransport(true),
  _pDispatch(0),
  _bytesTransferred(0),
  _bytesRead(0),
  _identifier(0),
  _isClient(false),
  _isEndpoint(false),
  _isConnected(false),
  _pListener(pListener)
{
}


SIPTransportSession::~SIPTransportSession()
{
}

void SIPTransportSession::dispatchMessage(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport)
{
  if (_messageDispatch)
  {
    if (_pListener && !_pListener->getTransportAlias().empty())
    {
      pMsg->setProperty(OSS::PropertyMap::PROP_TransportAlias, _pListener->getTransportAlias());
    }
    _messageDispatch(pMsg, pTransport);
  }
}

void SIPTransportSession::linkTransacton(const std::string& id)
{
  OSS::mutex_critic_sec_lock lock(_linkedTransactionsMutex);
  _linkedTransactions.insert(id);
}

void SIPTransportSession::unlinkTransaction(const std::string& id)
{
  OSS::mutex_critic_sec_lock lock(_linkedTransactionsMutex);
  _linkedTransactions.erase(id);
}

void SIPTransportSession::dispatchError(ErrorType type, const boost::system::error_code& e)
{
  OSS::mutex_critic_sec_lock lock(_linkedTransactionsMutex);
  if (_errorHandler)
  {
    for (LinkedTransactions::const_iterator iter = _linkedTransactions.begin(); iter != _linkedTransactions.end(); iter++)
    {
      _errorHandler(*iter, type, e);
    }
  }
}


} } // OSS::SIP
