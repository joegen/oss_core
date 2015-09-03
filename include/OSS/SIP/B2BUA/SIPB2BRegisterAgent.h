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

#ifndef OSS_SIPB2BREGISTERAGENT_H_INCLUDED
#define	OSS_SIPB2BREGISTERAGENT_H_INCLUDED


#include "OSS/UTL/Thread.h"
#include "OSS/SIP/UA/SIPRegistration.h"
#include "OSS/SIP/UA/SIPUserAgent.h"


namespace OSS {
namespace SIP {
namespace B2BUA {

  
class SIPB2BRegisterAgent
{
public:
  
  typedef std::map<std::string, OSS::SIP::UA::SIPRegistration::Ptr> Sessions;
  
  SIPB2BRegisterAgent();
  
  ~SIPB2BRegisterAgent();
  
  bool initialize(const std::string& agentName);
    /// Initialize the local registration agent
  
  bool sendRegister(
    const std::string& domain, 
    const std::string user, 
    const std::string& authUser, 
    const std::string& authPass,
    const std::string& contactParams,
    const std::string& extraHeaders,
    const std::string& nextHop,
    OSS::UInt32 expires = 3600);
    /// Register an account with a remote registrar
  
  bool sendRegister(
    const std::string& domain, 
    const std::string user, 
    const std::string& authUser, 
    const std::string& authPass,
    const std::string& contactParams,
    const std::string& extraHeaders,
    const std::string& nextHop,
    OSS::UInt32 expires,
    const OSS::SIP::UA::SIPRegistration::ResponseHandler& responseHandler
    );
  /// Register an account with a remote registrar with a response handler set
  
  
  void onRegisterResponse(
    OSS::SIP::UA::SIPRegistration* pReg, 
    const SIPMessage::Ptr& pMsg, 
    const std::string& error);
    /// Callback handler for the client registration
  
  bool run(const OSS::SIP::UA::SIPUserAgent::ExitHandler& exitHandler);
    /// Start processing requests
  
  void stop();
    /// Deregister all active regs and stop the agent
   
  bool isLocalRegistration(const std::string& uri) const;
    /// Returns true if the uri is equal to the contact of an existing registration 
  
protected:
  OSS::SIP::UA::SIPRegistration* prepareRegister(
    const std::string& domain, 
    const std::string user, 
    const std::string& authUser, 
    const std::string& authPass,
    const std::string& contactParams,
    const std::string& extraHeaders,
    const std::string& nextHop,
    OSS::UInt32 expires);
private:
  OSS::SIP::UA::SIPUserAgent* _pRegisterUa;
  Sessions _sessions;
  mutable OSS::mutex_critic_sec _sessionsMutex;
};

//
// Inlines
//

} } } //OSS::SIP::B2BUA

#endif	// OSS_SIPB2BREGISTERAGENT_H_INCLUDED

