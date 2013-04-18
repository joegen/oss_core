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


#include "OSS/SDP/SDPMedia.h"


namespace OSS {
namespace SDP {


SDPMedia::SDPMedia() :
  _dataPort(0),
  _controlPort(0),
  _address(),
  _payloads(),
  _type(TYPE_NONE),
  _ptime(0),
  _direction(MEDIA_UNSET)
{
  /*
  Media description, if present
         m=  (media name and transport address)
         i=* (media title)
         c=* (connection information -- optional if included at
              session level)
         b=* (zero or more bandwidth information lines)
         k=* (encryption key)
         a=* (zero or more media attribute lines)
  */
  _exitName = 'm';
  reset();
}

SDPMedia::SDPMedia(const char* rawHeader) :
  SDPHeaderList(rawHeader, 'm'),
  _dataPort(0),
  _controlPort(0),
  _address(),
  _payloads(),
  _type(TYPE_NONE),
  _ptime(0),
  _direction(MEDIA_UNSET)
{
  _exitName = 'm';
}

SDPMedia::SDPMedia(const SDPMedia& header) :
  SDPHeaderList(header)
{
  ReadLock lock(header._rwMutex);
  _exitName = 'm';
  _dataPort = header._dataPort;
  _controlPort = header._controlPort;
  _address = header._address;
  _payloads = header._payloads;
  _type = header._type;
  _ptime = header._ptime;
  _direction = header._direction;
}

SDPMedia::~SDPMedia()
{
}

SDPMedia& SDPMedia::operator = (const SDPMedia& media)
{
  SDPMedia swapable(media);
  swap(swapable);
  return *this;
}

void SDPMedia::swap(SDPMedia& media)
{
  WriteLock lock1(media._rwMutex);
  WriteLock lock2(_rwMutex);
  std::swap(_dataPort, media._dataPort);
  std::swap(_controlPort,media._controlPort);
  std::swap(_address, media._address);
  std::swap(_payloads, media._payloads);
  std::swap(_type, media._type);
  std::swap(_ptime, media._ptime);
  std::swap(_direction, media._direction);
  SDPHeaderList::swap(media);
}

void SDPMedia::reset()
{
  WriteLock lock(_rwMutex);
  clear();
  SDPHeader resetHeader;
  resetHeader.name() = 'm';
  resetHeader.value() = "none 0 RTP/AVP";
  push_back(resetHeader);
  _isValid = false;
  _tail = "";
  _dataPort = 0;
  _controlPort = 0;
  _address = "";
  _payloads.clear();
  _type = TYPE_NONE;
  _ptime = 0;
  _direction = MEDIA_UNSET;
}

SDPMedia::Type SDPMedia::getMediaType() const
{
  ReadLock lock(_rwMutex);
  if (_type != TYPE_NONE)
    return _type;

  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      if (OSS::string_caseless_starts_with(iter->value(), "audio"))
        const_cast<SDPMedia*>(this)->_type = TYPE_AUDIO;
      else if (OSS::string_caseless_starts_with(iter->value(), "video"))
        const_cast<SDPMedia*>(this)->_type = TYPE_VIDEO;
      else if (OSS::string_caseless_starts_with(iter->value(), "image"))
        const_cast<SDPMedia*>(this)->_type = TYPE_FAX;
      else if (OSS::string_caseless_starts_with(iter->value(), "data"))
        const_cast<SDPMedia*>(this)->_type = TYPE_DATA;
    }
  }
  return _type;
}

void SDPMedia::setMediaType(Type type)
{
  WriteLock lock(_rwMutex);
  
  if (type == _type)
    return;

  std::string mt;
  switch (type)
  {
  case TYPE_AUDIO:
    mt = "audio";
    break;
  case TYPE_VIDEO:
    mt = "video";
    break;
  case TYPE_FAX:
    mt = "image";
    break;
  case TYPE_DATA:
    mt = "data";
    break;
  default:
    OSS_VERIFY(false);
    break;
  }
  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      size_t index = iter->value().find(' ');
      if (index != std::string::npos && index < iter->value().size())
      {
        iter->value() = mt + OSS::string_right(iter->value(), iter->value().size() - index);
        _type = type;
      }
    }
  }
}

const SDPMedia::Payloads& SDPMedia::getPayloads() const
{
  WriteLock lock(_rwMutex);
  
  if (_payloads.size() > 0)
    return _payloads;

  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), " ");
      //audio 49230 RTP/AVP 0 8 96 97 98 101
      if (tokens.size() > 3)
      {
        for (std::size_t i = 3; i < tokens.size(); i++)
          const_cast<SDPMedia*>(this)->_payloads.push_back(OSS::string_to_number<int>(tokens[i].c_str()));
      }
    }
  }
  return _payloads;
}

void SDPMedia::setPayloads(const Payloads& payloads)
{
  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), " ");
      //audio 49230 RTP/AVP 0 8 96 97 98 101
      if (tokens.size() >= 3)
      {
        std::stringstream m;
        m << tokens[0] << " " << tokens[1] << " " << tokens[2] << " ";
        Payloads::const_iterator citer;
        std::size_t i = 0;
        for (citer = payloads.begin(); citer != payloads.end(); citer++)
        {
          m << *citer;
          if (i != payloads.size() - 1)
            m << " ";
          i++;
        }
        iter->value() = m.str();
        _payloads = payloads;
        _isValid = _payloads.size() > 0;
      }
    }
  }
}

bool SDPMedia::addPayload(int payload)
{
  OSS_VERIFY(payload <= 255);
  getPayloads();

  WriteLock lock(_rwMutex);
  if (_payloads.size() > 0)
  {
    //
    // Check if the payload already exists
    //
    Payloads::const_iterator iter;
    for (iter = _payloads.begin(); iter != _payloads.end(); iter++)
      if (*iter == payload)
        return false;
  }
  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      _payloads.push_back(payload);
      iter->value() += " ";
      iter->value() += OSS::string_from_number<int>(payload);
    }
  }

  _isValid = _payloads.size() > 0;

  return true;
}

void SDPMedia::removePayload(int payload)
{
  WriteLock lock(_rwMutex);
  if (_payloads.size() == 0)
    return;
 

  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), " ");
      if (tokens.size() >= 3)
      {
        _payloads.remove(payload);
        std::stringstream m;
        m << tokens[0] << " " << tokens[1] << " " << tokens[2] << " ";
        Payloads::const_iterator citer;
        std::size_t i = 0;
        for (citer = _payloads.begin(); citer != _payloads.end(); citer++)
        {
          m << *citer;
          if (i != _payloads.size() - 1)
            m << " ";
          i++;
        }
        iter->value() = m.str();
      }
    }
  }

  _isValid = _payloads.size() > 0;
}

bool SDPMedia::hasPayload(int payload) const
{
  
  bool retrievePayloads = false;
  {
    ReadLock lock(_rwMutex);
    retrievePayloads = _payloads.size() == 0;
  }

  if (retrievePayloads)
    getPayloads();

  ReadLock lock(_rwMutex);
  Payloads::const_iterator iter;
  for (iter = _payloads.begin(); iter != _payloads.end(); iter++)
    if (*iter == payload)
      return true;
  return false;
}

const std::string& SDPMedia::getAddress() const
{
  WriteLock lock(_rwMutex);
  if (!_address.empty())
    return _address;
  
  SDPMedia::iterator iter;
  std::string addressData;
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
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
      const_cast<SDPMedia*>(this)->_address = tokens[2];
    }
  }

  return _address;
}

void SDPMedia::internalSetAddress(const std::string& address, bool isV4)
{
  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      SDPHeader c;
      c.value().reserve(30);
      c.name() = 'c';
      if (isV4)
        c.value() = "IN IP4 ";
      else
        c.value() = "IN IP6 ";
      c.value() += address;
      _address = address;
      if (size() == 1)
      { 
        push_back(c);
      }else if(size() > 1)
      {
        iter++;
        if (iter != end())
        {
          if (iter->name() == 'c')
          {
            iter->value() = c.value();
          }else if(iter->name() == 'i')
          {
            iter++;
            if (iter != end())
              insert(iter, c);
            else
              push_back(c);
          }else
          {
            insert(iter, c);
          }
        }
      }
    }
  }
}



unsigned short SDPMedia::getDataPort() const
{
  WriteLock lock(_rwMutex);
  if (_dataPort != 0)
    return _dataPort;

  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  if (iter != end())
  {
    if (iter->name() == 'm')
    {
      std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), " ");
      if (tokens.size() > 2)
      {
        if (tokens[1].find("/") == std::string::npos)
        {
          const_cast<SDPMedia*>(this)->_dataPort = OSS::string_to_number<unsigned short>(tokens[1].c_str());
        }
        else
        {
          std::vector<std::string> subTokens = OSS::string_tokenize(tokens[1], "/");
          if (subTokens.size() == 2)
            const_cast<SDPMedia*>(this)->_dataPort = OSS::string_to_number<unsigned short>(subTokens[0].c_str());
        }
      }
    }
  }
  return _dataPort;
}

void SDPMedia::setDataPort(unsigned short port)
{
  OSS_VERIFY(port != 0);

  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->begin();
  
  OSS_VERIFY(iter != end());
  OSS_VERIFY(iter->name() == 'm');

  std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), " ");
  if (tokens.size() > 2)
  {
    tokens[1] = OSS::string_from_number<unsigned short>(port);
    std::stringstream newMLine;
    for (std::size_t i = 0; i < tokens.size() ;i++)
    {
      newMLine << tokens[i];
      if (i != tokens.size() -1)
        newMLine << " ";
    }
    iter->value() = newMLine.str();
    _dataPort = port;
  }
}

SDPMedia::iterator SDPMedia::findAttributeIterator(int payload, const char* attributeName)
{
  OSS_VERIFY(payload <= 255);
  OSS_VERIFY(attributeName != 0);
  std::stringstream key;
  key << attributeName << ":" << payload;
  SDPMedia::iterator iter;
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && OSS::string_caseless_starts_with(iter->value(), key.str().c_str()))
      return iter;
  }
  return end();
}

void SDPMedia::setAttribute(int payload, const char* attribute, const std::string& value)
{
  OSS_VERIFY(payload <= 255);
  OSS_VERIFY(!value.empty());
  OSS_VERIFY_NULL(attribute);

  WriteLock lock(_rwMutex);

  SDPMedia::iterator iter = findAttributeIterator(payload, attribute);
  std::stringstream newVal;
  newVal << attribute << ":" << payload << " " << value;
  if ( iter != end())
  {
    iter->value() = newVal.str();
  }
  else
  {
    SDPHeader a;
    a.name() = 'a';
    a.value() = newVal.str();
    push_back(a);
  }
}

std::string SDPMedia::getAttribute(int payload, const char* attribute) const
{
  OSS_VERIFY(payload <= 255);
  OSS_VERIFY_NULL(attribute);

  ReadLock lock(_rwMutex);

  SDPMedia::iterator iter = const_cast<SDPMedia*>(this)->findAttributeIterator(payload, attribute);
  if (iter != end())
  {
    size_t pos = iter->value().find(' ');
    if (pos != std::string::npos && pos < iter->value().size())
    {
      return std::string(iter->value().c_str() + pos + 1);
    }
  }
  return "";
}

void SDPMedia::removeAttribute(int payload, const char* attribute)
{
  OSS_VERIFY(payload <= 255);
  OSS_VERIFY_NULL(attribute);
  WriteLock lock(_rwMutex);

  SDPMedia::iterator iter = findAttributeIterator(payload, attribute);
  if (iter != end())
    erase(iter);
}

std::string SDPMedia::getCommonAttribute(const char* attribute) const
{
  OSS_VERIFY_NULL(attribute);
  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter;

  std::stringstream key;
  key << attribute << ":";
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && OSS::string_caseless_starts_with(iter->value(), key.str().c_str()))
    {
      size_t pos = iter->value().find(' ');
      if (pos != std::string::npos && pos < iter->value().size())
      {
        return std::string(iter->value().c_str() + pos + 1);
      }
      else
      {
        std::vector<std::string> tokens = OSS::string_tokenize(iter->value(), ":");
        if (tokens.size() >= 2)
        {
          std::stringstream strm;
          for (std::size_t i = 1; i < tokens.size(); i++)
          {
            strm << tokens[i];
            if (i < tokens.size() - 1)
              strm << ":";
          }
          return strm.str();
        }
      }
    }
  }
  return "";
}

void SDPMedia::setCommonAttribute(const char* attribute, const std::string& value)
{
  OSS_VERIFY_NULL(attribute);
  OSS_VERIFY(!value.empty());

  WriteLock lock(_rwMutex);

  SDPMedia::iterator iter;
  std::stringstream key;
  key << attribute << ':';
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && OSS::string_caseless_starts_with(iter->value(), key.str().c_str()) )
    {
      key << value;
      iter->value() = key.str();
      return;
    }
  }
  key << value;
  SDPHeader a;
  a.name() = 'a';
  a.value() = key.str();
  push_back(a);
}

void SDPMedia::removeCommonAttribute(const char* attribute)
{
  WriteLock lock(_rwMutex);
  OSS_VERIFY_NULL(attribute);

  SDPMedia::iterator iter;
  std::stringstream key;
  key << attribute << ':';
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && OSS::string_caseless_starts_with(iter->value(), key.str().c_str()))
    {
      this->erase(iter);
      return;
    }
  }
}

unsigned short SDPMedia::getControlPort() const
{
  {
  ReadLock lock(_rwMutex);
  if (_controlPort != 0)
    return _controlPort;
  }
  
  std::string port = this->getCommonAttribute("rtcp");
  if (!port.empty())
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_controlPort = OSS::string_to_number<unsigned short>(port.c_str());
    return _controlPort;
  }

  if (_dataPort == 0)
    getDataPort();
  
  if (_dataPort != 0)
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_controlPort= _dataPort + 1;
  }

  return _controlPort;
}

void SDPMedia::setControlPort(unsigned short port)
{
  OSS_VERIFY(port != 0);
  setCommonAttribute("rtcp", OSS::string_from_number<unsigned short>(port));
  WriteLock lock(_rwMutex);
  _controlPort = port;
}

void SDPMedia::removeControlPort()
{
  removeCommonAttribute("rtcp");
}

unsigned short SDPMedia::getPtime() const
{
  {
    ReadLock lock(_rwMutex);
    if (_ptime != 0)
      return _ptime;
  }
  
  std::string value = this->getCommonAttribute("ptime");
  if (!value.empty())
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_ptime = OSS::string_to_number<unsigned short>(value.c_str());
    return _ptime;
  }

  return _ptime;
}

void SDPMedia::setPtime(unsigned short ptime)
{
  OSS_VERIFY(ptime != 0);
  setCommonAttribute("ptime", OSS::string_from_number<unsigned short>(ptime));
  WriteLock lock(_rwMutex);
  _ptime = ptime;
}

void SDPMedia::removePtime()
{
  removeCommonAttribute("ptime");
}

void SDPMedia::setFlagAttribute(const char* flag)
{
  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter;
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && iter->value() == flag)
      return;
  }
  SDPHeader a;
  a.name() = 'a';
  a.value() = flag;
  push_back(a);
}

bool SDPMedia::hasFlagAttribute(const char* flag) const
{
  ReadLock lock(_rwMutex);
  SDPMedia::iterator iter;
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && iter->value() == flag)
      return true;
  }
  return false;
}

void SDPMedia::removeFlagAttribute(const char* flag)
{
  WriteLock lock(_rwMutex);
  SDPMedia::iterator iter;
  for (iter = const_cast<SDPMedia*>(this)->begin(); iter != const_cast<SDPMedia*>(this)->end(); iter++)
  {
    if (iter->name() == 'a' && iter->value() == flag)
    {
      erase(iter);
      return;
    }
  }
}

SDPMedia::Direction SDPMedia::getDirection() const
{
  {
    ReadLock lock(_rwMutex);
    if (_direction != MEDIA_UNSET)
      return _direction;
  }

  if (hasFlagAttribute("sendrecv"))
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_direction = MEDIA_SEND_AND_RECEIVE;
  }
  else if (hasFlagAttribute("sendonly"))
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_direction = MEDIA_SEND_ONLY;
  }
  else if (hasFlagAttribute("recvonly"))
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_direction = MEDIA_RECEIVE_ONLY;
  }
  else if (hasFlagAttribute("inactive"))
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_direction = MEDIA_INACTIVE;
  }
  else
  {
    WriteLock lock(_rwMutex);
    const_cast<SDPMedia*>(this)->_direction = MEDIA_SEND_AND_RECEIVE;
  }
  return _direction;
}

void SDPMedia::setDirection(Direction direction)
{
  {
    ReadLock lock(_rwMutex);
    if (_direction == direction)
      return;
  }

  getDirection();

  if (_direction == MEDIA_SEND_AND_RECEIVE)
    removeFlagAttribute("sendrecv");
  else if (_direction == MEDIA_SEND_ONLY)
    removeFlagAttribute("sendonly");
  else if (_direction == MEDIA_RECEIVE_ONLY)
    removeFlagAttribute("recvonly");
  else if (_direction == MEDIA_INACTIVE)
    removeFlagAttribute("inactive");

  if (direction == MEDIA_SEND_AND_RECEIVE)
  {
    setFlagAttribute("sendrecv");
    WriteLock lock(_rwMutex);
    _direction = MEDIA_SEND_AND_RECEIVE;
  }
  else if (direction == MEDIA_SEND_ONLY)
  {
    setFlagAttribute("sendonly");
    WriteLock lock(_rwMutex);
    _direction = MEDIA_SEND_ONLY;
  }
  else if (direction == MEDIA_RECEIVE_ONLY)
  {
    setFlagAttribute("recvonly");
    WriteLock lock(_rwMutex);
    _direction = MEDIA_RECEIVE_ONLY;
  }
  else if (direction == MEDIA_INACTIVE)
  {
    setFlagAttribute("inactive");
    WriteLock lock(_rwMutex);
    _direction = MEDIA_INACTIVE;
  }
}

} } // OSS::SIP::SDP


