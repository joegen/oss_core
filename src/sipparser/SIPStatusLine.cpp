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


#include "OSS/SIP/SIPStatusLine.h"
#include "OSS/ABNF/ABNFBaseRule.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {


const char* SIPStatusLine::EMPTY_STATUS_LINE = "SIP/2.0 0 INVALID";


static void check_empty(SIPStatusLine* rline)
{
  if (rline->data().empty())
  {
    rline->data() = SIPStatusLine::EMPTY_STATUS_LINE;
  }
}

static bool is_empty(const char* str)
{
  return (!str || strlen(str) == 0);
}

SIPStatusLine::SIPStatusLine()
{
  _data = SIPStatusLine::EMPTY_STATUS_LINE;
}

SIPStatusLine::SIPStatusLine(const std::string& sline)
{
  _data = sline;
}

SIPStatusLine::SIPStatusLine(const SIPStatusLine& sline)
{
  _data = sline._data;
}

SIPStatusLine::~SIPStatusLine()
{
}

SIPStatusLine& SIPStatusLine::operator = (const std::string& sline)
{
  _data = sline;
  return *this;
}

SIPStatusLine SIPStatusLine::operator = (const SIPStatusLine& sline)
{
  _data = sline._data;
  return *this;
}

void SIPStatusLine::swap(SIPStatusLine& sline)
{
  std::swap(sline._data, _data);
}

bool SIPStatusLine::getVersion(const std::string& sline, std::string& version)
{
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  version = std::string(sline.c_str(), versionOffSet-1);
  return true;
}

bool SIPStatusLine::setVersion(std::string& sline, const char* version)
{
  if (is_empty(version))
  {
    return false;
  }
  
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  std::ostringstream data;
  data << version << " " << versionOffSet;
  sline = data.str();
  return true;
}

bool SIPStatusLine::getStatusCode(const std::string& sline,std::string& statusCode)
{
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  const char* statusCodeOffSet = ABNF::findNextIterFromString(" ", versionOffSet);
  if (versionOffSet == statusCodeOffSet)
    return false;
  statusCode = std::string(versionOffSet, statusCodeOffSet-1);
  return true;
}

unsigned int  SIPStatusLine::getStatusCode() const
{
  std::string scode;
  if (!getStatusCode(scode))
  {
    return 0;
  }
  return OSS::string_to_number<unsigned int>(scode.c_str(), 0);
}

bool SIPStatusLine::setStatusCode(std::string& sline, const char* statusCode)
{
  std::ostringstream data;
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  data << std::string(sline.c_str(), versionOffSet) << statusCode;
  const char* statusCodeOffSet = ABNF::findNextIterFromString(" ", versionOffSet);
  if (versionOffSet == statusCodeOffSet)
    return false;
  data << " " << statusCodeOffSet;
  sline = data.str();
  return true;
}

bool SIPStatusLine::getReasonPhrase(const std::string& sline, std::string& reasonPhrase)
{
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  const char* statusCodeOffSet = ABNF::findNextIterFromString(" ", versionOffSet);
  if (versionOffSet == statusCodeOffSet)
    return false;
  reasonPhrase = std::string(statusCodeOffSet);
  return true;
}

bool SIPStatusLine::setReasonPhrase(std::string& sline, const char* reasonPhrase)
{
  const char* versionOffSet = ABNF::findNextIterFromString(" ", sline.c_str());
  if (versionOffSet == sline.c_str())
    return false;
  const char* statusCodeOffSet = ABNF::findNextIterFromString(" ", versionOffSet);
  if (versionOffSet == statusCodeOffSet)
    return false;
  sline = std::string(sline.c_str(), statusCodeOffSet);
  sline += reasonPhrase;
  return true;
}

bool SIPStatusLine::getVersion(std::string& version) const
{
  return SIPStatusLine::getVersion(_data, version);
}

std::string SIPStatusLine::getVersion() const
{
  std::string version; 
  getVersion(version); 
  return version;
}

bool SIPStatusLine::setVersion(const char* version)
{
  if (is_empty(version))
  {
    return false;
  }
  check_empty(this);
  return SIPStatusLine::setVersion(_data, version);
}

bool SIPStatusLine::getStatusCode(std::string& statusCode) const
{
  return SIPStatusLine::getStatusCode(_data, statusCode);
}

bool SIPStatusLine::setStatusCode(const char* statusCode)
{
  if (is_empty(statusCode))
  {
    return false;
  }
  check_empty(this);
  return SIPStatusLine::setStatusCode(_data, statusCode);
}

bool SIPStatusLine::setStatusCodeInt(unsigned int statusCode)
{
  return setStatusCode(OSS::string_from_number<unsigned int>(statusCode).c_str());
}

bool SIPStatusLine::getReasonPhrase(std::string& reasonPhrase) const
{
  return SIPStatusLine::getReasonPhrase(_data, reasonPhrase);
}

std::string SIPStatusLine::getReasonPhrase() const
{
  std::string reasonPhrase;
  getReasonPhrase(reasonPhrase);
  return reasonPhrase;
}

bool SIPStatusLine::setReasonPhrase(const char* reasonPhrase)
{
  if (is_empty(reasonPhrase))
  {
    return false;
  }
  check_empty(this);
  return SIPStatusLine::setReasonPhrase(_data, reasonPhrase);
}

} } // OSS::SIP


