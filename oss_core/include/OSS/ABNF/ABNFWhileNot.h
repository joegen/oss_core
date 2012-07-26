// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSABNF
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#ifndef ABNF_SIPWhileNot_INCLUDED
#define ABNF_SIPWhileNot_INCLUDED


#include "OSS/ABNF/ABNFBaseRule.h"


namespace OSS {

namespace ABNF {


template <typename T, bool consumeRule = false>
class ABNFWhileNot: public ABNFBaseRule
{
public:
  char* parse(const char* _t)
  {

    char* startIter = const_cast<char*>(_t);
    char* newIter = startIter;
    
    if (*startIter == '\0')
      return startIter;
    
    T p;
    bool cont = true;
    while (cont)
    {
      char* currentIter = p.parse(newIter);
      if (currentIter == newIter)
      {
        newIter++ ;
        if (*newIter == '\0' )
          return startIter;
      }
      else
      {
        if(consumeRule)
        {
          while (cont)
          {
            char* currentIter = p.parse(newIter);
            if (currentIter == newIter)
              break;
            else
              newIter = currentIter;
          }
        }
        return newIter;
      }
    }
    return startIter;
  }
};


} } // OSS::ABNF
#endif // ABNF_SIPWhileNot_INCLUDED

