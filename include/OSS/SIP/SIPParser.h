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


#ifndef SIP_SIPParser_INCLUDED
#define SIP_SIPParser_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParserException.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <map>
#include <sstream>


namespace OSS {
namespace SIP {


class OSS_API SIPParser
/// This class holds various helper functions to be used
/// later by SIPHeader and SIPMessage classes
{
public:
  SIPParser();
    /// Creates a new SIP Parser

  virtual ~SIPParser();
    /// Destroys the Parser object

  const std::string& data() const;
  std::string& data();
    /// Returns the raw bytes representing the parser string.
    ///
    /// If the parser object has been modified using the external object setters,
    /// data() will not return an updated version of the modifed string!!!
    /// You must call commitData() function to signal the parser
    /// to update the data with the current changes.  This will be handy
    /// if the application needs to preserve the old parser string before updating.

  static void escape(std::string& result, const char* str, const char* validChars = 0);
    /// Escape none valid characters in a string using RFC 2396 procedure.

  static void unescape(std::string& result, const char* str);
    /// Unescape none valid characters in a string using RFC 2396 procedure.

  static void unquoteString(std::string& quotedString);
    /// Unquote a quoted string

  static void enquoteString(std::string& unQuotedString);
    /// Enclose a string with quotes

  static bool isChar(int c);
    /// Check if a byte is a SIP character.

  static bool isCtl(int c);
    /// Check if a byte is an SIP control character.
  
  static bool isDigit(int c);
    /// Check if a byte is a digit.

  static void getReasonPhrase( 
    int statusCode,
    std::string& reasonPhrase
  );
    /// Returns the default reason phrase for the status code.
    ///
    /// If reasonPhrase parameter is not empty, this function
    /// will return immediately without changing the value.
    ///

  static std::string createBranchString();
    /// Create a via branch parameter string

  static std::string createTagString();
    /// Create a from/top tag parameter string

protected:
  std::string _data;
};

//
// inlines
//

inline const std::string & SIPParser::data() const
{
  return _data;
}

inline std::string & SIPParser::data()
{
  return _data;
}

}} //OSS::SIP
#endif //SIP_SIPParser_INCLUDED
