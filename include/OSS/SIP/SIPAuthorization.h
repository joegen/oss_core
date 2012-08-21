// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#ifndef SIP_SIPAuthorization_INCLUDED
#define SIP_SIPAuthorization_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {


class OSS_API SIPAuthorization: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPAuthorization();
    /// Create a new SIPAuthorization vector

  SIPAuthorization(const std::string& authorization);
    /// Create a authorization from a string.
    ///
    /// This constructor expects that the authorization header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the authorization variable parameter
    /// is the body of the authorization header (not including Authorization:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("authorization");
    ///   SIPAuthorization authorization(hdr);

  SIPAuthorization(const SIPAuthorization& authorization);
    /// Create a new SIPAuthorization from another SIPAuthorization object

  ~SIPAuthorization();
    /// Destroy the SIPAuthorization object

  SIPAuthorization& operator = (const std::string& authorization);
    /// Copy the content from another a authorization string.

  SIPAuthorization& operator = (const SIPAuthorization& authorization);
    /// Copy the content from another SIPAuthorization

  void swap(SIPAuthorization& authorization);
    /// Exchanges the data between two SIPAuthorization

  std::string getAuthParam(const char* paramName) const;
    /// Return the value of the header parameter if present

  std::string getQuotedAuthParam(const char* paramName) const;
    /// Return the "unquoted" value of the quoted header parameter if present

  static bool getAuthParam(const std::string& authorization, const char* paramName, std::string& paramValue);
    /// Return the value of the header parameter if present

  bool setAuthParam(const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  bool setQuotedAuthParam(const char* paramName, const char* paramValue);
    /// Set the quoted value of the header parameter.

  static bool setAuthParam(std::string& authorization, const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  void setUserName(const char* userName);
  std::string getUserName() const;
    ///
    /// username	= 	"username" EQUAL username-value
    /// username-value	= 	quoted-string
    ///

  void setDigestUri(const char* digestUri);
  std::string getDigestUri() const;
    ///
    /// digest-uri	= 	"uri" EQUAL LDQUOT digest-uri-value RDQUOT
    /// digest-uri-value	= 	request-uri
    ///		; equal to request-uri as specified by HTTP/1.1
    ///

  void setMessageQop(const char* qop);
  std::string getMessageQop() const;
    ///
    /// message-qop	= 	"qop" EQUAL qop-value
    ///

  void setNonce(const char* nonce);
  std::string getNonce() const;
  void setCNonce(const char* cnonce);
  std::string getCNonce() const;
    ///
    /// cnonce	= 	"cnonce" EQUAL cnonce-value
    /// cnonce-value	= 	nonce-value
    ///

  void setNonceCount(const char* nc);
  std::string getNonceCount() const;
    ///
    /// nonce-count	= 	"nc" EQUAL nc-value
    /// nc-value	= 	8LHEX
    ///

  void setDigestResponse(const char* response);
  std::string getDigestResponse() const;
    ///
    /// dresponse	= 	"response" EQUAL request-digest
    /// request-digest	= 	LDQUOT 32LHEX RDQUOT
    ///

    /// opaque	=  	"opaque" EQUAL  quoted-string

  void setAuts(const char* auts);
  std::string getAuts() const;
    ///
    /// auts	= 	"auts" EQUAL auts-param
    /// auts-param	= 	LDQUOT auts-value RDQUOT
    /// auts-value	= 	<base64 encoding of AUTS>
    ///

  void setOpaque(const char* opaque);
  std::string getOpaque() const;
    /// opaque	=  	"opaque" EQUAL  quoted-string

};

//
// Inlines
//

inline void SIPAuthorization::setUserName(const char* userName)
{
  setQuotedAuthParam("username", userName);
}

inline std::string SIPAuthorization::getUserName() const
{
  return getQuotedAuthParam("username");
}

inline void SIPAuthorization::setDigestUri(const char* digestUri)
{
  setQuotedAuthParam("uri", digestUri);
}

inline std::string SIPAuthorization::getDigestUri() const
{
  return getQuotedAuthParam("uri");
}

inline void SIPAuthorization::setMessageQop(const char* qop)
{
  setQuotedAuthParam("qop", qop);
}

inline std::string SIPAuthorization::getMessageQop() const
{
  return getQuotedAuthParam("qop");
}

inline void SIPAuthorization::setNonce(const char* nonce)
{
  setQuotedAuthParam("nonce", nonce);
}

inline std::string SIPAuthorization::getNonce() const
{
  return getQuotedAuthParam("nonce");
}

inline void SIPAuthorization::setCNonce(const char* cnonce)
{
  setQuotedAuthParam("cnonce", cnonce);
}

inline std::string SIPAuthorization::getCNonce() const
{
  return getQuotedAuthParam("cnonce");
}

inline void SIPAuthorization::setNonceCount(const char* nc)
{
  setAuthParam("nc", nc);
}

inline std::string SIPAuthorization::getNonceCount() const
{
  return getAuthParam("nc");
}

inline void SIPAuthorization::setDigestResponse(const char* response)
{
  setQuotedAuthParam("response", response);
}

inline std::string SIPAuthorization::getDigestResponse() const
{
  return getQuotedAuthParam("response");
}

inline void SIPAuthorization::setAuts(const char* auts)
{
  setQuotedAuthParam("auts", auts);
}

inline std::string SIPAuthorization::getAuts() const
{
  return getQuotedAuthParam("auts");
}

inline void SIPAuthorization::setOpaque(const char* opaque)
{
  setQuotedAuthParam("opaque", opaque);
}

inline std::string SIPAuthorization::getOpaque() const
{
  return getQuotedAuthParam("opaque");
}

} } // OSS::SIP
#endif // SIP_SIPAuthorization_INCLUDED


