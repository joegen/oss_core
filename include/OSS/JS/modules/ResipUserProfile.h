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

#ifndef OSS_RESIPUSERPROFILE_H_INCLUDED
#define OSS_RESIPUSERPROFILE_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/dum/UserProfile.hxx>
#include <rutil/SharedPtr.hxx>

class ResipUserProfile : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(setDefaultFrom);
  JS_METHOD_DECLARE(setDigestCredential);
  const resip::SharedPtr<resip::UserProfile>& profile() const;
private:
  ResipUserProfile();
  virtual ~ResipUserProfile();
  resip::SharedPtr<resip::UserProfile> _profile;
};


//
// Inlines
//

inline const resip::SharedPtr<resip::UserProfile>& ResipUserProfile::profile() const
{
  return _profile;
}

#endif // OSS_RESIPUSERPROFILE_H_INCLUDED