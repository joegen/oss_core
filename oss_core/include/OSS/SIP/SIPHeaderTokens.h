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


#ifndef SIP_SIPHeaderTokens_INCLUDED
#define SIP_SIPHeaderTokens_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParserException.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <map>
#include <sstream>


namespace OSS {
namespace SIP {


static const char CR[] = "\r";
static const char LF[] = "\n";
static const char CRLF[] = "\r\n";
static const char CRCR[] = "\r\r";
static const char LFLF[] = "\n\n";
static const char LFCR[] = "\n\r";
static const char CRLFCRLF[] = "\r\n\r\n";
//
// Compact Forms
//
static const char HDR_CONTENT_TYPE_COMPACT[]      = "c";
static const char HDR_REFERRED_BY_COMPACT[]       = "b";
static const char HDR_CONTENT_ENCODING_COMPACT[]  = "e";
static const char HDR_FROM_COMPACT[]              = "f";
static const char HDR_CALL_ID_COMPACT[]           = "i";
static const char HDR_SUPPORTED_COMPACT[]         = "k";
static const char HDR_CONTENT_LENGTH_COMPACT[]    = "l";
static const char HDR_CONTACT_COMPACT[]           = "m";
static const char HDR_EVENT_COMPACT[]             = "o";
static const char HDR_REFER_TO_COMPACT[]          = "r";
static const char HDR_SUBJECT_COMPACT[]           = "s";
static const char HDR_TO_COMPACT[]                = "t";
static const char HDR_ALLOW_EVENTS_COMPACT[]      = "u";
static const char HDR_VIA_COMPACT[]               = "v";
//
// RFC 3261 Headers
//
static const char HDR_ACCEPT[]                    = "Accept";
static const char HDR_ACCEPT_LCASE[]              = "accept";
static const char HDR_ACCEPT_ENCODING[]           = "Accept-Encoding";
static const char HDR_ACCEPT_ENCODING_LCASE[]     = "accept-encoding";
static const char HDR_ACCEPT_LANGUAGE[]           = "Accept-Language";
static const char HDR_ACCEPT_LANGUAGE_LCASE[]     = "accept-language";
static const char HDR_ALERT_INFO[]                = "Alert-Info";
static const char HDR_ALERT_INFO_LCASE[]          = "alert-info";
static const char HDR_ALLOW[]                     = "Allow";
static const char HDR_ALLOW_LCASE[]               = "allow";
static const char HDR_AUTHENTICATION_INFO[]       = "Authentication-Info";
static const char HDR_AUTHENTICATION_INFO_LCASE[] = "authentication-info";
static const char HDR_AUTHORIZATION[]             = "Authorization";
static const char HDR_AUTHORIZATION_LCASE[]       = "authorization";
static const char HDR_CALL_ID[]                   = "Call-ID";
static const char HDR_CALL_ID_LCASE[]             = "call-id";
static const char HDR_CALL_INFO[]                 = "Call-Info";
static const char HDR_CALL_INFO_LCASE[]           = "call-info";
static const char HDR_CONTACT[]                   = "Contact";
static const char HDR_CONTACT_LCASE[]             = "contact";
static const char HDR_CONTENT_DISPOSITION[]       = "Content-Disposition";
static const char HDR_CONTENT_DISPOSITION_LCASE[] = "content-disposition";
static const char HDR_CONTENT_ENCODING[]          = "Content-Encoding";
static const char HDR_CONTENT_ENCODING_LCASE[]    = "content-encoding";
static const char HDR_CONTENT_LANGUAGE[]          = "Content-Language";
static const char HDR_CONTENT_LANGUAGE_LCASE[]    = "content-language";
static const char HDR_CONTENT_LENGTH[]            = "Content-Length";
static const char HDR_CONTENT_LENGTH_LCASE[]      = "content-length";
static const char HDR_CONTENT_TYPE[]              = "Content-Type";
static const char HDR_CONTENT_TYPE_LCASE[]        = "content-type";
static const char HDR_CSEQ[]                      = "CSeq";
static const char HDR_CSEQ_LCASE[]                = "cseq";
static const char HDR_DATE[]                      = "Date";
static const char HDR_DATE_LCASE[]                = "date";
static const char HDR_ERROR_INFO[]                = "Error-Info";
static const char HDR_ERROR_INFO_LCASE[]          = "error-info";
static const char HDR_EXPIRES[]                   = "Expires";
static const char HDR_EXPIRES_LCASE[]             = "expires";
static const char HDR_FROM[]                      = "From";
static const char HDR_FROM_LCASE[]                = "from";
static const char HDR_IN_REPLY_TO[]               = "In-Reply-To";
static const char HDR_IN_REPLY_TO_LCASE[]         = "in-reply-to";
static const char HDR_MAX_FORWARDS[]              = "Max-Forwards";
static const char HDR_MAX_FORWARDS_LCASE[]        = "max-forwards";
static const char HDR_MIN_EXPIRES[]               = "Min-Expires";
static const char HDR_MIN_EXPIRES_LCASE[]         = "min-expires";
static const char HDR_MIME_VERSION[]              = "MIME-Version";
static const char HDR_MIME_VERSION_LCASE[]        = "mime-version";
static const char HDR_ORGANIZATION[]              = "Organization";
static const char HDR_ORGANIZATION_LCASE[]        = "organization";
static const char HDR_PRIORITY[]                  = "Priority";
static const char HDR_PRIORITY_LCASE[]            = "priority";
static const char HDR_PROXY_AUTHENTICATE[]        = "Proxy-Authenticate";
static const char HDR_PROXY_AUTHENTICATE_LCASE[]  = "proxy-authenticate";
static const char HDR_PROXY_AUTHORIZATION[]       = "Proxy-Authorization";
static const char HDR_PROXY_AUTHORIZATION_LCASE[] = "proxy-authorization";
static const char HDR_PROXY_REQUIRE[]             = "Proxy-Require";
static const char HDR_PROXY_REQUIRE_LCASE[]       = "proxy-require";
static const char HDR_RECORD_ROUTE[]              = "Record-Route";
static const char HDR_RECORD_ROUTE_LCASE[]        = "record-route";
static const char HDR_REPLY_TO[]                  = "Reply-To";
static const char HDR_REPLY_TO_LCASE[]            = "reply-to";
static const char HDR_REQUIRE[]                   = "Require";
static const char HDR_REQUIRE_LCASE[]             = "require";
static const char HDR_RETRY_AFTER[]               = "Retry-After";
static const char HDR_RETRY_AFTER_LCASE[]         = "retry-after";
static const char HDR_ROUTE[]                     = "Route";
static const char HDR_ROUTE_LCASE[]               = "route";
static const char HDR_SERVER[]                    = "Server";
static const char HDR_SERVER_LCASE[]              = "server";
static const char HDR_SUBJECT[]                   = "Subject";
static const char HDR_SUBJECT_LCASE[]             = "subject";
static const char HDR_SUPPORTED[]                 = "Supported";
static const char HDR_SUPPORTED_LCASE[]           = "supported";
static const char HDR_TIMESTAMP[]                 = "Timestamp";
static const char HDR_TIMESTAMP_LCASE[]           = "timestamp";
static const char HDR_TO[]                        = "To";
static const char HDR_TO_LCASE[]                  = "to";
static const char HDR_UNSUPPORTED[]               = "Unsupported";
static const char HDR_UNSUPPORTED_LCASE[]         = "unsupported";
static const char HDR_USER_AGENT[]                = "User-Agent";
static const char HDR_USER_AGENT_LCASE[]          = "user-agent";
static const char HDR_VIA[]                       = "Via";
static const char HDR_VIA_LCASE[]                 = "via";
static const char HDR_WARNING[]                   = "Warning";
static const char HDR_WARNING_LCASE[]             = "warning";
static const char HDR_WWW_AUTHENTICATE[]          = "WWW-Authenticate";
static const char HDR_WWW_AUTHENTICATE_LCASE[]    = "www-authenticate";
//
// RFC 3262 Headers
//
static const char HDR_RSEQ[]                      = "RSeq";
static const char HDR_RSEQ_LCASE[]                = "rseq";
static const char HDR_RACK[]                      = "RAck";
static const char HDR_RACK_LCASE[]                = "rack";
//
// RFC 3265 Headers
//
static const char HDR_EVENT[]                     = "Event";
static const char HDR_EVENT_LCASE[]               = "event";
static const char HDR_ALLOW_EVENTS[]              = "Allow-Events";
static const char HDR_ALLOW_EVENTS_LCASE[]        = "allow-events";
static const char HDR_SUBSCRIPTION_STATE[]        = "Subscription-State";
static const char HDR_SUBSCRIPTION_STATE_LCASE[]  = "subscription-state";
//
// RFC 3515/3892 Headers
//
static const char HDR_REFER_TO[]                  = "Refer-To";
static const char HDR_REFER_TO_LCASE[]            = "refer-to";
static const char HDR_REFERRED_BY[]               = "Referred-By";
static const char HDR_REFERRED_BY_LCASE[]         = "referred-by";
//
// RFC 3325 Headers
//
static const char HDR_P_ASSERTED_IDENTITY[]       = "P-Asserted-Identity";
static const char HDR_P_ASSERTED_IDENTITY_LCASE[] = "p-asserted-identity";
static const char HDR_P_PREFERRED_IDENTITY[]      = "P-Preferred-Identity";
static const char HDR_P_PREFERRED_IDENTITY_LCASE[]= "p-preferred-identity";
static const char HDR_PRIVACY[]                   = "Privacy";
static const char HDR_PRIVACY_LCASE[]             = "privacy";
//
// RFC 4028 Headers
//
static const char HDR_MIN_SE[]                    = "Min-SE";
static const char HDR_MIN_SE_LCASE[]              = "min-se";
static const char HDR_SESSION_EXPIRES[]           = "Session-Expires";
static const char HDR_SESSION_EXPIRES_LCASE[]     = "session-expires";
typedef std::vector<std::string> sip_header_tokens;
class OSS_API SIPHeaderTokens : public sip_header_tokens
{
public:
  SIPHeaderTokens();
    /// Create a new SIPHeaderTokens vector
  
  SIPHeaderTokens(const SIPHeaderTokens& tokens);
    /// Create a new SIPHeaderTokens vector from another SIPHeaderTokens vector object

  SIPHeaderTokens& operator=(const SIPHeaderTokens& tokens);
    /// Copy the content from another SIPHeaderTokens vector

  void swap(SIPHeaderTokens& tokens);
    /// Exchanges the data between two SIPHeaderTokens vectors
  
  std::string& rawHeaderName();
    /// Returns the raw header name;

  size_t& headerOffSet();
    /// Returns the header offset;

  void push_front(const std::string& header);
    /// This is a simple hack that mimics std::list::push_front method.
    ///
    /// Since it is essential for a SIP header to be accessed using
    /// their index, we cannot use std::list and resort to using iterators.


private:
  std::string _rawHeaderName;
  size_t _headerOffSet;
};

typedef std::map<std::string, SIPHeaderTokens> SIPHeaderList;


} } // OSS::SIP
#endif // SIP_SIPHeaderTokens_INCLUDED


