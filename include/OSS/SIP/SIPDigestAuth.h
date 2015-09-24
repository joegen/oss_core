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


#ifndef _SIPDIGESTAUTH_H
#define	_SIPDIGESTAUTH_H


#include "OSS/SIP/Parser.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {


class OSS_API SIPDigestAuth
{
public:
  SIPDigestAuth();
    /// Creates a new digest authenticator

  ~SIPDigestAuth();
    /// Destroys the digest authenticator object

  static std::string digestCreateA1Hash(const std::string& userName,
    const std::string& password, const std::string& realm);
    /// Create an A1 MD5 Hash

  static std::string digestCreateA2Hash(const std::string& uri, const char* method);
    /// Create the A2 hash

  static std::string digestCreateNonce(const std::string& key);
    /// Create the NONCE.  This would generate a hash of the external key provided
    /// by the eser and the static internal key within the function

  static std::string digestCreateAuthorization(const std::string& a1,
    const std::string& nonce, const std::string& a2);
    /// Create the Digest authorization

  static std::string digestCreateAuthorizationQop(
    const std::string& a1,
    const std::string& nonce,
    const std::string& nonceCount,
    const std::string& cnonce,
    const std::string& qop,
    const std::string& a2);
    /// Create an authorization with QoP
  
  static void setSecretKey(const std::string& key);
    /// Set the secret key used to generate nonce
};


} } // OSS::SIP

#endif	/* _SIPDIGESTAUTH_H */

