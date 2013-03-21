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


#ifndef SIP_SIPFrom_INCLUDED
#define SIP_SIPFrom_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPURI.h"


namespace OSS {
namespace SIP {


class OSS_API SIPFrom: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPFrom();
    /// Create a new SIPFrom vector

  SIPFrom(const std::string& from);
    /// Create a from from a string.
    ///
    /// This constructor expects that the from header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the from variable parameter
    /// is the body of the from header (not including From:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("from");
    ///   SIPFrom from(hdr);

  SIPFrom(const SIPFrom& from);
    /// Create a new SIPFrom from another SIPFrom object

  ~SIPFrom();
    /// Destroy the SIPFrom object

  SIPFrom& operator = (const std::string& from);
    /// Copy the content from another a from string.

  SIPFrom& operator = (const SIPFrom& from);
    /// Copy the content from another SIPFrom

  void swap(SIPFrom& from);
    /// Exchanges the data between two SIPFrom

  std::string getDisplayName() const;
    /// Returns the display name value if present.

  static bool getDisplayName(const std::string& from, std::string& displayName);
    /// Returns the display name value if present.

  bool setDisplayName(const char* displayName);
    /// Set the value of the display name.

  static bool setDisplayName(std::string& from, const char* uri);
    /// Set the value of the display name.

  std::string getAor();
    /// Returns the address of record (exluding port and params)

  std::string getURI() const;
    /// Returns the from uri value.

  static bool getURI(const std::string& from, std::string& uri);
    /// Returns the from uri value.

  bool setURI(const char* uri);
    /// Set the value of the from uri.

  static bool setURI(std::string& from, const char* uri);
    /// Set the value of the from uri.

  bool setUser(const char* user);
    /// Set the value of the from uri user token.

  static bool setUser(std::string& from, const char* user);
    /// Set the value of the from uri user token.

  std::string getUser() const;
    /// Returns the from uri value user token.

  static bool getUser(const std::string& from, std::string& user);
    /// Returns the from uri value user token.

  bool setHostPort(const char* hostPort);
    /// Set the value of the from uri hostport token.

  static bool setHostPort(std::string& from, const char* hostPort);
    /// Set the value of the from uri hostport token.

  std::string getHostPort() const;
    /// Returns the from uri value hostport token.

  static bool getHostPort(const std::string& from, std::string& host);
    /// Returns the from uri value hostport token.

  std::string getHost() const;
    /// Returns the from uri value host excluding the port.

  static bool getHost(const std::string& from, std::string& host);
    /// Returns the from uri value hostport excluding the port.

  std::string getHeaderParams() const;
    /// Returns the entire semi-colon delimited header param segment

  static bool getHeaderParams(const std::string& from, std::string& headerParams);
    /// Returns the entire semi-colon delimited header param segment

  bool setHeaderParams(const char* headerParams);
    /// Sets the header param from a semi-colon delimited string

  static bool setHeaderParams(std::string& from, const char* headerParams);
    /// Sets the header param from a semi-colon delimited string

  std::string getHeaderParam(const char* paramName) const;
    /// Return the value of the header parameter if present

  static bool getHeaderParam(const std::string& from, const char* paramName, std::string& paramValue);
    /// Return the value of the header parameter if present

  static bool getHeaderParamEx(const std::string& headerParams, const char* paramName, std::string& paramValue);
    /// Return the value of the header parameter if present

  bool setHeaderParam(const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  static bool setHeaderParam(std::string& from, const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  static bool setHeaderParamEx(std::string& headerParams, const char* paramName, const char* paramValue);
    /// Set the value of the header parameter.

  std::string getTag() const;
    /// Return the tag parameter

  static std::string getTag(const std::string& from);
};

typedef SIPFrom SIPTo;
typedef SIPFrom SIPReferTo;
typedef SIPFrom SIPReferredBy;

//
// Inlines
//

inline std::string SIPFrom::getTag() const
{
  return getHeaderParam("tag");
}

inline std::string SIPFrom::getTag(const std::string& from)
{
  std::string tag;
  getHeaderParam(from, "tag", tag);
  return tag;
}

} } // OSS::SIP
#endif // SIP_SIPFrom_INCLUDED


