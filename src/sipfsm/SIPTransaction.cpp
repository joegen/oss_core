// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
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


#include "OSS/Logger.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPTransactionPool.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPXOR.h"
#include "OSS/SIP/SIPFrom.h"


namespace OSS {
namespace SIP {


SIPTransaction::SIPTransaction():
  _type(TYPE_UNKNOWN),
  _owner(0),
  _transportService(0),
  _state(TRN_STATE_IDLE),
  _localAddress(),
  _remoteAddress(),
  _sendAddress(),
  _dialogTarget(),
  _willSendAckFor2xx(false),
  _isXOREncrypted(false),
  _isInitialRequest(true),
  _isChildTransaction(false)
{
}
	
SIPTransaction::~SIPTransaction()
{
  //std::ostringstream logMsg;
  //logMsg << _logId << "Transaction " << _id << " DESTROYED";
  //OSS::log_debug(logMsg.str());
}

SIPTransaction::SIPTransaction(const SIPTransaction&)
{
}

SIPTransaction& SIPTransaction::operator = (const SIPTransaction&)
{
  return *this;
}

SIPTransactionPool*& SIPTransaction::owner()
{
  return _owner;
}

void SIPTransaction::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  OSS::mutex_lock lock(_mutex);

  if (_isInitialRequest)
  {
    _isInitialRequest = false;

    if (_logId.empty())
      _logId = pMsg->createContextId(true);

    if (!_transport)
      _transport = pTransport;

    if (!_localAddress.isValid())
      _localAddress = pTransport->getLocalAddress();

    if (!_remoteAddress.isValid())
      _remoteAddress = pTransport->getRemoteAddress();

    if (SIPXOR::isEnabled())
    {
      std::string isXOR;
      _isXOREncrypted = pMsg->getProperty("xor", isXOR) && isXOR == "1";
    }
  }

  std::ostringstream logMsg;
  logMsg << _logId << "<<< " << pMsg->startLine()
  << " LEN: " << pTransport->getLastReadCount()
  << " SRC: " << _remoteAddress.toIpPortString()
  << " DST: " << _localAddress.toIpPortString()
  << " EXT: " << "[" << pTransport->getExternalAddress() << "]"
  << " FURI: " << pMsg->hdrGet("from") 
  << " ENC: " << _isXOREncrypted
  << " PROT: " << pTransport->getTransportScheme();
  OSS::log_information(logMsg.str());

  if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
    OSS::log_debug(pMsg->createLoggerData());
  
  if (_fsm)
    _fsm->onReceivedMessage(pMsg, pTransport);
}

void SIPTransaction::sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    SIPTransaction::Callback callback)
{
  OSS::mutex_lock lock(_mutex);

  if (_isInitialRequest)
  {
    _isInitialRequest = false;
    if (_logId.empty())
      _logId = pRequest->createContextId(true);
    
    if (SIPXOR::isEnabled() && !_isXOREncrypted)
    {
      std::string isXOR;
      _isXOREncrypted = pRequest->getProperty("xor", isXOR) && isXOR == "1";
    }
  }

  if (!_responseTU)
    _responseTU = callback;

  if (_localAddress.getPort() == 0)
    _localAddress = localAddress;

  if (_remoteAddress.getPort() == 0)
    _remoteAddress = remoteAddress;

  if (!_transport)
  {
    if (!_transportService)
      throw OSS::SIP::SIPException("Transport Not Ready!");


    std::string transport;
    if (pRequest->getProperty("target-transport", transport))
    {
      std::string transportId;
      pRequest->getProperty("transport-id", transportId);
      _transport = _transportService->createClientTransport(localAddress, remoteAddress, transport, transportId);
    }else if (SIPVia::msgGetTopViaTransport(pRequest.get(), transport))
    {
      _transport = _transportService->createClientTransport(localAddress, remoteAddress, transport);
    }
    if (!_transport)
      throw OSS::SIP::SIPException("Unable to create transport!");
  }

  if (_transport->isReliableTransport())
  {
    writeMessage(pRequest);
  }
  else
  {
    writeMessage(pRequest, remoteAddress);
  }
}

void SIPTransaction::sendAckFor2xx(
  const SIPMessage::Ptr& pAck,
  const OSS::IPAddress& dialogTarget)
{
  OSS::mutex_lock lock(_mutex);
  if (!_dialogTarget.isValid())
    _dialogTarget = dialogTarget;

  if (SIPXOR::isEnabled() && _isXOREncrypted)
  {
    pAck->setProperty("xor", "1");
  }

  if (_transport->isReliableTransport())
  {
    writeMessage(pAck);
  }
  else
  {
    writeMessage(pAck, dialogTarget);
  }
}

void SIPTransaction::sendResponse(
      const SIPMessage::Ptr& pRequest,
      const OSS::IPAddress& sendAddress,
      SIPTransaction::Callback callback)
{
  if (!_ackHandler)
    _ackHandler = callback;
  sendResponse(pRequest, sendAddress);
}

void SIPTransaction::sendResponse(
  const SIPMessage::Ptr& pResponse,
  const OSS::IPAddress& sendAddress)
{
  if (!pResponse->isResponse())
    throw OSS::SIP::SIPException("Sending a REQUEST using sendResponse() is illegal!");

  if (!_transport)
    throw OSS::SIP::SIPException("Transport Not Ready!");

  if (_sendAddress.getPort() == 0)
    _sendAddress = sendAddress;

  if (_transport->isReliableTransport())
  {
    if (_transport->writeKeepAlive())
    {
      writeMessage(pResponse);
    }
    else
    {
      //
      // Keep-alive failed so create a new transport
      //
      if (_localAddress.isValid() && _sendAddress.isValid())
      {
        //
        // According to RFC 3261, if there is any transport failure, we must try to
        // re-estabish a connectoin to the via sentby parameter instead
        //
        std::string transport;
        if (SIPVia::msgGetTopViaTransport(pResponse.get(), transport))
        {
          _transport = _transportService->createClientTransport(_localAddress, _sendAddress, transport);
          writeMessage(pResponse);
        }
      }
      else
      {
        OSS_LOG_ERROR("SIPTransaction::sendResponse - Uanble to re-establish transport to send response.");
      }

    }
  }
  else
  {
    //
    // This is UDP so a keep-alive check won't do us any good
    //
    writeMessage(pResponse, _sendAddress);
  }
}


void SIPTransaction::writeMessage(SIPMessage::Ptr pMsg)
{
  OSS::mutex_lock lock(_mutex);

  if (!_transport)
  {
    OSS_LOG_ERROR("SIPTransaction::writeMessage - Transport is NULL while attempting to send a request.");
    return;
  }

  if (SIPXOR::isEnabled() && _isXOREncrypted)
  {
    pMsg->setProperty("xor", "1");
  }

  std::ostringstream logMsg;
  logMsg << _logId << ">>> " << pMsg->startLine()
  << " LEN: " << pMsg->data().size()
  << " SRC: " << _transport->getLocalAddress().toIpPortString()
  << " DST: " << _transport->getRemoteAddress().toIpPortString()
  << " ENC: " << _isXOREncrypted
  << " PROT: " << _transport->getTransportScheme();
  OSS::log_information(logMsg.str());

  if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
    OSS::log_debug(pMsg->createLoggerData());

  if (_fsm->onSendMessage(pMsg))
  {
    _transport->writeMessage(pMsg);
  }
}

void SIPTransaction::writeMessage(SIPMessage::Ptr pMsg, const OSS::IPAddress& remoteAddress)
{
  OSS::mutex_lock lock(_mutex);

  if (!_transport)
  {
    OSS_LOG_ERROR("SIPTransaction::writeMessage does not ahve a transport to use");
    return;
  }

  if (SIPXOR::isEnabled() && _isXOREncrypted)
  {
    pMsg->setProperty("xor", "1");
  }

  if (_fsm->onSendMessage(pMsg))
  {
      std::ostringstream logMsg;
      logMsg << _logId << ">>> " << pMsg->startLine()
      << " LEN: " << pMsg->data().size()
      << " SRC: " << _transport->getLocalAddress().toIpPortString()
      << " DST: " << remoteAddress.toIpPortString()
      << " ENC: " << _isXOREncrypted
      << " PROT: " << _transport->getTransportScheme();
      OSS::log_information(logMsg.str());
      if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
        OSS::log_debug(pMsg->createLoggerData());

     _transport->writeMessage(pMsg, 
      remoteAddress.toString(),
      OSS::string_from_number<unsigned short>(remoteAddress.getPort()));
  }
}

void SIPTransaction::terminate()
{
   setState(TRN_STATE_TERMINATED);
  _fsm->cancelAllTimers();
  _fsm->onTerminate();
  _owner->removeTransaction(_id);
}

void SIPTransaction::setState(int state)
{
  OSS::mutex_write_lock lock(_stateMutex);

  if (_state == state)
    return;

  if (TRN_STATE_TERMINATED != _state)
    _state = state;
  else
    throw OSS::SIP::SIPException("Invalid Transaction State");
}

int SIPTransaction::getState() const
{
  OSS::mutex_write_lock lock(_stateMutex);
  return _state;
}


void SIPTransaction::handleTimeoutICT()
{
  OSS::mutex_write_lock lock(_stateMutex);
  if (_responseTU && TRN_STATE_TERMINATED != _state)
    _responseTU(SIPTransaction::Error(new OSS::SIP::SIPException("ICT Timeout")), SIPMessage::Ptr(), SIPTransportSession::Ptr(), shared_from_this());
}

void SIPTransaction::handleTimeoutNICT()
{
  OSS::mutex_write_lock lock(_stateMutex);
  if (_responseTU && TRN_STATE_TERMINATED != _state)
    _responseTU(SIPTransaction::Error(new OSS::SIP::SIPException("NICT Timeout")), SIPMessage::Ptr(), SIPTransportSession::Ptr(), shared_from_this());
}

void SIPTransaction::informTU(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  OSS::mutex_write_lock lock(_stateMutex);
  if (_responseTU && TRN_STATE_TERMINATED != _state)
    _responseTU(SIPTransaction::Error(), pMsg, pTransport, shared_from_this());
}

SIPTransaction::Ptr SIPTransaction::createChildTransactionFromResponse(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, bool mayExist)
{
  SIPTransaction::Ptr pChild;
  std::string toHeader = pMsg->hdrGet("to");
  if (toHeader.empty())
    return pChild; /// return a NULL child

  std::string tag;
  SIPTo to(toHeader);
  to.getTag(tag);

  if (tag.empty())
    return pChild;

  if (mayExist)
  {
    pChild = findChildTransaction(tag);
    if (pChild)
      return pChild; /// The child already exists
  }

  pChild = createChildTransactionFromResponse(tag, false);

  if (pChild)
  {
    pChild->_transport = _transport;
  }

  return pChild;
}

SIPTransaction::Ptr SIPTransaction::createChildTransactionFromResponse(const std::string& id, bool mayExist)
{
  SIPTransaction::Ptr pChild;
  if (mayExist)
  {
    pChild = findChildTransaction(id);
    if (pChild)
      return pChild; /// The child already exists
  }

  OSS::mutex_lock lock(_mutex);
  pChild = SIPTransaction::Ptr(new SIPTransaction());
  pChild->_isChildTransaction = true;

  //
  // pChild->_parent is a weak pointer so there is no danger of cyclic pointer reference here
  //
  pChild->_parent = shared_from_this();
  _children[id] = pChild->shared_from_this();

  pChild->_type = _type;
  pChild->setId(id);
  pChild->setLogId(_logId);

  //
  // A newly created ICT child must be set to TRN_STATE_CHILD regardless of the parents state
  // Will be treated as TRYING
  //
  pChild->setState(TRN_STATE_CHILD);


  //
  // Sets the transaction pool pointer
  //
  pChild->_owner = _owner;
  
  //
  // Attach an new FSM to child
  //
  pChild->_owner->onAttachFSM(pChild);

  //
  // Inherit from parent
  //
  pChild->_isXOREncrypted = _isXOREncrypted;
  pChild->_localAddress = _localAddress;
  pChild->_transportService = _transportService;
  pChild->_responseTU = _responseTU;
  pChild->_ackHandler = _ackHandler;

  return pChild;
}

SIPTransaction::Ptr SIPTransaction::findChildTransactionFromResponse(const SIPMessage::Ptr& pMsg)
{
  SIPTransaction::Ptr pChild;

  std::string toHeader = pMsg->hdrGet("to");
  if (toHeader.empty())
    return pChild; /// return a NULL child

  std::string tag;
  SIPTo to(toHeader);
  to.getTag(tag);

  return findChildTransaction(tag);
}

SIPTransaction::Ptr SIPTransaction::findChildTransaction(const std::string& id)
{
  OSS::mutex_lock lock(_mutex);

  if (_children.find(id) == _children.end())
    return SIPTransaction::Ptr();

  return _children.at(id);
}

} } // namespace OSS::SIP



