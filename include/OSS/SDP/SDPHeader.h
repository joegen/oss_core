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


#ifndef SIP_SDPHeader_H_INCLUDED
#define SIP_SDPHeader_H_INCLUDED


#include <sstream>

#include "OSS/SDP/SDP.h"
#include "OSS/Core.h"


namespace OSS {
namespace SDP {


class OSS_API SDPHeader
{
public:
  SDPHeader(char name = 0, const char* value = 0);
    /// Creates an SDP Header

  explicit SDPHeader(const char* rawHeader);
    /// Create an SDP header from an unparsed header string

  SDPHeader(const SDPHeader& header);
    /// SDP Header copy constructor

  ~SDPHeader();
    /// Destroys the SDP header

  void parse(const char* rawHeader);
    /// Parse an SDP header string

  SDPHeader& operator = (const SDPHeader& header);
    /// Copy another header by value

  SDPHeader& operator = (const char* rawHeader);
    /// Copy from a raw header;

  bool operator == (const SDPHeader& header) const;
    /// Equality operator

  bool operator < (const SDPHeader& header) const;
    /// Less than operator

  void swap(SDPHeader& header);
    /// Exchange the contents of two headers
    
  char& name();
    /// Return a direct reference to the name character

  std::string& value();
    /// Return a direct reference to the vlaue string

  std::string toString() const;
    /// Convert this header to a string

private:
  char _name;
  std::string _value;
};

//
// Inlines
//

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const SDPHeader& h)
{
  os << h.toString();
  return os;
}

inline char& SDPHeader::name()
{
  return _name;
}

inline std::string& SDPHeader::value()
{
  return _value;
}

inline std::string SDPHeader::toString() const
{
  if (_name != 0 && !_value.empty())
  {
    std::stringstream sdp;
    sdp << _name << "=" << _value;
    return sdp.str();
  }
  return "";
}

inline bool SDPHeader::operator == (const SDPHeader& header) const
{
  return _name == header._name && _value == header._value;
}

inline bool SDPHeader::operator < (const SDPHeader& header) const
{
  return toString() < header.toString();
}

} } // OSS::SDP
#endif // SIP_SDPHeader_H_INCLUDED

