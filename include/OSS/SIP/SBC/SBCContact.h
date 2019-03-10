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


#ifndef SBCCONTACT_H_INCLUDED
#define	SBCCONTACT_H_INCLUDED


#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/SIPContact.h"

namespace OSS {
namespace SIP {
namespace SBC {

using namespace OSS::SIP::B2BUA;

class SBCManager;

class OSS_API SBCContact
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

  static void initialize(const boost::filesystem::path& cfgDirectory);

  static bool isFromRegisteredUser(
    SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    std::string& regId);

  static bool transform(SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& localInterface,
    const SessionInfo& sessionInfo);

  static bool transformAsParams(SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static bool transformAsUserInfo(SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static bool transformAsRecordRouteParams(SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& localInterface,
    const std::string& transportScheme,
    const SessionInfo& sessionInfo);

  static std::string constructVia(SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const OSS::Net::IPAddress& localInterface,
    const std::string& transportScheme,
    const std::string& branchHash);

  static bool getSessionInfo(
    SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    SessionInfo& sessionInfo);

  //
  // Register
  //
  static bool transformRegister(
    SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& localInterface);

  static bool transformRegisterAsParams(
    SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const ContactURI& curi,
    const OSS::Net::IPAddress& localInterface,
    const std::string& expires);

  static bool transformRegisterAsUserInfo(
    SBCManager* pSBC,
    const SIPMessage::Ptr& pRequest,
    const ContactURI& curi,
    const OSS::Net::IPAddress& localInterface,
    const std::string& expires);
  
  static bool getContactUriFromRegisterResponse(const std::string& logId, const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pResponse, ContactURI& curi, std::string& regId);

  static bool isRegisterRoute(const SIPMessage::Ptr& pRequest);

  static bool getRegistrationIdentifier(const ContactURI& curi, std::string& regId);
  static bool getRegistrationIdentifier(const SIPURI& uri, std::string& regId);

  static bool _dialogStateInParams;
  static bool _registerStateInParams;
  static bool _dialogStateInRecordRoute;
};

} } } // OSS::SIP::SBC

#endif	// SBCCONTACT_H_INCLUDED

