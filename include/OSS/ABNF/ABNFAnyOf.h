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


#ifndef ABNF_SIPANYOF_INCLUDED
#define ABNF_SIPANYOF_INCLUDED


#include "OSS/ABNF/ABNFBaseRule.h"


namespace OSS {
namespace ABNF {


template <typename Rule_0, typename Rule_1 >
class ABNFAnyOf : public ABNFBaseRule
  /// This is a helper class that satisfies an OR Rule
{
public:
  ABNFAnyOf(){};
  /// Creates a new ABNFAnyOf object

  char * parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* startIter = const_cast<char*>(_t);

    if (*startIter == '\0')
      return startIter;

    char* newIter = _rule_0.parse(startIter);
    if (newIter != startIter)
      return newIter;
    else
    {
      newIter = _rule_1.parse(startIter);
      if (newIter != startIter)
        return newIter;
    }
    return startIter;
  }
  
private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
};

template 
<
  typename Rule_0 = ABNFLRNullRule,
  typename Rule_1 = ABNFLRNullRule,
  typename Rule_2 = ABNFLRNullRule,
  typename Rule_3 = ABNFLRNullRule,
  typename Rule_4 = ABNFLRNullRule,
  typename Rule_5 = ABNFLRNullRule,
  typename Rule_6 = ABNFLRNullRule,
  typename Rule_7 = ABNFLRNullRule,
  typename Rule_8 = ABNFLRNullRule,
  typename Rule_9 = ABNFLRNullRule,
  typename Rule_10 = ABNFLRNullRule,
  typename Rule_11 = ABNFLRNullRule,
  typename Rule_12 = ABNFLRNullRule,
  typename Rule_13 = ABNFLRNullRule,
  typename Rule_14 = ABNFLRNullRule,
  typename Rule_15 = ABNFLRNullRule
>
class ABNFAnyOfMultiple16 : public ABNFBaseRule
  /// This is a helper class that satisfies up to 16 OR Rules
{
public:
  ABNFAnyOfMultiple16(){};
  /// Create a new ABNFAnyOfMultiple LR Rule

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char *_iter = const_cast<char*>(_t);
    char* startIter = _iter;
    char* iter = _iter;
    char* newIter;
    for(size_t i = 0; i < 16; i++)
    {
      switch(i)
      {
      case 0:
        if (typeid(_rule0) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule0.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 1:
        if (typeid(_rule1) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule1.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 2:
        if (typeid(_rule2) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule2.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 3:
        if (typeid(_rule3) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule3.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 4:
        if (typeid(_rule4) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule4.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 5:
        if (typeid(_rule5) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule5.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 6:
        if (typeid(_rule6) == typeid(ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule6.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 7:
        if ( typeid( _rule7 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule7.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 8:
        if ( typeid( _rule8 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule8.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 9:
        if ( typeid( _rule9 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule9.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 10:
        if ( typeid( _rule10 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule10.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 11:
        if ( typeid( _rule11 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule11.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 12:
        if ( typeid( _rule12 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule12.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 13:
        if ( typeid( _rule13 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule13.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 14:
        if ( typeid( _rule14 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule14.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      case 15:
        if ( typeid( _rule15 ) == typeid( ABNFLRNullRule ) )
          return iter;
        
        newIter = _rule15.parse( iter );
        if ( newIter != iter )
          return newIter;
        break;
      }
    }
    return startIter;
  }


  Rule_0 _rule0;
  Rule_1 _rule1;
  Rule_2 _rule2;
  Rule_3 _rule3;
  Rule_4 _rule4;
  Rule_5 _rule5;
  Rule_6 _rule6;
  Rule_7 _rule7;
  Rule_8 _rule8;
  Rule_9 _rule9;
  Rule_10 _rule10;
  Rule_11 _rule11;
  Rule_12 _rule12;
  Rule_13 _rule13;
  Rule_14 _rule14;
  Rule_15 _rule15;
};


template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2
>
class ABNFAnyOfMultiple3 : public ABNFBaseRule
  /// This is a helper class that satisfies up to 16 OR Rules
{
public:
  ABNFAnyOfMultiple3(){};
  /// Create a new ABNFAnyOfMultiple LR Rule

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char *_iter = const_cast<char*>(_t);
    char* startIter = _iter;
    char* iter = _iter;
    char* newIter;
    for(size_t i = 0; i < 3; i++)
    {
      switch(i)
      {
      case 0:
        if (typeid(_rule0) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule0.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 1:
        if (typeid(_rule1) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule1.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 2:
        if (typeid(_rule2) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule2.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      }
    }
    return startIter;
  }


  Rule_0 _rule0;
  Rule_1 _rule1;
  Rule_2 _rule2;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2,
  typename Rule_3
>
class ABNFAnyOfMultiple4 : public ABNFBaseRule
  /// This is a helper class that satisfies up to 16 OR Rules
{
public:
  ABNFAnyOfMultiple4(){};
  /// Create a new ABNFAnyOfMultiple LR Rule

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char *_iter = const_cast<char*>(_t);
    char* startIter = _iter;
    char* iter = _iter;
    char* newIter;
    for(size_t i = 0; i < 4; i++)
    {
      switch(i)
      {
      case 0:
        if (typeid(_rule0) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule0.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 1:
        if (typeid(_rule1) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule1.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 2:
        if (typeid(_rule2) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule2.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 3:
        if (typeid(_rule3) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule3.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      }
    }
    return startIter;
  }


  Rule_0 _rule0;
  Rule_1 _rule1;
  Rule_2 _rule2;
  Rule_3 _rule3;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2,
  typename Rule_3,
  typename Rule_4
>
class ABNFAnyOfMultiple5 : public ABNFBaseRule
  /// This is a helper class that satisfies up to 16 OR Rules
{
public:
  ABNFAnyOfMultiple5(){};
  /// Create a new ABNFAnyOfMultiple LR Rule

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char *_iter = const_cast<char*>(_t);
    char* startIter = _iter;
    char* iter = _iter;
    char* newIter;
    for(size_t i = 0; i < 5; i++)
    {
      switch(i)
      {
      case 0:
        if (typeid(_rule0) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule0.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 1:
        if (typeid(_rule1) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule1.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 2:
        if (typeid(_rule2) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule2.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 3:
        if (typeid(_rule3) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule3.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      case 4:
        if (typeid(_rule4) == typeid(ABNFLRNullRule))
          return iter;
        
        newIter = _rule4.parse(iter);
        if (newIter != iter)
          return newIter;
        break;
      }
    }
    return startIter;
  }


  Rule_0 _rule0;
  Rule_1 _rule1;
  Rule_2 _rule2;
  Rule_3 _rule3;
  Rule_4 _rule4;
};


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
class ABNFAnyOfChars : public ABNFBaseRule
/// This template class satifies an OR rule for a series of characters
{
public:
  ABNFAnyOfChars()
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
        return ++t;
    }

    return startT;
  }
  char _value[50];
};

} } //OSS::ABNF

#endif  //ABNF_SIPANYOF_INCLUDED

