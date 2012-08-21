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


#ifndef ABNF_SIPSTRINGLITERAL_INCLUDE
#define ABNF_SIPSTRINGLITERAL_INCLUDE


#include "OSS/ABNF/ABNFBaseRule.h"


namespace OSS {

namespace ABNF {


template
<
  char c0 = 0x00,char c1 = 0x00,char c2 = 0x00,char c3 = 0x00,char c4 = 0x00,
  char c5 = 0x00,char c6 = 0x00,char c7 = 0x00,char c8 = 0x00,char c9 = 0x00,
  char c10 = 0x00,char c11 = 0x00,char c12 = 0x00,char c13 = 0x00,char c14 = 0x00,
  char c15 = 0x00,char c16 = 0x00,char c17 = 0x00,char c18 = 0x00,char c19 = 0x00,
  char c20 = 0x00,char c21 = 0x00,char c22 = 0x00,char c23 = 0x00,char c24 = 0x00,
  char c25 = 0x00,char c26 = 0x00,char c27 = 0x00,char c28 = 0x00,char c29 = 0x00,
  char c30 = 0x00,char c31 = 0x00,char c32 = 0x00,char c33 = 0x00,char c34 = 0x00,
  char c35 = 0x00,char c36 = 0x00,char c37 = 0x00,char c38 = 0x00,char c39 = 0x00,
  char c40 = 0x00,char c41 = 0x00,char c42 = 0x00,char c43 = 0x00,char c44 = 0x00,
  char c45 = 0x00,char c46 = 0x00,char c47 = 0x00,char c48 = 0x00,char c49 = 0x00  
>
class ABNFStringLiteral : public ABNFBaseRule
  /// A representation of a literal case-insensitive string
{
public:
  ABNFStringLiteral()
    /// Creates a new case-insensitive string literal rule
  {
    _value[0]  = c0;_value[1]   = c1;_value[2]   = c2;_value[3]   = c3;_value[4] = c4;         
    _value[5]  = c5;_value[6]   = c6;_value[7]   = c7;_value[8]   = c8;_value[9] = c9;
    _value[10] = c10;_value[11] = c11;_value[12] = c12;_value[13] = c13;_value[14] = c14;
    _value[15] = c15;_value[16] = c16;_value[17] = c17;_value[18] = c18;_value[19] = c19;
    _value[20] = c20;_value[21] = c21;_value[22] = c22;_value[23] = c23;_value[24] = c24;
    _value[25] = c25;_value[26] = c26;_value[27] = c27;_value[28] = c28;_value[29] = c29;
    _value[30] = c30;_value[31] = c31;_value[32] = c32;_value[33] = c33;_value[34] = c34;
    _value[35] = c35;_value[36] = c36;_value[37] = c37;_value[38] = c38;_value[39] = c39;
    _value[40] = c40;_value[41] = c41;_value[42] = c42;_value[43] = c43;_value[44] = c44;
    _value[45] = c45;_value[46] = c46;_value[47] = c47;_value[48] = c48;_value[49] = c49;
  }

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* startT = const_cast<char*>(_t);
    char* t = startT;
    for (size_t i = 0; i < 50; i++)
    {
      char c = *t;
      if (c == 0x00)
        return startT;

      if (c == _value[i])
        if (_value[i+1] != 0x00)
        {
          t++;
          continue;
        }
        else
        {
          t++;
          return t;
        }
      else if ((c == _value[i] - 0x20) || (c == _value[i] + 0x20))
        if ( _value[i+1] != 0x00 )
        {
          t++;
          continue;
        }
        else
        {
          t++;
          return t;
        }
      else
        return startT;
    }

    return startT;
  }
private:

  char _value[50];
};

/** 
        */

template
  <
    char c0 = 0x00,char c1 = 0x00,char c2 = 0x00,char c3 = 0x00,char c4 = 0x00,
    char c5 = 0x00,char c6 = 0x00,char c7 = 0x00,char c8 = 0x00,char c9 = 0x00,
    char c10 = 0x00,char c11 = 0x00,char c12 = 0x00,char c13 = 0x00,char c14 = 0x00,
    char c15 = 0x00,char c16 = 0x00,char c17 = 0x00,char c18 = 0x00,char c19 = 0x00,
    char c20 = 0x00,char c21 = 0x00,char c22 = 0x00,char c23 = 0x00,char c24 = 0x00,
    char c25 = 0x00,char c26 = 0x00,char c27 = 0x00,char c28 = 0x00,char c29 = 0x00,
    char c30 = 0x00,char c31 = 0x00,char c32 = 0x00,char c33 = 0x00,char c34 = 0x00,
    char c35 = 0x00,char c36 = 0x00,char c37 = 0x00,char c38 = 0x00,char c39 = 0x00,
    char c40 = 0x00,char c41 = 0x00,char c42 = 0x00,char c43 = 0x00,char c44 = 0x00,
    char c45 = 0x00,char c46 = 0x00,char c47 = 0x00,char c48 = 0x00,char c49 = 0x00  
  >
class ABNFStrictStringLiteral : public ABNFBaseRule
///A representation of a literal [case-sensitive] string
{
public:
  ABNFStrictStringLiteral()
  {
    _value[0]  = c0;_value[1]   = c1;_value[2]   = c2;_value[3]   = c3;_value[4] = c4;         
    _value[5]  = c5;_value[6]   = c6;_value[7]   = c7;_value[8]   = c8;_value[9] = c9;
    _value[10] = c10;_value[11] = c11;_value[12] = c12;_value[13] = c13;_value[14] = c14;
    _value[15] = c15;_value[16] = c16;_value[17] = c17;_value[18] = c18;_value[19] = c19;
    _value[20] = c20;_value[21] = c21;_value[22] = c22;_value[23] = c23;_value[24] = c24;
    _value[25] = c25;_value[26] = c26;_value[27] = c27;_value[28] = c28;_value[29] = c29;
    _value[30] = c30;_value[31] = c31;_value[32] = c32;_value[33] = c33;_value[34] = c34;
    _value[35] = c35;_value[36] = c36;_value[37] = c37;_value[38] = c38;_value[39] = c39;
    _value[40] = c40;_value[41] = c41;_value[42] = c42;_value[43] = c43;_value[44] = c44;
    _value[45] = c45;_value[46] = c46;_value[47] = c47;_value[48] = c48;_value[49] = c49;
  }

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* startT = const_cast<char*>(_t);
    char* t = startT;
    for(size_t i = 0; i < 50; i++ )
    {
      char c = *t;
      if( c == 0x00 )
        return startT;

      if( c == _value[i] )
        if( _value[i+1] != 0x00 )
        {
          t++;
          continue;
        }else
        {
          t++;
          return t;
        }
      else
        return startT;
    }

    return startT;
  }
  char _value[50];
};

} } //OSS::ABNF
#endif //ABNF_SIPSTRINGLITERAL_INCLUDE
