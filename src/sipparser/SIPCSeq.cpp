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


#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {


using namespace OSS::ABNF;
static ABNFWhileNot<ABNF_SIP_LWS, false> lwsFinder_1;
static ABNFWhileNot<ABNF_SIP_LWS, true> lwsFinder_2;

SIPCSeq::SIPCSeq()
{
  _data = "UNKNOWN 0";
}

SIPCSeq::SIPCSeq(const std::string& cseq)
{
  _data = cseq;
}

SIPCSeq::SIPCSeq(const SIPCSeq& cseq)
{
  _data = cseq._data;
}

SIPCSeq::~SIPCSeq()
{
}

SIPCSeq& SIPCSeq::operator = (const std::string& cseq)
{
  _data = cseq;
  return *this;
}

SIPCSeq& SIPCSeq::operator = (const SIPCSeq& cseq)
{
  SIPCSeq clonable(cseq);
  swap(clonable);
  return *this;
}

void SIPCSeq::swap(SIPCSeq& cseq)
{
  std::swap(_data, cseq._data);
}

std::string SIPCSeq::getMethod() const
{
  std::string method;
  getMethod(_data, method);
  return method;
}

bool SIPCSeq::getMethod(const std::string& cseq, std::string& method)
{
  char* offSet = lwsFinder_2.parse(cseq.c_str());
  if (offSet == cseq.c_str())
    return false;
  method = std::string(offSet);
  OSS::string_to_upper(method);
  return true;
}

bool SIPCSeq::setMethod(const char* method)
{
  return setMethod(_data, method);
}

bool SIPCSeq::setMethod(std::string& cseq, const char* method_)
{
  
  char* offSet = lwsFinder_2.parse(cseq.c_str());
  char* method = const_cast<char*>(method_);
  if (offSet == cseq.c_str())
    return false;
  std::string front(cseq.c_str(), (const char*)offSet);
  front += method;
  cseq = front;
  return true;
}

std::string SIPCSeq::getNumber() const
{
  std::string number;
  getNumber(_data, number);
  return number;
}

bool SIPCSeq::getNumber(const std::string& cseq, std::string& number)
{
  char* offSet = lwsFinder_1.parse(cseq.c_str());
  if( offSet == cseq.c_str())
    return false;
  number = std::string(cseq.c_str(), (const char*)offSet);
  return true;
}

bool SIPCSeq::setNumber(const std::string& number)
{
  return setNumber(_data, number);
}

bool SIPCSeq::setNumber(unsigned int number)
{
  return setNumber(_data, number);
}

bool SIPCSeq::setNumber(std::string& cseq, unsigned int number)
{
  return setNumber(cseq, OSS::string_from_number<unsigned int>(number));
}

bool SIPCSeq::setNumber(std::string& cseq, const std::string& number)
{
  char* offSet = lwsFinder_1.parse(cseq.c_str());
  if( offSet == cseq.c_str())
    return false;
  std::string front(number);
  front += offSet;
  cseq = front;
  return true;
}


} } // OSS::SIP



