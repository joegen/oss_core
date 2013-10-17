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
  _isXOREncrypted(false),
  _pParent(),
  _isParent(true)
{
}

SIPTransaction::SIPTransaction(SIPTransaction::Ptr pParent) :
  _type(pParent->getType()),
  _owner(0),
  _transportService(0),
  _state(TRN_STATE_IDLE),
  _localAddress(),
  _remoteAddress(),
  _sendAddress(),
  _dialogTarget(),
  _isXOREncrypted(false),
  _pParent(pParent),
  _isParent(false)
{
  _id = pParent->getId();
  _logId = pParent->getLogId();
  _pInitialRequest = pParent->getInitialRequest();
  std::ostringstream logMsg;
  logMsg << _logId << getTypeString() << " " << _id << " CREATED";
  OSS::log_debug(logMsg.str());
}
	
SIPTransaction::~SIPTransaction()
{
  std::ostringstream logMsg;
  logMsg << _logId << getTypeString() << " " << _id << " isParent=" << (isParent() ? "Yes" : "No") << " DESTROYED";
  OSS::log_debug(logMsg.str());
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

std::string SIPTransaction::getTypeString() const
{
  if (TYPE_UNKNOWN == _type)
    return "Unknown";
  else if (TYPE_ICT == _type)
    return "InviteClientTransaction";
  else if (TYPE_IST == _type)
    return "InviteServerTransaction";
  else if (TYPE_NICT == _type)
    return "NonInviteClientTrasaction";
  else if (TYPE_NIST == _type)
    return "NonInviteServerTransaction";
  return "Unknow";
}

void SIPTransaction::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  OSS::mutex_lock lock(_mutex);

  bool isAck = pMsg->isRequest("ACK");

  if (pMsg->isRequest() && !_pInitialRequest && !isAck)
    _pInitialRequest = pMsg;

  if (_logId.empty())
    _logId = pMsg->createContextId(true);

  if (!_transport)
    _transport = pTransport;

  if (!_localAddress.isValid())
    _localAddress = pTransport->getLocalAddress();

  if (!_remoteAddress.isValid())
    _remoteAddress = pTransport->getRemoteAddress();

  if (SIPXOR::isEnabled() && !_isXOREncrypted)
  {
    std::string isXOR;
    _isXOREncrypted = pMsg->getProperty("xor", isXOR) && isXOR == "1";
  }

  if (isParent())
  {
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
  }

  //
  // If this is a request and is not an ACK, then the parent IST fsm must always handle it
  //
  if (isParent() && pMsg->isRequest() && !isAck)
  {
    _fsm->onReceivedMessage(pMsg, pTransport);
  }
  else if (!pMsg->isRequest() || isAck)
  {
    //
    // This is a response or an ACK and the transaction could have branched out
    //
    if (!isParent())
    {
      _fsm->onReceivedMessage(pMsg, pTransport);
    }
    else
    {
      SIPTransaction::Ptr pBranch = findBranch(pMsg);
      if (pBranch)
        pBranch->onReceivedMessage(pMsg, pTransport);
      else
        _fsm->onReceivedMessage(pMsg, pTransport);
    }
  }
}

void SIPTransaction::sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    SIPTransaction::Callback callback)
{
  OSS::mutex_lock lock(_mutex);

  if (!pRequest->isRequest())
  {
    throw OSS::SIP::SIPException("Sending a REQUEST using sendRequest() is illegal!");
  }

  if (!_pInitialRequest)
  {
    _pInitialRequest = pRequest;
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
  const SIPMessage::Ptr& pResponse,
  const OSS::IPAddress& sendAddress)
{
  if (!pResponse->isResponse())
    throw OSS::SIP::SIPException("Sending a REQUEST using sendResponse() is illegal!");

  SIPTransaction::Ptr pParent = getParent();

  if (!_transport && isParent())
    throw OSS::SIP::SIPException("Transport Not Ready!");
  else if (!isParent() && pParent)
    _transport = pParent->_transport;

  if (_sendAddress.getPort() == 0)
    _sendAddress = sendAddress;

  SIPTransaction::Ptr pBranch = findBranch(pResponse);

  if (pBranch)
  {
    pBranch->sendResponse(pResponse, sendAddress);
    return;
  }
  else
  {
    //
    // No branch is found.  This instance will handle the response
    //
    if (_transport && _transport->isReliableTransport())
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
          OSS_LOG_ERROR("SIPTransaction::sendResponse - Unable to re-establish transport to send response.");
        }

      }
    }
    else if (_transport)
    {
      //
      // This is UDP so a keep-alive check won't do us any good
      //
      writeMessage(pResponse, _sendAddress);
    }
    else
    {
      OSS_LOG_ERROR("SIPTransaction::sendResponse - Transport is NULL.");
    }
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
    OSS_LOG_ERROR("SIPTransaction::writeMessage does not have a transport to use");
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

  if (isParent())
  {
    {
      OSS::mutex_critic_sec_lock lock(_branchesMutex);
      for (Branches::iterator iter = _branches.begin(); iter != _branches.end(); iter++)
        iter->second->terminate();
    }
    _owner->removeTransaction(_id);
  }
}

void SIPTransaction::onBranchTerminated(const SIPTransaction::Ptr& pBranch)
{
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

SIPTransaction::Ptr SIPTransaction::getParent()
{
  SIPTransaction::Ptr pParent;
  if (!_isParent)
  {
    try
    {
      pParent = _pParent.lock();
    }
    catch(...)
    {
    }
  }
  return pParent;
}

std::size_t SIPTransaction::getBranchCount() const
{
  OSS::mutex_critic_sec_lock lock(_branchesMutex);
  return _branches.size();
}

SIPTransaction::Ptr SIPTransaction::findBranch(const SIPMessage::Ptr& pRequest)
{
  //
  // Only a parent transaction can have branches
  //
  if(!isParent())
    return SIPTransaction::Ptr();

  OSS::mutex_critic_sec_lock lock(_branchesMutex);
  SIPTransaction::Ptr foundBranch;
  std::string branch = pRequest->getToTag();
  if (branch.empty())
    return SIPTransaction::Ptr();

  Branches::iterator pBranch = _branches.find(branch);
  
  //
  // Branch is non-existent.  Create a new one and attach a new FSM to it
  //
  if (pBranch == _branches.end())
  {
    foundBranch = SIPTransaction::Ptr(new SIPTransaction(shared_from_this()));
    _owner->onAttachFSM(foundBranch);
    foundBranch->fsm()->setRequest(fsm()->getRequest());
    foundBranch->_owner = _owner;
    foundBranch->_responseTU = _responseTU;
    _branches[branch] = foundBranch;
  }
  else
  {
    foundBranch = pBranch->second;
  }
  return foundBranch;
}

bool SIPTransaction::allBranchesCompleted() const
{
  OSS::mutex_critic_sec_lock lock(_branchesMutex);
  for (Branches::const_iterator iter = _branches.begin(); iter != _branches.end(); iter++)
    if (!iter->second->fsm()->isCompleted())
      return false;
  return true;
}

bool SIPTransaction::allBranchesTerminated() const
{
  OSS::mutex_critic_sec_lock lock(_branchesMutex);
  for (Branches::const_iterator iter = _branches.begin(); iter != _branches.end(); iter++)
    if (iter->second->getState() != SIPTransaction::TRN_STATE_TERMINATED)
      return false;
  return true;
}


} } // namespace OSS::SIP



