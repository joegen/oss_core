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


#include <vector>

#include "OSS/SIP/SIPDigestAuth.h"
#include "OSS/SIP/SIPParser.h"

#define MD5_NONCE_KEY "toadfish"

namespace OSS {
namespace SIP {


SIPDigestAuth::SIPDigestAuth()
{
}

SIPDigestAuth::~SIPDigestAuth()
{
}

std::string SIPDigestAuth::digestCreateA1Hash(
  const std::string& userName,
  const std::string& password,
  const std::string& realm)
{
  std::string userName_ = userName;
  std::string realm_ = realm;
  SIPParser::unquoteString(userName_);
  SIPParser::unquoteString(realm_);
  std::ostringstream input;
  input << userName_ << ":" << realm_ << ":" << password;
  return OSS::string_md5_hash(input.str().c_str());
}
    /// Create an A1 MD5 Hash

std::string SIPDigestAuth::digestCreateA2Hash(const std::string& uri, const char* method)
{
  std::ostringstream input;
  input << method << ":" << uri;
  return OSS::string_md5_hash(input.str().c_str());
}

std::string SIPDigestAuth::digestCreateNonce(const std::string& key)
{
  std::ostringstream input;
  input << MD5_NONCE_KEY << ":" << key;
  return OSS::string_md5_hash(input.str().c_str());
}

std::string SIPDigestAuth::digestCreateAuthorization(const std::string& a1,
    const std::string& nonce, const std::string& a2)
{
  std::string nonce_ = nonce;
  SIPParser::unquoteString(nonce_);
  std::ostringstream input;
  input << a1 << ":" << nonce_ << ":" << a2;
  return OSS::string_md5_hash(input.str().c_str());
}

std::string SIPDigestAuth::digestCreateAuthorizationQop(
  const std::string& a1,
  const std::string& nonce,
  const std::string& nonceCount,
  const std::string& cnonce,
  const std::string& qop,
  const std::string& a2)
{
  std::string nonce_ = nonce;
  std::string cnonce_ = cnonce;
  std::string qop_ = qop;
  SIPParser::unquoteString(nonce_);
  SIPParser::unquoteString(cnonce_);
  SIPParser::unquoteString(qop_);

  std::ostringstream input;
  input << a1 << ":"
          << nonce_ << ":"
          << nonceCount << ":"
          << cnonce_ << ":"
          << qop_ << ":"
          << a2;

  return OSS::string_md5_hash(input.str().c_str());
}

} } // OSS::SIP