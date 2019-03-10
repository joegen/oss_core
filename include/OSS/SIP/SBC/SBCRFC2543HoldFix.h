// OSS Software Solutions Application Programmer Interface
// Package: Karoo
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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

#ifndef SBCRFC2542HOLDFIX_INCLUDED
#define	SBCRFC2542HOLDFIX_INCLUDED

#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCException.h"
#include "OSS/SIP/B2BUA/SIPB2BHandler.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"


namespace OSS {
namespace SIP {
namespace SBC {


using namespace OSS::SIP::B2BUA;

class SBCManager;

class OSS_API SBCRFC2543HoldFix
  // This class is a utility object that fixes RFC 2543 style
  // Hold where the c lines is 0.0.0.0 and convert it to RFC3264
  // style hold where the actual IP address remains the same
  // and the direction media attribute set.
{
public:
  SBCRFC2543HoldFix(SBCManager* pManager);
    /// Create a new RFC2543 hold Fixer

  ~SBCRFC2543HoldFix();
    /// Destroy the RFC2543 Hold Fixer

  void fixRFC2543Hold(
    const SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& previousSDP);
    ///
};

//
// Inlines
//

} } } // OSS::SIP::SBC

#endif	// SBCRFC2542HOLDFIX_INCLUDED

