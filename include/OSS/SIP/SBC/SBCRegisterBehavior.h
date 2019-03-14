// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef SIP_SBCREGBEHAV_INCLUDED
#define SIP_SBCREGBEHAV_INCLUDED

#include <map>
#include <set>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>

#include "OSS/SIP/SBC/SBC.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/SIP/SBC/SBCRegistrationRecord.h"
#include "OSS/Persistent/RedisClient.h"
#include "OSS/Exec/Process.h"
#include "OSS/SIP/SBC/SBCPlugin.h"
#include "SBCWorkSpaceManager.h"
#include "SBCConsole.h"


namespace OSS {
namespace SIP {
namespace SBC {


class OSS_API SBCRegisterBehavior : public SBCDefaultBehavior
{
public:

  enum GatewayStatus
  {
    GatewayOnline,
    GatewayOffline,
    GatewayUnknownStatus
  };
  
  struct Gateway
  {
    Gateway()
    {
      port = 0;
      outboundProxyPort = 0;
      transport = "udp";
      status = GatewayUnknownStatus;
      frequencyInSeconds = 60;
      nextSendCounter = 10;
    }
    
    std::string name;
    std::string host;
    unsigned short port;
    std::string outboundProxy;
    unsigned short outboundProxyPort;
    std::string localInterfaceAddress;
    unsigned short localInterfacePort;
    std::string targetAdddress;
    unsigned short targetPort;
    std::string domain;
    std::string transport;
    GatewayStatus status;
    int frequencyInSeconds;
    int nextSendCounter;
  };
  
  typedef std::map<std::string, Gateway> GatewayList;
  
  SBCRegisterBehavior(SBCManager* pManager);
    /// Creates a new SBC register behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCRegisterBehavior();
    /// Destroys the register behavior

  virtual void initialize();
    /// Called by the B2BUA when the handler is first registered

  void startOptionsThread();
    /// starts the option thread

  virtual void deinitialize();
    /// Called by the B2BUA before a handler is destroyed

  virtual SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route the new request.
    ///
    /// This method expects that the application will format the request-uri
    /// towards the intended target, insert the correct via and contact where responses,
    /// will be received as well as insert a route-set if
    /// upstream proxies are needed.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.  Eg 404, if no route exists.
    /// If the request is routable, the return value must be a null-Ptr.
    ///
    /// Both Local Interface and Target address that the request would use
    /// must be set by the application layer


  SIPMessage::Ptr onRouteUpperReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// This routes the call comming from the upper registrar

  bool onRouteByAOR(SIPMessage* pRequest, SIPB2BTransaction* pTransaction, bool userComparisonOnly, OSS::Net::IPAddress& localInterface, OSS::Net::IPAddress& target);
    /// This is called from the route scripts to attempt routing via the address
    /// of record of a registered user.  If userComparisonOnly is set to true,
    /// the domain/host portion of the request-uri will be ignored and only compare
    /// the user agains the registration state.   This will only allow to the first
    /// occurence of a registration state with the same AOR.  Thus, this function
    /// may result to a stale route if there are mutiple registrations using the same
    /// AOR.  Future version of this function may allow forking


  virtual SIPMessage::Ptr onProcessRequestBody(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    /// This method allows the application to
    /// process the body of the request
    /// before it gets sent out.
    ///
    /// This method allows the upper layer to modify the body
    /// based on specific application requirements.  For
    /// example, a media proxy may modify SDP address and port
    /// so that RTP passes through the application.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.
    ///
    /// If the body is supported, the return value must be a null-Ptr.

  virtual void onProcessResponseBody(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    /// This method allows the application to
    /// process the body of the response
    /// before it gets sent out.
    ///
    /// This method allows the upper layer to modify the body
    /// based on specific application requirements.  For
    /// example, a media proxy may modify SDP address and port
    /// so that RTP passes through the application.
    ///

  virtual void onProcessOutbound(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound request before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  virtual void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Process the inbound response

  virtual void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound response before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  virtual void onTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  void handleOptionsResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransacion);
    /// This callback hadles responses to the OPTIONS keep-alive

  void deleteUpperRegistration(const std::string& regId);

  void addGateway(const std::string& name, Gateway& gateway);
    /// Add this gatewat to the keep-alive list
  
  GatewayStatus getGatewayStatus(const std::string& name) const;
    /// Return the gateway status
  
  void setGatewayStatus(const std::string& name, GatewayStatus status);
    /// Set the gateway status.  This is normally called in handleOptionsResponse callback
  
  bool cliGetGatewayStatus(const SBCConsole::CommandTokens& data, std::string& result);
    /// CLI request for gateway status
  
private:
  void runOptionsThread();
    /// This method runs the OPTIONS keep-alive loop

  void runOptionsResponseThread();
    /// This method runs the OPTIONS keep-alive response loop

  void sendOptionsKeepAlive(const std::string& regRecord);
    /// Send an options keep-alive to the ua owning the registration record
  
  void sendGatewayKeepAlive(Gateway& gateway);
    /// Send options to the gateway
  
  void sendKeepAliveToGateways();
    /// Send options to all configured gateways
  
  void updateContactsFromStorage(const std::string& logId, const OSS::SIP::SIPMessage::Ptr& pResponse, std::set<std::string>* excludeRegId = 0);
    /// Rewrite the contact from bindings store in db

  boost::thread* _pOptionsThread;
  OSS::semaphore _optionsThreadExit;
  OSS::BlockingQueue<std::string> _optionsResponseQueue;
  boost::thread* _pOptionsResponseThread;
  OSS::semaphore _optionsResponseThreadExit;
  OSS::SIP::SIPTransaction::Callback _keepAliveResponseCb;
  OSS::mutex_read_write _rwKeepAliveListMutex;
  typedef std::map<OSS::Net::IPAddress, OSS::Net::IPAddress> KeepAliveList;
  KeepAliveList _keepAliveList;
  SBCWorkSpaceManager::WorkSpace _workspace;
  OSS::thread_pool _threadPool;
  bool _enableOptionsKeepAlive;
  GatewayList _gateways;
  mutable OSS::mutex_critic_sec _gatewaysMutex;
};

//
// Inlines
//



} } } // OSS::SIP::SBC

#endif // SIP_SBCREGBEHAV_INCLUDED

