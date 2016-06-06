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


#ifndef SIPStatusLine_INCLUDED
#define SIPStatusLine_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"

namespace OSS {
namespace SIP {

class OSS_API SIPStatusLine : public SIPParser
{
public:
  SIPStatusLine();
    /// Create a blank status line

  SIPStatusLine(const std::string& sline);
    /// Create a new status line from a string

  SIPStatusLine(const SIPStatusLine& sline);
    /// Create a new status line from another

  ~SIPStatusLine();
    /// Destroy the status line

  SIPStatusLine& operator = (const std::string& sline);
    /// Copy a status line from a string

  SIPStatusLine operator = (const SIPStatusLine& sline);
    /// Copy a status line from another status line

  void swap(SIPStatusLine& sline);
    /// Exchange the values of two status lines

  bool getVersion(std::string& version) const;
    /// Get the version token
  
  std::string getVersion() const;
    /// Get the version token

  static bool getVersion(const std::string& sline, std::string& version);
    /// Get the version token from a string

  bool setVersion(const char* version);
    /// Set the version

  static bool setVersion(std::string& sline, const char* version);
    /// Set the version of a valid startline string

  bool getStatusCode(std::string& statusCode) const; 
    /// Get the status code token

  unsigned int getStatusCode() const;
    /// Get the status code token
  
  static bool getStatusCode(const std::string& sline,std::string& statusCode);
    /// Get the status code token from a string

  bool setStatusCode(const char* statusCode);
    /// Set the status code
  
  bool setStatusCodeInt(unsigned int statusCode);
  /// Set the status code

  static bool setStatusCode(std::string& sline, const char* statusCode);
    /// Set the status code of a valid startline string

  bool getReasonPhrase(std::string& reasonPhrase) const;
    /// Get the reason phrase token
  
  std::string getReasonPhrase() const;
    /// Get the reason phrase token

  static bool getReasonPhrase(const std::string& sline, std::string& reasonPhrase);
    /// Get the reason phrase token from a string

  bool setReasonPhrase(const char* reasonPhrase);
    /// Set the reason phrase

  static bool setReasonPhrase(std::string& sline, const char* reasonPhrase);
    /// Set the reason phrase of a valid startline string
  
  static const char* EMPTY_STATUS_LINE;
};

//
// Inlines
//

} } // OSS::SIP
#endif // SIPStatusLine_INCLUDED
