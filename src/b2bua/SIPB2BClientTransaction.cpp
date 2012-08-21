// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
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

#include "OSS/SIP/B2BUA/SIPB2BClientTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/Logger.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


SIPB2BClientTransaction::SIPB2BClientTransaction(SIPB2BTransactionManager* pManager) :
  SIPB2BTransaction(pManager)
{
}

SIPB2BClientTransaction::~SIPB2BClientTransaction()
{
}

void SIPB2BClientTransaction::runTask()
{
  _pInternalPtr = new Ptr(this);
  try
  {
    //
    // This method runs in its own thread and will not block any operation
    // in the subsystem.  It is therefore safe to call blocking functions
    // in this method.
    //

    if (!_pClientRequest)
    {
      //
      // Not calling releaseInternalRef because transacton creation ahs not been signaled yet
      //
      delete _pInternalPtr;
      _pInternalPtr = 0;
      throw OSS::SIP::SIPException("Client Request is missing while calling SIPB2BClientTransaction::runTask()");
    }

    _logId =  _pClientRequest->createContextId(true);

    std::string trnId;
    _pClientRequest->getTransactionId(trnId);

    {
      std::ostringstream logMsg;
      logMsg << _logId << "B2B Client Transaction CREATED - " << trnId;
      OSS::log_information(logMsg.str());
    }

    //
    // Signal transaction creation
    //
    if (!_pManager->onClientTransactionCreated(_pClientRequest, shared_from_this()))
    {
      releaseInternalRef();
      return;
    }

    //
    // Route the outbound request.
    // Send a response (probably a 404) if the request is non-routable
    //
    OSS::IPAddress outboundTarget;
    OSS::IPAddress localInterface;
    SIPMessage::Ptr pRouteResponse;

    if (!_pManager->onRouteClientTransaction(_pClientRequest, shared_from_this(), localInterface, outboundTarget))
    {
      OSS::log_critical(_logId + "onRouteClientTransaction failed");
      releaseInternalRef();
      return;
    }

    if (localInterface.isValid() && !_pManager->stack().transport().isLocalTransport(localInterface))
    {
      OSS::log_critical(_logId + "Invalid Local-Interface returned by onRouteClientTransaction");
      releaseInternalRef();
      return;
    }

    if (!outboundTarget.isValid())
    {
      OSS::log_critical(_logId + "Invalid Outbound-Target returned by onRouteTransaction");
      releaseInternalRef();
      return;
    }



    //
    // Save the address properties
    //
    _pClientRequest->setProperty("target-address", outboundTarget.toIpPortString());
    _pClientRequest->setProperty("local-address", localInterface.toIpPortString());

    //
    // Last chance for the application to process the outbound request
    //
    _pManager->onProcessOutbound(_pClientRequest, shared_from_this());

    //
    // Commit the changes
    //
    _pClientRequest->commitData();

    //
    // Send the request
    //
    OSS::SIP::SIPTransaction::Callback responseCallback
      = boost::bind(&SIPB2BTransaction::handleResponse, this, _1, _2, _3, _4);
    _pManager->stack().sendRequest(
      _pClientRequest,
      localInterface,
      outboundTarget,
      responseCallback);


    //
    // Take note that at this point, this transaction is in limbo
    // since it is not maintained in any list. The responses
    // including transaction errors is the only callback that will
    // assure that this transaction is garbage collected
    //
  }
  catch(OSS::Exception e)
  {
    std::ostringstream errorMsg;
    errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runTask() - "
            << e.message();
    OSS::log_error(errorMsg.str());
    releaseInternalRef();
    return;
  }
}

void SIPB2BClientTransaction::runResponseTask()
{
  try
  {
    OSS::mutex_lock reponseLock(_resposeMutex);
    if (_pTransactionError)
    {
        _pManager->onClientTransactionError(_pTransactionError, SIPMessage::Ptr(), shared_from_this());
        releaseInternalRef();
        return;
    }

    SIPMessage::Ptr response;
    {//localize
      _responseQueueMutex.lock();
      response = _responseQueue.front();
      _responseQueue.pop();
      _responseQueueMutex.unlock();
    }//localize

    if (!response)
      throw OSS::SIP::SIPException("Response is NULL while calling SIPB2BTransaction::runResponseTask()");

    if (response->isErrorResponse())
    {
      _pManager->onClientTransactionError(_pTransactionError, response, shared_from_this());
      releaseInternalRef();
      return;
    }

    _pManager->onProcessClientResponse(response, shared_from_this());

    releaseInternalRef();
    return;
  }
  catch(OSS::Exception e)
  {
    std::ostringstream errorMsg;
    errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runResponseTask() - "
            << e.message();
    OSS::log_error(errorMsg.str());
    releaseInternalRef();
    return;
  }
}

} } } // OSS::SIP::B2BUA




