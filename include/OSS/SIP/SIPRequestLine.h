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


#ifndef SIPRequestLine_INCLUDED
#define SIPRequestLine_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPURI.h"

namespace OSS {
namespace SIP {

class OSS_API SIPRequestLine : public SIPParser
{
public:
  SIPRequestLine();
    /// Create a blank request line

  SIPRequestLine(const std::string& rline);
    /// Create a new request line from a string

  SIPRequestLine(const SIPRequestLine& rline);
    /// Create a new request line from another

  ~SIPRequestLine();
    /// Destroy the request line

  SIPRequestLine& operator = (const std::string& rline);
    /// Copy a request line from a string

  SIPRequestLine operator = (const SIPRequestLine& rline);
    /// Copy a request line from another request line

  void swap(SIPRequestLine& rline);
    /// Exchange the values of two request lines

  bool getMethod(std::string& method) const; 
    /// Get the method token

  static bool getMethod(const std::string& rline, std::string& method);
    /// Get the mthod token from a string

  bool getURI(std::string& uri) const;
    /// Get the URI token

  bool getURI(SIPURI& uri);
    /// Get the URI token

  static bool getURI(const std::string& rline, std::string& uri);
    /// Get the URI token from a string

  bool getVersion(std::string& version) const;
    /// Get the version token

  static bool getVersion(const std::string& rline, std::string& version);
    /// Get the version token from a string

  bool setMethod(const char* method);
    /// Set the method token

  static bool setMethod(std::string& rline, const char* method);
    /// Set the method token of a valid startline string

  bool setURI(const SIPURI& uri);
    /// Set the URI token
  
  bool setURI(const char* uri);
    /// Set the URI token

  static bool setURI(std::string& rline, const char* uri);
    /// Set the method of a valid startline string

  bool setVersion(const char* version);
    /// Set the version

  static bool setVersion(std::string& rline, const char* version);
    /// Set the version of a valid startline string
};

//
// Inlines
//

inline bool SIPRequestLine::getMethod(std::string& method) const
{
  return SIPRequestLine::getMethod(_data, method);
}

inline bool SIPRequestLine::getURI(std::string& uri) const
{
  return SIPRequestLine::getURI(_data, uri);
}

inline bool SIPRequestLine::getVersion(std::string& version) const
{
  return SIPRequestLine::getVersion(_data, version);
}

inline bool SIPRequestLine::setMethod(const char* method)
{
  return SIPRequestLine::setMethod(_data, method);
}

inline bool SIPRequestLine::setURI(const char* uri)
{
  return SIPRequestLine::setURI(_data, uri);
}

inline bool SIPRequestLine::setURI(const SIPURI& uri)
{
  return setURI(uri.data().c_str());
}

inline bool SIPRequestLine::setVersion(const char* version)
{
  return SIPRequestLine::setVersion(_data, version);
}

} } // OSS::SIP
#endif // SIPRequestLine_INCLUDED
