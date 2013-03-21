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

namespace OSS {
namespace SIP {


SIPStatusLine::SIPStatusLine()
{
  _data = "SIP/2.0 * *";
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

} } // OSS::SIP


