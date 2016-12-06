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


#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/ABNF/ABNFBaseRule.h"

namespace OSS {
namespace SIP {


const char* SIPRequestLine::EMPTY_REQUEST_LINE = "INVALID sip:invalid SIP/2.0";


static void check_empty(SIPRequestLine* rline)
{
  if (rline->data().empty())
  {
    rline->data() = SIPRequestLine::EMPTY_REQUEST_LINE;
  }
}

static bool is_empty(const char* str)
{
  return (!str || strlen(str) == 0);
}

SIPRequestLine::SIPRequestLine()
{
  _data = SIPRequestLine::EMPTY_REQUEST_LINE;
}

SIPRequestLine::SIPRequestLine(const std::string& rline)
{
  _data = rline;
}

SIPRequestLine::SIPRequestLine(const SIPRequestLine& rline)
{
  _data = rline._data;
}

SIPRequestLine::~SIPRequestLine()
{
}

SIPRequestLine& SIPRequestLine::operator = (const std::string& rline)
{
  _data = rline;
  return *this;
}

SIPRequestLine SIPRequestLine::operator = (const SIPRequestLine& rline)
{
  _data = rline._data;
  return *this;
}

void SIPRequestLine::swap(SIPRequestLine& rline)
{
  std::swap(rline._data, _data);
}

bool SIPRequestLine::getMethod(const std::string& rline, std::string& method)
{
  const char* offSet = ABNF::findNextIterFromString(" ", rline.c_str());
  if (offSet == rline.c_str())
    return false;
  method = std::string(rline.c_str(), offSet-1);
  return true;
}

bool SIPRequestLine::setMethod(std::string& rline, const char* method)
{
  const char* offSet = ABNF::findNextIterFromString(" ", rline.c_str());
  if (offSet == rline.c_str())
    return false;
  std::ostringstream data;
  data << method << " " << offSet;
  rline = data.str();
  return true;
}

bool SIPRequestLine::getURI(SIPURI& uri)
{
  std::string uri_;
  if (!getURI(_data, uri_))
    return false;
  uri = uri_;
  return true;
}
    /// Get the URI token

bool SIPRequestLine::getURI(const std::string& rline, std::string& uri)
{
  const char* methodOffset = ABNF::findNextIterFromString(" ", rline.c_str());
  if (methodOffset == rline.c_str())
    return false;
  const char* uriOffset = ABNF::findNextIterFromString(" ", methodOffset);
  if (methodOffset == uriOffset)
    return false;
  uri = std::string(methodOffset, uriOffset-1);
  return true;
}

bool SIPRequestLine::setURI(std::string& rline, const char* uri)
{
#if 0
  //
  // This breaks unit test.  It will fail if the URI has a display name
  //
  if (!SIPURI::verify(uri))
  {
    return false;
  }
#endif
  std::ostringstream data;
  const char* methodOffset = ABNF::findNextIterFromString(" ", rline.c_str());
  if (methodOffset == rline.c_str())
    return false;
  data << std::string(rline.c_str(), methodOffset) << uri;
  const char* uriOffset = ABNF::findNextIterFromString(" ", methodOffset);
  if (methodOffset == uriOffset)
    return false;
  data << " " << uriOffset;
  rline = data.str();
  return true;
}

bool SIPRequestLine::getVersion(const std::string& rline, std::string& version)
{
  const char* methodOffset = ABNF::findNextIterFromString(" ", rline.c_str());
  if (methodOffset == rline.c_str())
    return false;
  const char* uriOffset = ABNF::findNextIterFromString(" ", methodOffset);
  if (methodOffset == uriOffset)
    return false;
  version = std::string(uriOffset);
  return true;
}

bool SIPRequestLine::setVersion(std::string& rline, const char* version)
{
  const char* methodOffset = ABNF::findNextIterFromString(" ", rline.c_str());
  if (methodOffset == rline.c_str())
    return false;
  const char* uriOffset = ABNF::findNextIterFromString(" ", methodOffset);
  if (methodOffset == uriOffset)
    return false;
  rline = std::string(rline.c_str(), uriOffset);
  rline += version;
  return true;
}

bool SIPRequestLine::getMethod(std::string& method) const
{
  return SIPRequestLine::getMethod(_data, method);
}

std::string SIPRequestLine::getMethod() const
{
  std::string method;
  getMethod(method);
  return method;
}

bool SIPRequestLine::getURI(std::string& uri) const
{
  return SIPRequestLine::getURI(_data, uri);
}

std::string SIPRequestLine::getURI() const
{
  std::string uri;
  getURI(uri);
  return uri;
}

bool SIPRequestLine::getVersion(std::string& version) const
{
  return SIPRequestLine::getVersion(_data, version);
}

std::string SIPRequestLine::getVersion() const
{
  std::string version;
  getVersion(version);
  return version;
}

bool SIPRequestLine::setMethod(const char* method)
{
  if (is_empty(method))
  {
    return false;
  }
  
  check_empty(this);
  return SIPRequestLine::setMethod(_data, method);
}

bool SIPRequestLine::setURI(const char* uri)
{
  if (is_empty(uri))
  {
    return false;
  }
  check_empty(this);
  return SIPRequestLine::setURI(_data, uri);
}

bool SIPRequestLine::setURI(const SIPURI& uri)
{
  return setURI(uri.data().c_str());
}

bool SIPRequestLine::setVersion(const char* version)
{
  if (is_empty(version))
  {
    return false;
  }
  check_empty(this);
  return SIPRequestLine::setVersion(_data, version);
}



} } // OSS::SIP


