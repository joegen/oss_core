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

#include <locale>

#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/ABNF/ABNFParser.h"
#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPToken.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"
#include <sstream>

namespace OSS {
namespace SIP {


using namespace OSS::ABNF;

SIPVia::SIPVia()
{
  _data = "SIP/2.0/UDP invalid:0;branch=empty";
}

SIPVia::SIPVia(const std::string& via)
{
  _data = via;
}

SIPVia::SIPVia(const SIPVia& via)
{
  _data = via._data;
}

SIPVia::~SIPVia()
{
}

SIPVia& SIPVia::operator = (const std::string& via)
{
  SIPVia clone(via);
  swap(clone);
  return *this;
}

SIPVia& SIPVia::operator = (const SIPVia& via)
{
  SIPVia clone(via);
  swap(clone);
  return *this;
}

void SIPVia::swap(SIPVia& via)
{
  std::swap(via._data, _data);
}

std::string SIPVia::getSentBy() const
{
  std::string sentBy;
  getSentBy(_data, sentBy);
  return sentBy;
}

bool SIPVia::getSentBy(const std::string& via, std::string& sentBy)
{
  typedef ABNFLRSequence5<ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token> _pvar1; //sent-protocol =  protocol-name SLASH protocol-version SLASH transport
  static ABNFLRSequence3<_pvar1, ABNF_SIP_LWS, ABNF_SIP_hostport> parser;
  
  ABNFTokens tokens;  
  parser.parseTokens(via.c_str(), tokens);
  if (tokens.size() != 3)
    return false;
  sentBy = tokens[2];
  return true;
}

bool SIPVia::getSentByAddress(OSS::Net::IPAddress& sentBy)
{
  return SIPVia::getSentByAddress(_data, sentBy);
}

bool SIPVia::getSentByAddress(const std::string& via, OSS::Net::IPAddress& sentBy)
{
  std::string address;
  if (!SIPVia::getSentBy(via, address))
  {
    return false;
  }
  sentBy = OSS::Net::IPAddress::fromV4IPPort(address.c_str());
  
  if (!sentBy.isValid())
  {
    return false;
  }
  
  std::string transport;
  SIPVia::getTransport(via, transport);
  
  if (transport == "UDP")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::UDP);
  }
  else if (transport == "TCP")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::TCP);
  }
  else if (transport == "TLS")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::TLS);
  }
  else if (transport == "WS")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::WS);
  }
  else if (transport == "WSS")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::WSS);
  }
  else if (transport == "SCTP")
  {
    sentBy.setProtocol(OSS::Net::IPAddress::SCTP);
  }
  
  return true;
}

 bool SIPVia::setSentBy(const char* hostPort)
 {
   return setSentBy(_data, hostPort);
 }

 bool SIPVia::setSentBy(std::string& via, const char* hostPort)
 {
   typedef ABNFLRSequence5<ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token> _pvar1; //sent-protocol =  protocol-name SLASH protocol-version SLASH transport
  static ABNFLRSequence3<_pvar1, ABNF_SIP_LWS, ABNF_SIP_hostport> parser;
  
  ABNFTokens tokens;  
  parser.parseTokens(via.c_str(), tokens);
  if (tokens.size() != 3)
    return false;
  
  std::ostringstream newData;
  newData << tokens[0] << " " << hostPort;
  
  //
  // append the parameters
  //
  const char* params = ABNF::findNextIterFromString(";", via.c_str());
  if (params && params != via.c_str())
  {
    newData << ";" << params;
  }
  else
  {
    newData << ";branch=unset";
  }
  
  via = newData.str();
  
  return true;
 }

std::string SIPVia::getTransport() const
{
  std::string transport;
  getTransport(_data, transport);
  return transport;
}

bool SIPVia::getTransport(const std::string& via, std::string& transport)
{
  ABNFLRSequence5<ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token, ABNF_SIP_SLASH, ABNF_SIP_token> parser; //sent-protocol =  protocol-name SLASH protocol-version SLASH transport

  ABNFTokens tokens;  
  parser.parseTokens(via.c_str(), tokens);
  if (tokens.size() != 5)
    return false;
  transport = tokens[4];
  OSS::string_to_upper(transport);
  return true;
}

void SIPVia::setTransport(const std::string& transport)
{
  SIPVia::setTransport(_data, transport);
}

bool SIPVia::setTransport(std::string& via, const std::string& transport)
{
  std::ostringstream newData;
  newData << "SIP/2.0/" << transport;
  std::string tail = OSS::string_right(via, via.size() - newData.str().size());
  newData << tail;
  via = newData.str();
  return true;
}

std::string SIPVia::getParam(const char* paramName) const
{
  std::string paramValue;
  getParam(_data, paramName, paramValue);
  return paramValue;
}

bool SIPVia::getParam(const std::string& via, const char* paramName, std::string& paramValue)
{
  static ABNF::ABNF_SIP_pvalue parser;

  if (via.empty())
    return false;

  std::string key = paramName;
  key += "=";

  char* startIter = ABNF::findNextIterFromString(key, via.c_str());
  if (startIter == via.c_str())
    return false;

  char* newIter = parser.parse(startIter);
  if (newIter == startIter)
    return false;
  
  paramValue = std::string(startIter, newIter);
  return true;
}

std::string SIPVia::getBranch() const
{
  std::string paramValue;
  getBranch(_data, paramValue);
  return paramValue;
}

bool SIPVia::getBranch(const std::string& via, std::string& paramValue)
{
  return getParam(via, "branch", paramValue);
}

std::string SIPVia::getRPort() const
{
  std::string paramValue;
  getRPort(_data, paramValue);
  return paramValue;
}

bool SIPVia::getRPort(const std::string& via, std::string& paramValue)
{
  if (!getParam(via, "rport", paramValue))
    return hasParam(via, "rport");
  return true;
}

std::string SIPVia::getReceived() const
{
  std::string paramValue;
  getReceived(_data, paramValue);
  return paramValue;
}

bool SIPVia::getReceived(const std::string& via, std::string& paramValue)
{
  return getParam(via, "received", paramValue);
}

bool SIPVia::hasParam(const char* paramName) const
{
  return hasParam(_data, paramName);
}

bool SIPVia::hasParam(const std::string& via, const char* paramName)
{
  std::string key = paramName;
  boost::to_lower(key);
  if( key != "rport" ) // rport may not have pvalue
    key += "="; 
  return ABNF::findNextIterFromString(key, via.c_str()) != via.c_str();
}

bool SIPVia::setParam(const char* paramName, const char* paramValue)
{
  return setParam(_data, paramName, paramValue);
}

bool SIPVia::setParam(std::string& via, const char* paramName, const char* paramValue)
{
  static ABNFEvaluate<ABNF_SIP_pname> pnameVerify;
  static ABNFEvaluate<ABNF_SIP_pvalue> pvalueVerify;

  if (!pnameVerify(paramName) || !pvalueVerify(paramValue))
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");

  std::string key = paramName;
  boost::to_lower(key);
  if( key != "rport" ) // rport may not have pvalue
    key += "="; 
  
  char* offSet = ABNF::findNextIterFromString(key, via.c_str());
  if (offSet == via.c_str())
  {
    std::ostringstream strm;
    strm << ";" << paramName << "=" << paramValue;
    via += strm.str();
    return true;
  }

  std::string front(via.c_str(), (const char*)offSet);
  if(front.at(front.size() - 1) != '=')
    front += "=";
  front += paramValue;

  char* tailOffSet = ABNF::findNextIterFromString(";", offSet);
  if (tailOffSet != offSet)
  {
    tailOffSet--;
    front += tailOffSet;
  }

  via = front;
  return true;
}

int SIPVia::countElements(const std::string& via)
{
  int count = 0;
  const char* oldOffSet = via.c_str();
  for (;;)
  {
    char* newOffSet = ABNF::findNextIterFromString(",", oldOffSet);
    if (newOffSet == oldOffSet)
      break;
    oldOffSet = newOffSet;
    count++;
  }
  return count + 1;
}

int SIPVia::splitElements(const std::string& via, std::vector<std::string>& elements)
{
  int count = 0;
  const char* oldOffSet = via.c_str();
  for (;;)
  {
    char* newOffSet = ABNF::findNextIterFromString(",", oldOffSet);
    if (newOffSet == oldOffSet)
    {
      elements.push_back(newOffSet);
      break;
    }
    elements.push_back(std::string(oldOffSet, (const char*)newOffSet - 1));
    std::locale loc;
    while (std::isspace(*newOffSet, loc) && *newOffSet != '\0')
      newOffSet++;
    oldOffSet = newOffSet;
    count++;
  }
  return count + 1;
}

bool SIPVia::getTopVia(const std::string& hVia, std::string& via)
{
  const char* oldOffSet = hVia.c_str();
  char* newOffSet = ABNF::findNextIterFromString(",", oldOffSet);
  if (newOffSet == oldOffSet)
    via = newOffSet;
  else if (newOffSet > oldOffSet)
    via = std::string(oldOffSet, (const char*)newOffSet - 1);
  else
    return false;

  return true;
}

bool SIPVia::msgGetTopVia(SIPMessage* pMsg, std::string& topVia)
{
  OSS_VERIFY_NULL(pMsg);
  if (!pMsg->hdrPresent(OSS::SIP::HDR_VIA))
    return false;
  std::string via = pMsg->hdrGet(OSS::SIP::HDR_VIA);
  return getTopVia(via, topVia);
}

std::string SIPVia::popTopVia(const std::string& hVia, std::string& via)
{
  const char* oldOffSet = hVia.c_str();
  char* newOffSet = ABNF::findNextIterFromString(",", oldOffSet);
  if (newOffSet == oldOffSet)
  {
    via = oldOffSet;
    return "";
  }else if (newOffSet > oldOffSet)
  {
    via = std::string(oldOffSet, (const char*)newOffSet - 1);
    ++newOffSet;
    return std::string(newOffSet);
  }
  return "";
}

bool SIPVia::msgPopTopVia(SIPMessage* pMsg, std::string& topVia)
{
  OSS_VERIFY_NULL(pMsg);
  if (!pMsg->hdrPresent(OSS::SIP::HDR_VIA))
    return false;
  std::string hVia = pMsg->hdrGet(OSS::SIP::HDR_VIA, 0);
  std::string moreVia = popTopVia(hVia, topVia);
  if (!moreVia.empty())
  {
    pMsg->hdrSet(OSS::SIP::HDR_VIA, moreVia, 0);
  }
  else
  {
    pMsg->hdrListPopFront(OSS::SIP::HDR_VIA);
  }
  return !topVia.empty();
}

bool SIPVia::msgAddVia(SIPMessage* pMsg, const std::string& via)
{
  OSS_VERIFY_NULL(pMsg);
  return pMsg->hdrListPrepend(OSS::SIP::HDR_VIA, via);
}

bool SIPVia::msgClearVias(SIPMessage* pMsg)
{
  OSS_VERIFY_NULL(pMsg);
  return pMsg->hdrListRemove(OSS::SIP::HDR_VIA);
}

bool SIPVia::msgGetTopViaSentBy(SIPMessage* pMsg, std::string& sentBy)
{
  std::string topVia;
  if (!msgGetTopVia(pMsg, topVia))
    return false;
  return getSentBy(topVia, sentBy);
}

bool SIPVia::msgGetTopViaSentByAddress(SIPMessage* pMsg, OSS::Net::IPAddress& sentBy)
{
  std::string topVia;
  if (!msgGetTopVia(pMsg, topVia))
    return false;
  return getSentByAddress(topVia, sentBy);
}

bool SIPVia::msgGetTopViaTransport(SIPMessage* pMsg, std::string& transport)
{
  std::string topVia;
  if (!msgGetTopVia(pMsg, topVia))
    return false;
  return getTransport(topVia, transport);
}

bool SIPVia::getBottomVia(const std::string& hVia, std::string& bottomVia)
{
  const char* oldOffSet = hVia.c_str();
  char* newOffSet = ABNF::findNextIterFromString(",", oldOffSet);

  if (newOffSet == oldOffSet)
  {
    bottomVia = newOffSet;
    OSS::string_trim(bottomVia);
    return true;
  }
  //
  // There are more vias in this list.  Do a reverse iterator
  //
  newOffSet = ABNF::findLastIterFromString(",", oldOffSet);
  if (oldOffSet < newOffSet)
  {
    bottomVia = newOffSet;
    OSS::string_trim(bottomVia);
    return true;
  }

  return false;
}

bool SIPVia::msgGetBottomVia(SIPMessage* pMsg, std::string& via)
{
  int count = pMsg->hdrGetSize(OSS::SIP::HDR_VIA);
  if (count == 0)
    return false;
  std::string hVia = pMsg->hdrGet(OSS::SIP::HDR_VIA, count -1);
  if (hVia.empty())
    return false;
  return getBottomVia(hVia, via);
}

bool SIPVia::msgGetBottomViaSentBy(SIPMessage* pMsg, std::string& sentBy)
{
  std::string hVia;
  if (!msgGetBottomVia(pMsg, hVia))
    return false;
  return getSentBy(hVia, sentBy);
}

bool SIPVia::msgGetBottomViaSentByAddress(SIPMessage* pMsg, OSS::Net::IPAddress& sentBy)
{
  std::string hVia;
  if (!msgGetBottomVia(pMsg, hVia))
    return false;
  return getSentByAddress(hVia, sentBy);
}

bool SIPVia::msgGetBottomViaReceived(SIPMessage* pMsg, std::string& received)
{
  std::string hVia;
  if (!msgGetBottomVia(pMsg, hVia))
    return false;
  return getReceived(hVia, received);
}

bool SIPVia::msgGetBottomViaRPort(SIPMessage* pMsg, std::string& rport)
{
  std::string hVia;
  if (!msgGetBottomVia(pMsg, hVia))
    return false;
  return getRPort(hVia, rport);
}

} } // OSS::SIP



