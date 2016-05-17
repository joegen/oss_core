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


#ifndef SIP_SIPURI_INCLUDED
#define SIP_SIPURI_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/Net/IPAddress.h"


namespace OSS{
namespace SIP{


class OSS_API SIPURI : public SIPParser
{
public:
  typedef std::map<std::string, std::string> header_tokens;

  SIPURI();
    /// Constructs a blank SIPURI

  SIPURI(const std::string& uri);
    /// Constructs a SIPURI from a string. 

  SIPURI(const SIPURI& uri);
    /// Constructs a SIPURI from another object

  SIPURI& operator=(const SIPURI& uri);
    /// Copies a URI object

  SIPURI& operator=(const std::string& uri);
    /// Copies a URI object from a string

  void swap(SIPURI& uri);
    /// Exchange data between two uris
  
  std::string getScheme() const;
    /// Returns the value of the uri scheme string

  bool setScheme(const char* scheme);
    /// Sets the value of the uri scheme

  static bool setScheme(std::string& uri, const char* scheme);
    /// Sets the value of the uri scheme

  static bool getScheme(const std::string& uri, std::string& value);
    /// Returns the value of the uri scheme string
  
  std::string getUser() const;
    /// Returns the value of the user string

  static bool getUser(const std::string& uri, std::string& value);
    /// Returns the value of the user string

  bool setUserInfo(const char* userInfo);
    /// Sets the value of the uri userinfo.

  static bool setUserInfo(std::string& uri, const char* userInfo);
    /// Sets the value of the uri userinfo.

  std::string getPassword() const;
    /// Returns the value of the password string

  static bool getPassword(const std::string& uri, std::string& value);
    /// Returns the value of the password string

  std::string getHostPort() const;
    /// Returns the value of the uri-host string

  bool getHostPort(std::string& host, unsigned short& port) const;
    /// Get the tokenized host port

  static bool getHostPort(const std::string& uri, std::string& value);
    /// Returns the value of the uri-host string

  std::string getHost() const;
    /// Returns the value of the uri-host string exluding the port

  static bool getHost(const std::string& uri, std::string& host);
    /// Returns the value of the uri-host string excluding the port.

  std::string getPort() const;
    /// Returns the port

  static bool getPort(const std::string& uri, std::string& port);
    /// Returns the port

  bool setHost(const char* hostPort);
    /// Set the host but preserve the port if set previously
  
  bool setPort(const char* port);
    /// Set the port
  
  bool setHostPort(const char* hostPort);
    /// Sets the value of the uri hostport.
    ///
    /// Take note that this function may throw SIPABNFSyntaxException
    /// if there are invalid characters or the host is not properly formatted
    
  bool setHostPort(const OSS::Net::IPAddress& hostPort);
    /// Sets the value of the uri hostport.
    ///
    /// Take note that this function may throw SIPABNFSyntaxException
    /// if there are invalid characters or the host is not properly formatted

  static bool setHostPort(std::string& uri, const char* hostPort);
    /// Sets the value of the uri hostport.

  std::string getParams() const;
    /// Returns the entire parameter segment of the uri if present

  static bool getParams(const std::string& uri, std::string& params);
    /// Returns the entire parameter segment of the uri if present

  bool setParams(const std::string& params);
    /// Replace the params list of a uri with a new one

  static bool setParams(std::string& uri, const std::string& params);
    /// Replace the params list of a uri with a new one

  bool hasParam(const char* paramName) const;
    /// Returns true if the parameter is found

  static bool hasParam(const std::string& uri, const char* paraName);
    /// Returns true if the parameter is found

  std::string getParam(const char* paramName) const;
    /// Returns the value of any uri parameter if present.
    /// 

  bool getParam(const char* paramName, std::string& value) const;
    /// Returns the value of any uri parameter if present.
    ///

  bool getEscapedParam(const char* paramName, std::string& value) const;
    /// Same as getParam() except the value will be unescaped

  std::string getEscapedParam(const char* paramName) const;
    /// Same as getParam() except the value will be unescaped

  static bool getParam(const std::string& uri, const char* paramName, std::string& paramValue);
    /// Returns the value of any uri parameter if present.

  static bool getParamEx(const std::string& params, const char* paramName, std::string& paramValue);
    /// Returns the value of any uri parameter if present.
    /// 
    /// This is basically the same as getParam except that it requires
    /// that the first parameter is already the parsed parameter list
    /// usually coming from the return value of a previous call to getParams()
    ///
    /// This is the recommended function to use when traversing uri parameters
    /// because getParam() function would always parse the params list
    /// which requires it to traverse the entire uri string everytime
    ///
    /// Usage:
    ///
    ///   std::string params = uri.getParams();
    ///   std::string userParam;
    ///   SIPURI::getParamEx(params, "user", userParam);
    ///   std::string transportParam;
    ///   SIPURI::getParamEx(params, "transport", transportParam);

  bool setParam(const char* paramName, const char* paramValue);
    /// Set the value of the URI parameter.
    ///
    /// If the parameter exists it will be replaced.
    /// A new one will be created if it doesn't.
    ///
    /// Take note that this function may throw SIPABNFSyntaxException
    /// if there are invalid characters in the parameter.
    ///
    /// Use SIPURI::escapeParam() function to escape the invalid characters.

  static bool setParam(std::string& uri, const char* paramName, const char* paramValue);
    /// Set the value of the URI parameter.
    ///
    /// If the parameter exists it will be replaced.
    /// A new one will be created if it doesn't.
    ///
    /// Take note that this function may throw SIPABNFSyntaxException
    /// if there are invalid characters in the parameter.
    ///
    /// Use SIPURI::escapeParam() function to escape the invalid characters.

  bool setEscapedParam(const char* paramName, const char* paramValue);
    /// Same as setParam except the paramValue will be escaped first prior to setting

  static bool setParamEx(std::string& params, const char* paramName, const char* paramValue);
    /// Set the value of the URI parameter.
    /// 
    /// This is basically the same as setParam except that it requires
    /// that the first parameter is already the parsed parameter list
    /// usually coming from the return value of a previous call to getParams()
    ///
    /// This is the recommended function to use when setting multiple uri parameters
    /// because setParam() function would always parse the params list
    /// which requires it to traverse the entire uri string everytime.
    ///
    /// Usage:
    ///
    ///   std::string params = uri.getParams();
    ///   SIPURI::setParamEx(params, "user", "phone");
    ///   SIPURI::setParamEx(params, "transport", "udp");
    ///   uri.setParams(params);
    ///
    /// Take note that this function may throw SIPABNFSyntaxException
    /// if there are invalid characters in the parameter.
    ///
    /// Use SIPURI::escapeParam() function to escape the invalid characters.

  static void escapeUser(std::string& result, const char* user);
    /// Escape none valid user characters in a string using RFC 2396 procedure.

  static void escapeParam(std::string& result, const char* param);
    /// Escape none valid param characters in a string using RFC 2396 procedure.

  std::string getHeader(const char* header);
    /// Return the value of the uri header if present

  std::string getHeaders() const;
    /// Returns the entire header segment of the uri if present

  bool getHeaders(SIPURI::header_tokens& tokens) const;
    /// Retrieves the tokenized header tokens.
    /// Returns false if there are no headers defined

  static bool getHeaders(const std::string& uri, std::string& headers);
    /// Returns the entire header segment of the uri if present

  bool setHeaders(const std::string& headers);
    /// Replace the headers of a uri with a new one

  static bool setHeaders(std::string& uri, const std::string& headers);
    /// Replace the headers of a uri with a new one

  bool verify() const;
    /// Verifies the validity of a SIP URI based on RFC 3261 ABNF grammar.

  static bool verify(const char* uri);
    /// Verifies the validity of a SIP URI based on RFC 3261 ABNF grammar.

  std::string getIdentity(bool includeScheme = true) const;
    /// Returns scheme + user + hostport

  static bool getIdentity(const std::string& uri, std::string& identity, bool includeScheme = true);
    /// Returns scheme + user + hostport
  
  void setData(const std::string& data);
    /// Set the data held by this uri
};

//
// inlines
//

inline bool SIPURI::getParam(const char* paramName, std::string& value) const
{
  return getParam(_data, paramName, value);
}

inline bool SIPURI::setEscapedParam(const char* paramName, const char* paramValue)
{
  std::string escaped;
  escapeParam(escaped, paramValue);
  return setParam(paramName, escaped.c_str());
}

inline bool SIPURI::getEscapedParam(const char* paramName, std::string& value) const
{
  if (getParam(paramName, value))
  {
    SIPParser::unescape(value, value.c_str());
    return true;
  }
  return false;
}

inline std::string SIPURI::getEscapedParam(const char* paramName) const
{
  std::string value;
  getEscapedParam(paramName, value);
  return value;
}

inline void SIPURI::setData(const std::string& data)
{
  _data = data;
}


}} // OSS::SIP
#endif // SIP_SIPURI_INCLUDED
