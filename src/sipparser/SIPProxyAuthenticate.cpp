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


#include "OSS/SIP/SIPProxyAuthenticate.h"
#include "OSS/ABNF/ABNFSIPRules.h"

using namespace OSS::ABNF;
static ABNF_SIP_pvalue pvalueParser;
static ABNF_SIP_pname pnameParser;
static ABNFEvaluate<ABNF_SIP_pname> pnameVerify;
static ABNFEvaluate<ABNF_SIP_pvalue> pvalueVerify;


/*
 Authorization	=  	"Authorization" HCOLON credentials
		; example:
    ;    Authorization: Digest username="bob", realm="atlanta.example.com"
    ;      nonce="ea9c8e88df84f1cec4341ae6cbe5a359", opaque="",
    ;      uri="sips:ss2.biloxi.example.com",
    ;      response="dfe56131d1958046689d83306477ecc"
credentials	= 	("Digest" LWS digest-response) / other-response
digest-response	= 	dig-resp *(COMMA dig-resp)
dig-resp	= 	username / realm / nonce / digest-uri / dresponse / algorithm / cnonce
/ opaque / message-qop / nonce-count / auth-param / auts
username	= 	"username" EQUAL username-value
username-value	= 	quoted-string
digest-uri	= 	"uri" EQUAL LDQUOT digest-uri-value RDQUOT
digest-uri-value	= 	request-uri
		; equal to request-uri as specified by HTTP/1.1
message-qop	= 	"qop" EQUAL qop-value
cnonce	= 	"cnonce" EQUAL cnonce-value
cnonce-value	= 	nonce-value
nonce-count	= 	"nc" EQUAL nc-value
nc-value	= 	8LHEX
dresponse	= 	"response" EQUAL request-digest
request-digest	= 	LDQUOT 32LHEX RDQUOT

auth-param	= 	auth-param-name EQUAL ( token / quoted-string )
auth-param-name	= 	token

other-response	= 	auth-scheme LWS auth-param *(COMMA auth-param)
auth-scheme	= 	token
auts	= 	"auts" EQUAL auts-param
auts-param	= 	LDQUOT auts-value RDQUOT
auts-value	= 	<base64 encoding of AUTS>
 */


namespace OSS {
namespace SIP {


SIPProxyAuthenticate::SIPProxyAuthenticate()
{
  _data = "Digest algorithm=MD5";
}

SIPProxyAuthenticate::SIPProxyAuthenticate(const std::string& authorization)
{
  _data = authorization;
}

SIPProxyAuthenticate::SIPProxyAuthenticate(const SIPProxyAuthenticate& authorization)
{
  _data = authorization._data;
}

SIPProxyAuthenticate::~SIPProxyAuthenticate()
{

}

SIPProxyAuthenticate& SIPProxyAuthenticate::operator = (const std::string& authorization)
{
  SIPProxyAuthenticate clonable(authorization);
  swap(clonable);
  return *this;
}

SIPProxyAuthenticate& SIPProxyAuthenticate::operator = (const SIPProxyAuthenticate& authorization)
{
  SIPProxyAuthenticate clonable(authorization);
  swap(clonable);
  return *this;
}

void SIPProxyAuthenticate::swap(SIPProxyAuthenticate& authorization)
{
  std::swap(_data, authorization._data);
}

std::string SIPProxyAuthenticate::getAuthParam(const char* paramName) const
{
  std::string paramValue;
  getAuthParam(_data, paramName, paramValue);
  return paramValue;
}

bool SIPProxyAuthenticate::getAuthParam(const std::string& authorization, const char* paramName, std::string& paramValue)
{
  std::string key = paramName;
  key += "=";

  char* startIter = ABNF::findNextIterFromString(key, authorization.c_str());
  if (startIter == authorization.c_str())
    return false;

  char* newIter = pvalueParser.parse(startIter);
  if (newIter == startIter)
    return false;

  paramValue = std::string(startIter, newIter);
  return true;
}

bool SIPProxyAuthenticate::setAuthParam(const char* paramName, const char* paramValue)
{
  return setAuthParam(_data, paramName, paramValue);
}

bool SIPProxyAuthenticate::setAuthParam(std::string& authorization, const char* paramName, const char* paramValue)
{
  if (!pnameVerify(paramName) || !pvalueVerify(paramValue))
    throw OSS::SIP::SIPParserException("ABNF Syntax Error");

  std::string key = paramName;
  boost::to_lower(key);
  key += "=";

  char* offSet = ABNF::findNextIterFromString(key, authorization.c_str());
  if (offSet == authorization.c_str())
  {
    std::ostringstream strm;
    strm << ", " << paramName << "=" << paramValue;
    authorization += strm.str();
    return true;
  }

  std::string front(authorization.c_str(), (const char*)offSet);
  if(front.at(front.size() - 1) != '=')
    front += "=";
  front += paramValue;

  char* tailOffSet = ABNF::findNextIterFromString(",", offSet);
  if (tailOffSet != offSet)
  {
    tailOffSet--;
    front += tailOffSet;
  }

  authorization = front;
  return true;
}



} } // OSS::SIP





