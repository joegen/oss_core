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


#include "OSS/SDP/SDPHeader.h"


namespace OSS {
namespace SDP {



SDPHeader::SDPHeader(char name, const char* value) :
  _name(name),
  _value()
{
  if (value != 0)
    _value = value;
}


SDPHeader::SDPHeader(const char* rawHeader) :
  _name(0),
  _value()
{
  parse(rawHeader);
}

SDPHeader::SDPHeader(const SDPHeader& header) :
  _name(header._name),
  _value(header._value)
{
}

SDPHeader::~SDPHeader()
{
}

void SDPHeader::swap(SDPHeader& header)
{
  std::swap(_name, header._name);
  std::swap(_value, header._value);
}

SDPHeader& SDPHeader::operator = (const SDPHeader& header)
{
  SDPHeader swapable(header);
  swap(swapable);
  return *this;
}

SDPHeader& SDPHeader::operator = (const char* rawHeader)
{
  SDPHeader swapable(rawHeader);
  swap(swapable);
  return *this;
}

void SDPHeader::parse(const char* rawHeader)
{
  OSS_VERIFY_NULL(rawHeader);
  _name = rawHeader[0];
  _value = rawHeader + 2;
}


} }// OSS::SIP::SDP
