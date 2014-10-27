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

#include <iostream>

#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/UTL/Logger.h"

namespace OSS {
namespace SIP {


SIPFSMDispatch::SIPFSMDispatch() :
  _transport(boost::bind(&SIPFSMDispatch::onReceivedMessage, this, _1, _2)),
  _ict(this),
  _nict(this),
  _ist(this),
  _nist(this),
  _istBlocker(60),
  _enableIctForking(false)
{
}

SIPFSMDispatch::~SIPFSMDispatch()
{
}

void SIPFSMDispatch::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPFSMDispatch::deinitialize()
{

}

void SIPFSMDispatch::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  if (pTransport->getLastReadCount() < 10)
  {
    //
    // message is too short to be a SIP Message
    // Bailing out
    //
    return;
  }

  try
  {
    pMsg->parse();
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << "Incoming message failed to be parsed - " << e.message()
      << " LEN: " << pTransport->getLastReadCount()
      << " SRC: " << pTransport->getRemoteAddress().toIpPortString();
    OSS::log_warning(logMsg.str());
    return;
  }

  std::string id;
  if (!pMsg->getTransactionId(id))
    return;  // don't throw here
             // we don't have control over what we receive from the transport

  SIPTransaction::Ptr trn;
  SIPTransaction::Type transactionType = SIPTransaction::TYPE_UNKNOWN;
  if (pMsg->isRequest())
  {
    if (OSS::string_caseless_starts_with(pMsg->startLine(), "invite"))
    {
      transactionType = SIPTransaction::TYPE_IST;
      if (_istBlocker.has(id))
      {
        OSS_LOG_WARNING("Blocked request retransmission - " <<  pMsg->startLine());
        return;
      }
      
      trn = _ist.findTransaction(pMsg, pTransport);
    }
    else if (OSS::string_caseless_starts_with(pMsg->startLine(), "ack"))
    {
      //
      // ACK for error responses will get matched to a transaction
      //
      transactionType = SIPTransaction::TYPE_IST;
      trn = _ist.findTransaction(pMsg, pTransport, false);
    }
    else
    {
      transactionType = SIPTransaction::TYPE_NIST;
      trn = _nist.findTransaction(pMsg, pTransport);
    }
  }
  else if (!pMsg->isRequest())
  {
    std::string cseq;
    cseq = pMsg->hdrGet(OSS::SIP::HDR_CSEQ);
    if (OSS::string_caseless_ends_with(cseq, "invite"))
    {
      transactionType = SIPTransaction::TYPE_ICT;
      trn = _ict.findTransaction(pMsg, pTransport, false);
    }
    else
    {
      transactionType = SIPTransaction::TYPE_NICT;
      trn = _nict.findTransaction(pMsg, pTransport, false);
    }
  }
  if (trn)
  {
    std::ostringstream logMsg;
    if (!trn->getLogId().empty())
    {
      logMsg << trn->getLogId() << "Found Transaction " << trn->getId();
      OSS::log_debug(logMsg.str());
    }
    else
    {
      trn->setLogId(pMsg->createContextId(true));
      logMsg << trn->getLogId() << "Transaction " << trn->getId() << " CREATED";
      OSS::log_information(logMsg.str());
    }
  }

  if (!trn)
  {
    //
    // We did not get a transaction, check if this is an ack and find the IST ACK transaction
    //
    if (transactionType == SIPTransaction::TYPE_IST && pMsg->isRequest("ACK"))
    {
        //
        // No IST is existing in the ackable Pool.
        // Report this ACK as orphaned to the UA CORE
        //
        if (_ackFor2xxTransactionHandler)
          _ackFor2xxTransactionHandler(pMsg, pTransport);
    }
    else if (transactionType == SIPTransaction::TYPE_ICT && pMsg->is2xx())
    {
      if (_ackFor2xxTransactionHandler)
        _ackFor2xxTransactionHandler(pMsg, pTransport);
    }
    else
    {
      std::ostringstream logMsg;
      logMsg << pMsg->createContextId(true) << "Unable to match incoming request to a transaction - "
        << pMsg->startLine();
      OSS::log_warning(logMsg.str());
    }
  }
  else
  {
    if (!trn->transportService())
    {
      //
      // This is a newly created transaction
      //
      trn->transportService() = &_transport;
      trn->transport() = pTransport;
    }
    trn->onReceivedMessage(pMsg, pTransport);
  }
}

void SIPFSMDispatch::sendRequest(
  const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress,
  SIPTransaction::Callback& callback,
  SIPTransaction::TerminateCallback& terminateCallback)
{
  if (!pRequest->isRequest())
    throw OSS::SIP::SIPException("Sending a response using sendRequest() method is illegal");

  std::string id;
  if (!pRequest->getTransactionId(id))
    throw OSS::SIP::SIPException("Unable to determine transaction identifier");

  SIPTransaction::Ptr trn;
  SIPTransportSession::Ptr nullTransport;
  if (OSS::string_caseless_starts_with(pRequest->startLine(), "invite"))
  {
    //
    // This is an ICT
    //
    trn = _ict.findTransaction(pRequest, nullTransport);
  }
  else
  {
    //
    // This is an NICT
    //
    trn = _nict.findTransaction(pRequest, nullTransport);
  }

  if (trn)
  {
    if (!trn->transportService())
    {
      //
      // This is a newly created transaction
      //
      trn->transportService() = &_transport;
      trn->localAddress() = localAddress;
      trn->remoteAddress() = remoteAddress;
    }
    trn->sendRequest(pRequest, localAddress, remoteAddress, callback, terminateCallback);
  }
}

void SIPFSMDispatch::stop()
{
  _ict.stop();
  _nict.stop();
  _ist.stop();
  _nist.stop();
}
		
} } // namespace OSS::SIP

