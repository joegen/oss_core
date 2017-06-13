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


#include "OSS/SDP/SDPSession.h"


namespace OSS {
namespace SDP {


SDPSession::SDPSession() 
{
  //Session description
  //v=  (protocol version)
  //o=  (originator and session identifier)
  //s=  (session name)
  //i=* (session information)
  //u=* (URI of description)
  //e=* (email address)
  //p=* (phone number)
  //c=* (connection information -- not required if included in
  //     all media)
  //b=* (zero or more bandwidth information lines)
  //One or more time descriptions ("t=" and "r=" lines; see below)
  //z=* (time zone adjustments)
  //k=* (encryption key)
  //a=* (zero or more session attribute lines)
  reset();
  _exitName = 'm';
}

SDPSession::SDPSession(const char* session) :
  SDPHeaderList(session, 'm')
{
  _exitName = 'm';

  if (!_tail.empty())
  {
    while (!_tail.empty())
    {
      SDPMedia::Ptr media(new SDPMedia(_tail.c_str()));
      _mediaDescriptions.push_back(media);
      _tail = media->getTail();
    }
  }
}

SDPSession::SDPSession(const SDPSession& session) :
  SDPHeaderList(session)
{
  ReadLock(session._rwMutex);
  _exitName = 'm';
  _mediaDescriptions = session._mediaDescriptions;
}

SDPSession::~SDPSession()
{
}

SDPSession& SDPSession::operator = (const SDPSession& session)
{
  SDPSession swapable(session);
  swap(swapable);
  return *this;
}

void SDPSession::swap(SDPSession& session)
{
  WriteLock lock1(session._rwMutex);
  WriteLock lock2(_rwMutex);
  std::swap(_mediaDescriptions, session._mediaDescriptions);
  SDPHeaderList::swap(session);
}

void SDPSession::reset()
{
  WriteLock lock(_rwMutex);
  clear();

  SDPHeader v;
  v.name() = 'v';
  v.value() = "0";
  push_back(v);

  SDPHeader o;
  o.name() = 'o';
  o.value() = "_ 0 0 IN IP4 0.0.0.0";
  push_back(o);

  SDPHeader s;
  s.name() = 's';
  s.value() = "_";
  push_back(s);

  SDPHeader t;
  t.name() = 't';
  t.value() = "0 0";
  push_back(t);

  //
  // Set this as valid
  //
  _isValid = true;
  _tail = "";
}

std::string SDPSession::toString() const
{
  ReadLock lock(_rwMutex);
  std::stringstream strm;
  strm << SDPHeaderList::toString();
  MediaDescriptions::const_iterator iter;
  for (iter = _mediaDescriptions.begin(); iter != _mediaDescriptions.end(); iter++)
    strm << (*iter)->toString();
  return strm.str();
}

void SDPSession::addMedia(SDPMedia::Ptr media)
{
  WriteLock lock(_rwMutex);
  _mediaDescriptions.push_back(media);
}

SDPMedia::Ptr SDPSession::getMedia(SDPMedia::Type type, size_t index) const
{
  ReadLock lock(_rwMutex);
  size_t mediaIndex = 0;
  MediaDescriptions::iterator iter;
  for (iter = const_cast<SDPSession*>(this)->_mediaDescriptions.begin();
    iter != const_cast<SDPSession*>(this)->_mediaDescriptions.end();
    iter++)
  {
    SDPMedia::Ptr& media = *iter;
    if (media->getMediaType() == type)
    {
      if (mediaIndex == index)
        return media;
      else
        mediaIndex++;
    }
  }
  return SDPMedia::Ptr();
}

SDPMedia::Ptr SDPSession::getMedia(size_t index) const
{
  ReadLock lock(_rwMutex);
  size_t mediaIndex = 0;
  MediaDescriptions::iterator iter;
  for (iter = const_cast<SDPSession*>(this)->_mediaDescriptions.begin();
    iter != const_cast<SDPSession*>(this)->_mediaDescriptions.end();
    iter++)
  {
    if (mediaIndex == index)
      return *iter;
    else
      mediaIndex++;
  }
  return SDPMedia::Ptr();
}

bool SDPSession::removeMedia(SDPMedia::Type type, size_t index)
{
  WriteLock lock(_rwMutex);
  size_t mediaIndex = 0;
  MediaDescriptions::iterator iter;
  for (iter = _mediaDescriptions.begin();
    iter != _mediaDescriptions.end();
    iter++)
  {
    SDPMedia::Ptr& media = *iter;
    if (media->getMediaType() == type)
    {
      if (mediaIndex == index)
      {
        _mediaDescriptions.erase(iter);
        return true;
      }
      else
      {
        mediaIndex++;
      }
    }
  }
  return false;
}

size_t SDPSession::getMediaCount(SDPMedia::Type type)const
{
  ReadLock lock(_rwMutex);
  size_t mediaCount = 0;
  MediaDescriptions::iterator iter;
  for (iter = const_cast<SDPSession*>(this)->_mediaDescriptions.begin();
    iter != const_cast<SDPSession*>(this)->_mediaDescriptions.end();
    iter++)
  {
    SDPMedia::Ptr& media = *iter;
    if (SDPMedia::TYPE_NONE == type)
      mediaCount++;
    else if (media->getMediaType() == type)
      mediaCount++;
  }
  return mediaCount;
}

SDPSession::iterator SDPSession::findIterator(char name)
{
  SDPSession::iterator iter;
  for (iter=begin(); iter != end(); iter++)
    if (iter->name() == name)
      return iter;
  return end();
}

std::string SDPSession::findHeader(char name) const
{
  ReadLock lock(_rwMutex);
  SDPSession::iterator iter;
  for (iter=const_cast<SDPSession*>(this)->begin(); iter != const_cast<SDPSession*>(this)->end(); iter++)
    if (iter->name() == name)
      return iter->value();
  return "";
}

bool SDPSession::removeHeader(char name)
{
  WriteLock lock(_rwMutex);
  SDPSession::iterator iter;
  for (iter=const_cast<SDPSession*>(this)->begin(); iter != const_cast<SDPSession*>(this)->end(); iter++)
  {
    if (iter->name() == name)
    {
      erase(iter);
      return true;
    }
  }

  return false;
}

std::string SDPSession::getAddress() const
{
  WriteLock lock(_rwMutex);

  std::string addressData;
  SDPSession::iterator iter;
  for (iter=const_cast<SDPSession*>(this)->begin(); iter != const_cast<SDPSession*>(this)->end(); iter++)
  {
    if (iter->name() == 'c')
    {
      addressData = iter->value();
      break;
    }
  }


  if (!addressData.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(addressData, " ");
    if (tokens.size() == 3)
    {
      return tokens[2];
    }
  }

  return "";
}

void SDPSession::changeAddress(const std::string& address, const char* version )
{
  WriteLock lock(_rwMutex);

  SDPSession::iterator iter;
  for (iter=begin(); iter != end(); iter++)
  {
    if (iter->name() == 'c')
    {
      std::ostringstream newVal;
      newVal << "IN " << version << " " << address;
      iter->value() = newVal.str();
    }
  }
}

bool SDPSession::getOrigin(
  std::string& userName,
  std::string& sessionId,
  std::string& sessionVersion,
  std::string& netType,
  std::string& addressType,
  std::string& unicastAddress
) const
{
  std::string oLine = findHeader('o');
  if (oLine.empty())
  {
    return false;
  }
  std::vector<std::string> oLineTokens = OSS::string_tokenize(oLine, " ");
  if (oLineTokens.size() != 6)
  {
    return false;
  }

  userName = oLineTokens[0];
  sessionId = oLineTokens[1];
  sessionVersion = oLineTokens[2];
  netType = oLineTokens[3];
  addressType = oLineTokens[4];
  unicastAddress = oLineTokens[5];
  
  return true;
}

} } // OSS::SDP


