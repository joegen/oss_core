// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SDP
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


#ifndef SIP_SDPHeaderList_H_INCLUDED
#define SIP_SDPHeaderList_H_INCLUDED


#include <list>

#include "OSS/SDP/SDP.h"
#include "OSS/SDP/SDPHeader.h"


namespace OSS {
namespace SDP {


class OSS_API SDPHeaderList : public std::list<SDPHeader>
{
public:
  enum ParserState
  {
    STATE_EXPECTING_NAME,
    STATE_EXPECTING_EQUAL,
    STATE_EXPECTING_BODY,
    STATE_EXPECTING_LINE_END_OR_NAME
  };

  SDPHeaderList();
    /// Creates an SDP Header

  explicit SDPHeaderList(const char* rawHeader, char exitName = '\0');
    /// Create an SDP header from an unparsed header string

  SDPHeaderList(const SDPHeaderList& header);
    /// SDP Header copy constructor

  ~SDPHeaderList();
    /// Destroys the SDP header

  void parse(const char* rawHeader);
    /// Parse an SDP header string

  SDPHeaderList& operator = (const SDPHeaderList& header);
    /// Copy another header by value

  SDPHeaderList& operator = (const char* rawHeader);
    /// Copy from a raw header;

  void swap(SDPHeaderList& header);
    /// Exchange the contents of two headers
    
  std::string toString() const;
    /// Convert this header to a string

  bool isValid() const;
    /// Return true if the parser determines the SDP in proper order.
    /// Take note that this only checks for basic format and not actual ABNF syntax check

  const std::string& getTail() const;
    /// Returns the extra string that is left to be parsed after
    /// a call to parse exits due to encountering exit name

protected:
  int _parserState;
  bool _isValid;
  char _exitName;
  std::string _tail;
};

//
// Inlines
//

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const SDPHeaderList& h)
{
  os << h.toString();
  return os;
}


inline std::string SDPHeaderList::toString() const
{
  if (!_isValid)
    return "";

  std::stringstream sdp;
  const_iterator iter;
  for (iter = begin(); iter != end(); iter++)
    sdp << *iter << "\r\n";
  
  return sdp.str();
}

inline bool SDPHeaderList::isValid() const
{
  return _isValid;
}

inline const std::string& SDPHeaderList::getTail() const
{
  return _tail;
}

} } // OSS::SDP
#endif // SIP_SDPHeaderList_H_INCLUDED

