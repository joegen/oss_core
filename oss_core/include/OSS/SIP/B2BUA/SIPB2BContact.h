// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: B2BUA
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


#ifndef B2BCONTACT_H_INCLUDED
#define	B2BCONTACT_H_INCLUDED


#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/SIPContact.h"


namespace OSS {
namespace SIP {
namespace B2BUA {



class SIPB2BTransactionManager;

class OSS_API SIPB2BContact
{
public:
  struct SessionInfo
  {
    std::string sessionId;
    unsigned callIndex;
    SessionInfo()
    {
      callIndex = 0;
    }
  };

  static bool transform(SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::IPAddress& localInterface,
    const SessionInfo& sessionInfo);

  static bool transformAsParams(SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static bool transformAsUserInfo(SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static bool transformAsRecordRouteParams(SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static std::string constructVia(SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localInterface,
    const std::string& transportScheme,
    const std::string& branchHash);

  static bool getSessionInfo(
    SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    SessionInfo& sessionInfo);

  //
  // Register
  //
  static bool transformRegister(
    SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::IPAddress& localInterface);

  static bool transformRegisterAsParams(
    SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const ContactURI& curi,
    const OSS::IPAddress& localInterface,
    const std::string& expires);

  static bool transformRegisterAsUserInfo(
    SIPB2BTransactionManager* pManager,
    const SIPMessage::Ptr& pRequest,
    const ContactURI& curi,
    const OSS::IPAddress& localInterface,
    const std::string& expires);

  static bool isRegisterRoute(const SIPMessage::Ptr& pRequest);

  static bool _dialogStateInParams;
  static bool _registerStateInParams;
  static bool _dialogStateInRecordRoute;
  static const char* _regPrefix;
};

} } } // OSS::SIP::B2BUA

#endif	// SBCCONTACT_H_INCLUDED

