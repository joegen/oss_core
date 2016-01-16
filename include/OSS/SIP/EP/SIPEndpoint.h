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

#ifndef OSS_SIPENDPOINT_H_INCLUDED
#define	OSS_SIPENDPOINT_H_INCLUDED


#include "OSS/Net/IPAddress.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"
#include "OSS/SIP/B2BUA/SIPB2BUserAgentHandler.h"


namespace OSS {
namespace SIP {
namespace EP {
  
  
class SIPEndpoint : public OSS::SIP::B2BUA::SIPB2BUserAgentHandler
{
public:
  enum TransportType
  {
    TYPE_UDP,
    TYPE_TCP,
    TYPE_TLS,
    TYPE_WS,
    TYPE_WSS,
    NUM_TYPE
  };
  
  SIPEndpoint();
  
  ~SIPEndpoint();
        
  bool addTransport(TransportType transportType, unsigned short port);
  bool addTransport(TransportType transportType, const std::string& address, unsigned short port);
  bool addTransport(TransportType transportType, const OSS::Net::IPAddress& address);
  // Add transport to the endpoint.  This must be done
  // prior to calling run();)
  //  
  
  bool run();
  // Initialize the endpoint and start processing incoming SIP messages.
  // This function will return right away.  If it returns false,
  // something went wrong with initialization.  Consult logs.
  //
  
  void stop();
  // Stop processing incoming messages and deinitialize the endpoint.
  
  OSS::SIP::B2BUA::SIPB2BUserAgentHandler::Action handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
  // Callback for incoming messages
  
private:
  OSS::SIP::B2BUA::SIPB2BTransactionManager _transactionManager;
};

//
// Inlines
//

} } } // OSS::SIP::EP

#endif // OSS_SIPENDPOINT_H_INCLUDED