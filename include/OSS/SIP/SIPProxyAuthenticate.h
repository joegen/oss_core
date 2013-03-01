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


#ifndef SIP_SIPProxyAuthenticate_INCLUDED
#define SIP_SIPProxyAuthenticate_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {


class OSS_API SIPProxyAuthenticate: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPProxyAuthenticate();
    /// Create a new SIPProxyAuthenticate header

  SIPProxyAuthenticate(const std::string& authorization);
    /// Create a SIPProxyAuthenticate from a string a string.
    ///
    /// This constructor expects that the authorization header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the authorization variable parameter
    /// is the body of the authorization header (not including Proxy-Authenticate:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("proxy-authenticate");
    ///   SIPProxyAuthenticate authorization(hdr);

  SIPProxyAuthenticate(const SIPProxyAuthenticate& authorization);
    /// Create a new SIPProxyAuthenticate from another SIPProxyAuthenticate object

  ~SIPProxyAuthenticate();
    /// Destroy the SIPProxyAuthenticate object

  SIPProxyAuthenticate& operator = (const std::string& authorization);
    /// Copy the content authorization another a authorization string.

  SIPProxyAuthenticate& operator = (const SIPProxyAuthenticate& authorization);
    /// Copy the content authorization another SIPProxyAuthenticate

  void swap(SIPProxyAuthenticate& authorization);
    /// Exchanges the data between two SIPProxyAuthenticate

  std::string getAuthParam(const char* paramName) const;
    /// Return the value of the header parameter if present

  static bool getAuthParam(const std::string& authorization, const char* paramName, std::string& paramValue);
    /// Return the value of the header parameter if present

  bool setAuthParam(const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  static bool setAuthParam(std::string& authorization, const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

    /// realm	=  	"realm" EQUAL  realm-value
    /// realm-value	= 	quoted-string
    /// domain	= 	"domain" EQUAL LDQUOT URI *( 1*SP URI ) RDQUOT
    /// URI	= 	absoluteURI / abs-path
    /// opaque	= 	"opaque" EQUAL quoted-string
    /// stale	= 	"stale" EQUAL ( "true" / "false" )
    /// qop-options	= 	"qop" EQUAL LDQUOT qop-value *("," qop-value) RDQUOT
    /// qop-value	= 	"auth" / "auth-int" / token
};

//
// Inlines
//


} } // OSS::SIP
#endif // SIP_SIPProxyAuthenticate_INCLUDED


