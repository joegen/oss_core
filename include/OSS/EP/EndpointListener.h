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

#ifndef ENDPOINTLISTENER_H_INCLUDED
#define	ENDPOINTLISTENER_H_INCLUDED

#include "OSS/EP/EndpointConnection.h"
#include "OSS/SIP/SIPListener.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/UTL/BlockingQueue.h"


namespace OSS {
namespace EP {
  
  
using OSS::SIP::SIPListener;  
using OSS::SIP::SIPTransportService;

class EndpointListener : public SIPListener
{
public:
  typedef BlockingQueue<SIPMessage::Ptr> EventQueue;
  typedef boost::shared_ptr<EndpointListener> Ptr;
  
  EndpointListener( const std::string& endpointName);
  
  virtual ~EndpointListener();
  
  virtual void handleStart();
    /// handle a start request.  This should not block. 
  
  virtual void handleStop();
    /// handle a stop request.  This should not block
  
  virtual void run();
    /// Called start the event loop of the ep.  This should not block.
  
  virtual void stop();
    /// Stop the endpoints event loop
  
  
  void postEvent(const SIPMessage::Ptr& pRequest);
    /// Post an event to the queue
  
  const std::string& getEndpointName() const;
    /// Get the name assigned to this endpoint.  This is used by the transport service
  
  void setEndpointName(const std::string& endpointName);
    /// Set the assi9gned endpoint name
    
  
  const EndpointConnection::Ptr & getConnection() const;
    /// The solitary conneciton for this endpoint
  
 
  void dispatchMessage(const SIPMessage::Ptr& pMsg);
    /// Dispatch a message to the FSM
  
  void setTransportService(SIPTransportService* pTransportService);
    /// Set the transport service
  
  void setDispatch(const SIPTransportSession::Dispatch& dispatch);
    /// Set the dispatch callback
  
protected:
  virtual void monitorEvents();
    /// event loop
  
  virtual void onHandleEvent(const SIPMessage::Ptr& pRequest) = 0;
    /// handle an incoming SIP event
  
private:
  void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0);
  
  std::string _listenerAddess;
  unsigned short _listenerPort;
  std::string _endpointName;
  EventQueue _eventQueue;
  boost::thread* _pEventQueueThread;
  SIPTransportSession::Dispatch _dispatch;
  EndpointConnection::Ptr _pConnection;
  bool _isTerminating;
};
  
//
// Inlines
//

inline void EndpointListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE userData)
{
  //
  // Not implemented for endpoints
  //
}

inline const std::string& EndpointListener::getEndpointName() const
{
  return _endpointName;
}
  
inline void EndpointListener::setEndpointName(const std::string& endpointName)
{
  _endpointName = endpointName;
  OSS::string_to_lower(_endpointName);
}

inline const EndpointConnection::Ptr& EndpointListener::getConnection() const
{
  return _pConnection;
}

inline void EndpointListener::setDispatch(const SIPTransportSession::Dispatch& dispatch)
{
  _dispatch = dispatch;
}

} }  // OSS::SIP::EP

#endif	// ENDPOINTLISTENER_H_INCLUDED

