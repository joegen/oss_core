
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


#ifndef SBCAUTHENTICATOR_H
#define	SBCAUTHENTICATOR_H


#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SBC/SBCAccounts.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::SIP::B2BUA::SIPB2BTransaction;  
  
class SBCAuthenticator
{
public:
  SBCAuthenticator();
  ~SBCAuthenticator();
  
  SIPMessage::Ptr onAuthenticateTransaction(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
  
  bool isAuthorized(const SIPMessage::Ptr& pRequest, const SBCAccountRecord& record);

  SBCAccounts& accounts();
private:
  SBCAccounts _accounts;
   
};


//
// Inlines
//


inline SBCAccounts& SBCAuthenticator::accounts()
{
  return _accounts;
}

} } } // OSS::SIP::SBC

#endif	/* SBCAUTHENTICATOR_H */

