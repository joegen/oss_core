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


#include "OSS/SIP/SIPHeaderTokens.h"


namespace OSS {
namespace SIP {


SIPHeaderTokens::SIPHeaderTokens()
{
  _headerOffSet = std::string::npos;
}

SIPHeaderTokens::SIPHeaderTokens(const SIPHeaderTokens& tokens) :
std::vector<std::string>(tokens),
  _rawHeaderName(tokens._rawHeaderName),
  _headerOffSet(tokens._headerOffSet)
{
}

SIPHeaderTokens& SIPHeaderTokens::operator=(const SIPHeaderTokens& tokens)
{
  SIPHeaderTokens swapable(tokens);
  swap(swapable);
  return *this;
}

void SIPHeaderTokens::swap(SIPHeaderTokens& tokens)
{
  dynamic_cast<std::vector<std::string>* >(this)->swap(tokens);
  _rawHeaderName = tokens._rawHeaderName;
  _headerOffSet = tokens._headerOffSet;
}

std::string& SIPHeaderTokens::rawHeaderName()
{
  return _rawHeaderName;
}

size_t& SIPHeaderTokens::headerOffSet()
{
  return _headerOffSet;
}

void SIPHeaderTokens::push_front(const std::string& header)
{
  SIPHeaderTokens oldTokens;
  oldTokens = *this;
  clear();
  push_back(header);

  SIPHeaderTokens::iterator it;
  for (it = oldTokens.begin(); it != oldTokens.end(); it++)
    push_back(*it);
}

} } // OSS::SIP



