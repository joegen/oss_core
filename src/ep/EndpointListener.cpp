
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


#include "OSS/EP/EndpointListener.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace EP {
  

EndpointListener::EndpointListener(const std::string& endpointName) :
  SIPListener(0, "", ""),
  _endpointName(endpointName),
  _pEventQueueThread(0),
  _isTerminating(false)
{
  _isEndpoint = true;
  _pConnection = EndpointConnection::Ptr(new EndpointConnection(this));
  OSS::string_to_lower(_endpointName);
}

EndpointListener::~EndpointListener()
{
  stop();
}

void EndpointListener::setTransportService(SIPTransportService* pTransportService)
{
  _pTransportService = pTransportService;
  _pIoService = (&(_pTransportService->ioService()));
  _dispatch = _pTransportService->dispatch();
}

void EndpointListener::run()
{
  assert(_dispatch);
  //
  // The connection relies on address and port not being empty
  //
  assert(!_address.empty() && !_port.empty());
  _isTerminating = false;
  _pEventQueueThread = new boost::thread(boost::bind(&EndpointListener::monitorEvents, this));
}

void EndpointListener::stop()
{
  _isTerminating = true;
  postEvent(SIPMessage::Ptr());
  if (_pEventQueueThread)
  {
    _pEventQueueThread->join();
    delete _pEventQueueThread;
    _pEventQueueThread = 0;
  }
}

void EndpointListener::monitorEvents()
{
  OSS_LOG_NOTICE("EndpointListener::monitorEvents( " << _endpointName << " ) - STARTED processing events");
  handleStart();
  while(!_isTerminating)
  {
    SIPMessage::Ptr pRequest;
    _eventQueue.dequeue(pRequest);
    if (pRequest)
    {
      OSS_LOG_DEBUG(pRequest->createContextId(true) << "EndpointListener::monitorEvents( " << _endpointName << " ) - processing event " << pRequest->startLine());
      onHandleEvent(pRequest);
    }
    else
    {
      OSS_LOG_DEBUG("EndpointListener::monitorEvents( " << _endpointName << " ) - dropping NULL event");
    }
  }
  handleStop();
  OSS_LOG_NOTICE("EndpointListener::monitorEvents( " << _endpointName << " ) TERMINATED");
}

void EndpointListener::postEvent(const SIPMessage::Ptr& pRequest)
{
  _eventQueue.enqueue(pRequest);
}

void EndpointListener::dispatchMessage(const SIPMessage::Ptr& pRequest)
{
  if (_dispatch)
  {
    OSS_LOG_DEBUG(pRequest->createContextId(true) << "EndpointListener::dispatchMessage( " << pRequest->startLine() << " )");
    pRequest->setProperty(OSS::PropertyMap::PROP_EndpointName, _endpointName);
    _dispatch(pRequest, _pConnection);
  }
  else
  {
    OSS_LOG_ERROR(pRequest->createContextId(true) << "EndpointListener::dispatchMessage( NULL )");
  }
}

void EndpointListener::handleStart()
{
}
  
void EndpointListener::handleStop()
{
}
  
 
} }  // OSS::SIP::EP


