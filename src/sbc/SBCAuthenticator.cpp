// OSS Software Solutions Application Programmer Interface
// Package: SBC
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


#include "OSS/SIP/SBC/SBCAuthenticator.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/UTL/PropertyMap.h"


namespace OSS {
namespace SIP {
namespace SBC {

static const std::string WEB_RTC_DEMO_DOMAIN = "demo.webrtc.local";
  
  
SBCAuthenticator::SBCAuthenticator()
{
  _accounts.addRealm(WEB_RTC_DEMO_DOMAIN);
}

SBCAuthenticator::~SBCAuthenticator()
{
}
  
SIPMessage::Ptr SBCAuthenticator::onAuthenticateTransaction(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  //
  // Find the account for this identity
  //
  SIPFrom hFrom(pRequest->hdrGet(SIP::HDR_FROM));
  std::string identity = hFrom.getAor(false /*no scheme*/);
  
  std::string domain = hFrom.getHost();
  if (domain == WEB_RTC_DEMO_DOMAIN)
  {
    if (pTransaction)
    {
      pTransaction->setProperty(OSS::PropertyMap::PROP_IsLocallyAuthenticated, "yes");
      pTransaction->setProperty("web-rtc-transient-account", "yes");
    }
    OSS_LOG_INFO(pRequest->createContextId(true) << "SBCAuthenticator::onAuthenticateTransaction - Granting demo authorization");
    return SIPMessage::Ptr();
  }
  
  if (!_accounts.isKnownRealm(domain))
  {
    OSS_LOG_DEBUG(pRequest->createContextId(true) << "SBCAuthenticator::onAuthenticateTransaction - We are not authoritative for domain " << domain);
    return SIPMessage::Ptr();
  }
  else
  {
    OSS_LOG_DEBUG(pRequest->createContextId(true) << "SBCAuthenticator::onAuthenticateTransaction - Domain " << domain << " is local for identity " << identity);
  }
  
  SBCAccountRecord account;
  if (!_accounts.findAccount(identity, account))
  {
    return pRequest->createResponse(OSS::SIP::SIPMessage::CODE_403_Forbidden);
  }
  
  //
  // There is a local account configured.  Check if it is authenticated.
  //
  if (!isAuthorized(pRequest, account))
  {
    SIPMessage::Ptr pChallengeResponse = pRequest->createResponse(pRequest->isRequest("REGISTER") ?
      SIPMessage::CODE_401_Unauthorized :
      SIP::SIPMessage::CODE_407_ProxyAuthenticationRequired);
    
    //
    // Check if the application wants to use its own nonce
    //
    std::string forceNonce;
    pRequest->getProperty("force-nonce", forceNonce);

    //
    // Compute the response
    //
    SIPAuthorization challenge;
    account.computeAuthenticateChallengeHeader(pRequest->hdrGet(OSS::SIP::HDR_CALL_ID), forceNonce, challenge);
       
    if (pRequest->isRequest("REGISTER"))
    {
      pChallengeResponse->hdrSet(OSS::SIP::HDR_WWW_AUTHENTICATE, challenge.data().c_str());
    }
    else
    {
      pChallengeResponse->hdrSet(OSS::SIP::HDR_PROXY_AUTHENTICATE, challenge.data().c_str());
    }
    
    pChallengeResponse->commitData();
    
    return pChallengeResponse;
  }
  
  //
  // This can be null when running from unit test
  //
  if (pTransaction)
  {
    pTransaction->setProperty(OSS::PropertyMap::PROP_IsLocallyAuthenticated, "yes");
  }
  
  pRequest->setProperty(OSS::PropertyMap::PROP_IsLocallyAuthenticated, "yes");
  
  return SIPMessage::Ptr();
}

bool SBCAuthenticator::isAuthorized(const SIPMessage::Ptr& pRequest, const SBCAccountRecord& record)
{
  std::string authorizationHeader;
  
  //
  // Proxy-Authorization: Digest 
  //   username=\"5001/0004f22dc547\", 
  //   realm=\"gobitech.inc\", 
  //   nonce=\"26406c04b3dae533c62787a1d23ede67560ccda9\", 
  //   qop=auth, 
  //   cnonce=\"y5IJxVOQ5XqEyv+\", 
  //   nc=00000001, 
  //   uri=\"sip:5002@gobitech.inc;user=phone\", 
  //   response=\"d9fd5f5fd5d8fd98fafb0bc7eed616aa\", 
  //   algorithm=MD5\r
  //
  std::vector<std::string> authorizations;
  std::string method = pRequest->getMethod();
  
  if (method == "REGISTER")
  {
    for (std::size_t i = 0; i < pRequest->hdrGetSize(OSS::SIP::HDR_AUTHORIZATION); i++)
    {
      authorizations.push_back(pRequest->hdrGet(OSS::SIP::HDR_AUTHORIZATION, i));
    }
  }
  else
  {
    for (std::size_t i = 0; i < pRequest->hdrGetSize(OSS::SIP::HDR_PROXY_AUTHORIZATION); i++)
    {
      authorizations.push_back(pRequest->hdrGet(OSS::SIP::HDR_PROXY_AUTHORIZATION, i));
    }
  }
  
  bool matched = false;
  SIPAuthorization authorizationResponse;
  for (std::vector<std::string>::iterator iter = authorizations.begin(); iter != authorizations.end(); iter++)
  {
    authorizationResponse = *iter;
    if (authorizationResponse.getRealm() == record.getRealm() && authorizationResponse.getUserName() == record.getUser())
    {
      matched = true;
      break;
    }
  }
  
  if (!matched)
  {
    return false;
  }

  //
  // We only support qop="auth" for now
  //
  std::string qop = authorizationResponse.getQop();
  if (!qop.empty() && qop != "auth")
  {
    return false;
  }
  
  //
  // Create the A2 hash
  //
  std::string uri = authorizationResponse.getDigestUri();
  std::string a2Hash = SIPDigestAuth::digestCreateA2Hash(uri, method.c_str());
  
  //
  // Compute the response
  //
  std::string nonce = authorizationResponse.getNonce();
  std::string digestResponse;
  if (qop.empty())
  {
    digestResponse = SIPDigestAuth::digestCreateAuthorization(record.getA1Hash(), nonce, a2Hash);
  }
  else
  {
    std::string nonceCount(authorizationResponse.getNonceCount());
    std::string cnonce(authorizationResponse.getCNonce());
    digestResponse = SIPDigestAuth::digestCreateAuthorizationQop(
      record.getA1Hash(),
      nonce,
      nonceCount,
      cnonce,
      qop,
      a2Hash);
  }
  
  return  digestResponse == authorizationResponse.getDigestResponse();
}


//
// Inlines
//

} } } // OSS::SIP::SBC



