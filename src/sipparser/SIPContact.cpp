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


#include "OSS/SIP/SIPContact.h"
#include "OSS/ABNF/ABNFSIPContactParam.h"
#include "OSS/SIP/SIPMessage.h"


namespace OSS {
namespace SIP {


using namespace OSS::ABNF;
static ABNFWhileNot<ABNFSIPContactParam, true> contactParamFinder;
static ABNFWhileNot<ABNF_SIP_COMMA, true> commaFinder;

SIPContact::SIPContact()
{
}

SIPContact::SIPContact(const std::string& contact)
{
  _data = contact;
}

SIPContact::SIPContact(const SIPContact& contact)
{
  _data = contact._data;
}

SIPContact::SIPContact(const std::vector<std::string>& contacts)
{
  std::ostringstream strm;
  for (std::size_t i = 0; i < contacts.size(); i++)
  {
    strm << contacts[i];
    if (i < contacts.size() - 1)
    {
      strm << ", ";
    }
  }
  _data = strm.str();;
}

SIPContact::SIPContact(const ContactList& contacts)
{
  std::ostringstream strm;
  for (std::size_t i = 0; i < contacts.size(); i++)
  {
    strm << contacts[i].data();
    if (i < contacts.size() - 1)
    {
      strm << ", ";
    }
  }
  _data = strm.str();;
}


SIPContact::~SIPContact()
{
}

SIPContact& SIPContact::operator = (const std::string& contact)
{
  _data = contact;
  return *this;
}

SIPContact& SIPContact::operator = (const SIPContact& contact)
{
  SIPContact clonable(contact);
  swap(clonable);
  return *this;
}

SIPContact& SIPContact::operator = (const std::vector<std::string>& contacts)
{
  std::ostringstream strm;
  
  for (std::size_t i = 0; i < contacts.size(); i++)
  {
    strm << contacts[i];
    if (i < contacts.size() - 1)
      strm << ", ";
  }
  
  _data = strm.str();
  
  return *this;
}

SIPContact& SIPContact::operator = (const ContactList& contacts)
{
  std::ostringstream strm;
  
  for (std::size_t i = 0; i < contacts.size(); i++)
  {
    strm << contacts[i].data();
    if (i < contacts.size() - 1)
      strm << ", ";
  }
  
  _data = strm.str();
  
  return *this;
}

void SIPContact::swap(SIPContact& contact)
{
  std::swap(_data, contact._data);
}

bool SIPContact::getAt(ContactURI& uri, size_t index) const
{
  return getAt(_data, uri, index);
}

bool SIPContact::getAt(const std::string& contact, ContactURI& uri, size_t index)
{
  char* offSet = const_cast<char*>(contact.c_str());
  char* newOffSet = offSet;
  size_t i = 0;
  bool found = true;
  while(found)
  {
    newOffSet = contactParamFinder.parse(offSet);
    if (newOffSet == offSet)
      return false;

    char* commaOffSet = commaFinder.parse(newOffSet);
    if (commaOffSet == newOffSet)
    {
      //nomore to follow
      if (index == i)
        break;
      else
        return false;
    }
    else
    {
      if (index == i)
        break;
      i++;
      offSet = commaOffSet;
      continue;
    }
  }
  uri = std::string(offSet, newOffSet);
  return true;
}

bool SIPContact::setAt(const char* uri, size_t index)
{
  return setAt(_data, uri, index);
}

bool SIPContact::setAt(std::string& contact, const char* uri, size_t index)
{
  char* offSet = const_cast<char*>(contact.c_str());
  char* newOffSet = offSet;
  size_t i = 0;
  bool found = true;
  while(found)
  {
    newOffSet = contactParamFinder.parse(offSet);
    if (newOffSet == offSet)
      return false;

    char* commaOffSet = commaFinder.parse(newOffSet);
    if (commaOffSet == newOffSet)
    {
      //nomore to follow
      if (index == i)
        break;
      else
        return false;
    }
    else
    {
      if (index == i)
        break;
      i++;
      offSet = commaOffSet;
      continue;
    }
  }

  std::string front(contact.c_str(), (const char*)offSet);
  front += uri;
  front += newOffSet;
  contact = front;
  return true;
}

size_t SIPContact::getSize() const
{
  return getSize(_data);
}

size_t SIPContact::getSize(const std::string& contact)
{
  char* offSet = contactParamFinder.parse(contact.c_str());
  if (offSet == contact.c_str())
    return 0;
  bool moreElements = true;
  size_t count = 1;
  offSet = commaFinder.parse(offSet);
  while (moreElements)
  {
    char* newOffSet = contactParamFinder.parse(offSet);
    if (newOffSet == offSet)
      break;
    count++;
    offSet = commaFinder.parse(newOffSet);
    if (newOffSet == offSet)
      break;
  }
  return count;
}

bool SIPContact::popTopURI(ContactURI& topURI)
{
  return popTopURI(_data, topURI);
}

bool SIPContact::popTopURI(std::string& contact, ContactURI& topURI)
{
  char* offSet = const_cast<char*>(contact.c_str());
  
  char* newOffSet = offSet;

  newOffSet = contactParamFinder.parse(offSet);
  if (newOffSet == offSet)
    return false;

  if (*newOffSet == '\0')
  {
    topURI = contact;
    contact = "";
  }
  else
  {
    char* commaOffSet = commaFinder.parse(newOffSet);
    if (commaOffSet == newOffSet)
    {
      topURI = contact;
      contact = "";
    }
    else
    {
      topURI  = std::string(offSet, commaOffSet - 2);
      contact = commaOffSet;
    }
  }
  
  return true;
}

int SIPContact::msgGetContacts(SIPMessage* pMsg, std::vector<std::string>& contacts)
{
  ContactList contactList;
  msgGetContacts(pMsg, contactList);
  for (ContactList::iterator iter = contactList.begin(); iter != contactList.end(); iter++)
  {
    contacts.push_back(iter->data());
  }
  return contacts.size();
}

 int SIPContact::msgGetContacts(SIPMessage* pMsg, ContactList& contacts)
 {
    int headerCount = pMsg->hdrGetSize(OSS::SIP::HDR_CONTACT);
    for (int i = 0; i < headerCount; i++)
    {
      std::string contact = pMsg->hdrGet(OSS::SIP::HDR_CONTACT, i);
      int sz = getSize(contact);
      for (int j = 0; j < sz; j++)
      {
        ContactURI curi;
        if (getAt(contact,curi, i))
        {
          contacts.push_back(curi);
        }
      }
    }
    return contacts.size();
 }

} } // OSS::SIP



