
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


namespace OSS {
namespace EP {
  

EndpointListener::EndpointListener(const std::string& endpointName) :
  SIPListener(0, "", ""),
  _endpointName(endpointName),
  _pEventQueueThread(0)
{
  _isEndpoint = true;
  _pConnection = EndpointConnection::Ptr(new EndpointConnection(this));
  OSS::string_to_lower(_endpointName);
}

EndpointListener::~EndpointListener()
{
}

void EndpointListener::setTransportService(SIPTransportService* pTransportService)
{
  _pTransportService = pTransportService;
  _pIoService = (&(_pTransportService->ioService()));
  _dispatch = _pTransportService->dispatch();
}

void EndpointListener::run()
{
  assert(_pTransportService);
  handleStart();  
  //
  // The connection relies on address and port not being empty
  //
  assert(!_address.empty() && !_port.empty());
  
  _pEventQueueThread = new boost::thread(boost::bind(&EndpointListener::monitorEvents, this));
}

void EndpointListener::monitorEvents()
{
  while(true)
  {
    SIPMessage::Ptr pRequest;
    _eventQueue.dequeue(pRequest);
  }
}

void EndpointListener::postEvent(const SIPMessage::Ptr& pRequest)
{
  _eventQueue.enqueue(pRequest);
}

void EndpointListener::dispatchMessage(const SIPMessage::Ptr& pRequest)
{
  if (_dispatch)
  {
    _dispatch(pRequest, _pConnection);
  }
}
  
 
} }  // OSS::SIP::EP


