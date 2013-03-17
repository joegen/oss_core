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


#include "OSS/ABNF/ABNFParser.h"


namespace OSS {

namespace ABNF {

//
// ABNFOctet
//
ABNFOctet::ABNFOctet()
{
}

char* ABNFOctet::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if (*t >= 0x00 && *t <= 0xFF)
    t++;
  return t;
}
//
// ABNFChar
//
ABNFChar::ABNFChar()
{
}

char* ABNFChar::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if (*t >= 0x01 && *t <= 0x7F)
    t++;
  return t;
}
//
// ABNFCharPrintable
//
ABNFCharPrintable::ABNFCharPrintable()
{
}

char* ABNFCharPrintable::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if( *t >= 0x21 && *t <= 0x7E )
    t++;
  return t;
}
//
// ABNFCharAlpha
//
ABNFCharAlpha::ABNFCharAlpha()
{
}

char* ABNFCharAlpha::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if ((*t >= 0x41 && *t <= 0x5A) || (*t >= 0x61 && *t <= 0x7A))
    t++;
  return t;
} 
//
// ABNFCharDigit
//
ABNFCharDigit::ABNFCharDigit()
{
}
  
char* ABNFCharDigit::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if (*t >= 0x30 && *t <= 0x39)
    t++;
  return t;
} 
//
// ABNFCharControl
//
ABNFCharControl::ABNFCharControl()
{
}

char* ABNFCharControl::parse(const char* _t)
{
  char * t = const_cast<char*>(_t);
  if ((*t >= 0x00 && *t <= 0x1F) || *t == 0x7F)
    t++;
  return t;
} 
//
// ABNFCharHEX
//
ABNFCharHex::ABNFCharHex()
{
}

char* ABNFCharHex::parse(const char* _t)
{
  char* t = const_cast<char*>(_t);
  if( (*t >= 0x30 && *t <= 0x39) || (*t >= 0x41 && *t <= 0x46 ) || (*t >= 0x61 && *t <= 0x66) )
    t++;
  return t;
} 

char* findNextIterFromString(const std::string& needle, const char* hayStack)
{
  char* startT = const_cast<char*>(hayStack);
  char* t = startT;
  for (std::size_t i = 0; i < needle.size(); i++)
  {
    char c = *t;
    if (c == 0x00)
      return startT;

    char c_ = c <= 90 ? c + 32 : c - 32;
    
    if (c == needle[i] || c_ == needle[i])
    {
      if (needle[i+1] != 0x00)
      {
        t++;
        continue;
      }
      else
      {
        t++;
        return t;
      }
    }else
    {
      /// did not match, reset the counter
      i = -1;
      t++;
      continue;
    }
  }

  return startT;
}

OSS_API char* findLastIterFromString(const std::string& needle, const char* hayStack)
{
  int strLen = strlen(hayStack);
  char* t = const_cast<char*>(hayStack) + strLen;
  char* startT = const_cast<char*>(hayStack);
  for (int i = needle.size() - 1; i >= 0; i--)
  {

    if (t == hayStack)
      return startT;

    char c = *t;

    if (c == needle[i])
    {
      if (i-1 >= 0)
      {
        t--;
        continue;
      }
      else
      {
        return t + needle.size();
      }
    }else
    {
      /// did not match, reset the counter
      i = needle.size();
      t--;
      continue;
    }
  }
  return startT;
}

} } // OSS::ABNF


