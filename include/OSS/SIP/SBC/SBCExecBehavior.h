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

#ifndef SIP_SBCExecBEHAV_INCLUDED
#define SIP_SBCExecBEHAV_INCLUDED


#include <map>
#include <boost/function.hpp>

#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"


namespace OSS {
namespace SIP {
namespace SBC {


class OSS_API SBCExecBehavior : public SBCDefaultBehavior
{
public:
  

  SBCExecBehavior(SBCManager* pManager);
    /// Creates a new SBC register behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCExecBehavior();
    /// Destroys the register behavior


  SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
};

//
// Inlines
//

} } } // OSS::SIP::SBC

#endif // SIP_SBCExecBEHAV_INCLUDED

