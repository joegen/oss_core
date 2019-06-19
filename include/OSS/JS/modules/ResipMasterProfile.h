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

#ifndef OSS_RESIPMASTERPROFILE_H_INCLUDED
#define OSS_RESIPMASTERPROFILE_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/dum/MasterProfile.hxx>
#include <rutil/SharedPtr.hxx>

class ResipMasterProfile : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(addSupportedMethod);
  JS_METHOD_DECLARE(addAllowedEvent);
  JS_METHOD_DECLARE(addSupportedMimeType);
  JS_METHOD_DECLARE(validateAcceptEnabled);
  JS_METHOD_DECLARE(validateContentEnabled);
  JS_METHOD_DECLARE(setUserAgent);
  JS_METHOD_DECLARE(setOutboundProxy);
  JS_METHOD_DECLARE(setDefaultFrom);
  JS_METHOD_DECLARE(setDigestCredential);
  JS_METHOD_DECLARE(setClientSubscriptionWaitForNotify);
  const resip::SharedPtr<resip::MasterProfile>& profile() const;
private:
  ResipMasterProfile();
  virtual ~ResipMasterProfile();
  resip::SharedPtr<resip::MasterProfile> _profile;
};


//
// Inlines
//

inline const resip::SharedPtr<resip::MasterProfile>& ResipMasterProfile::profile() const
{
  return _profile;
}

#endif // OSS_RESIPMASTERPROFILE_H_INCLUDED