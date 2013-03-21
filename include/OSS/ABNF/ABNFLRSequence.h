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


#ifndef ABNF_SIPLRSEQUENCE_INCLUDE
#define ABNF_SIPLRSEQUENCE_INCLUDE


#include "OSS/ABNF/ABNFBaseRule.h"


namespace OSS {
namespace ABNF {


template <typename Rule_T>
class ABNFLROptional : public ABNFBaseRule
/// This a helper class that marks a parser
/// as optional based on typid checking
{
public:
  ABNFLROptional()
  {
    _optional = true;
  }
  /// Creates a new ABNFLROptional object

  char* parse(const char* _t)
    /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    if (*t == '\0')
      return t;
    char* iter = _rule.parse( t );
    bool is_set = iter != t;
    return is_set ? iter : t;
  }
private:
  Rule_T _rule;
};

class ABNFLRNullRule : public ABNFBaseRule
{
public:
  char * parse(const char*p){ return const_cast<char*>(p); }
};
/// Null rule marker needed by ABNFLRSequence

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
class ABNFLRSequence16 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence16(){};
  /// Creates a new ABNFLRSequence object.

 char* parse(const char* _t)
 /// Returns the next off-set if the rule is satisfied
 {
  char* _iter = const_cast<char*>(_t);

  char* startIter = _iter;
  char* iter = _iter;
  char* newIter;

  for (size_t i = 0; i < 16; i++)
  {
    switch(i)
    {
    case 0:
      if (typeid(_rule_0) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_0.parse(iter);
      if (newIter == iter && !_rule_0.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 1:
      if (typeid(_rule_1) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_1.parse(iter);
      if (newIter == iter && !_rule_1.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 2:
      if (typeid(_rule_2) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_2.parse(iter);
      if (newIter == iter && !_rule_2.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 3:
      if (typeid(_rule_3) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_3.parse(iter);
      if (newIter == iter && !_rule_3.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 4:
      if ( typeid(_rule_4) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_4.parse(iter);
      if (newIter == iter && !_rule_4.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 5:
      if ( typeid(_rule_5) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_5.parse(iter);
      if (newIter == iter && !_rule_5.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 6:
      if (typeid(_rule_6) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_6.parse(iter);
      if ( newIter == iter && !_rule_6.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 7:
      if (typeid(_rule_7) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_7.parse(iter);
      if (newIter == iter && !_rule_7.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 8:
      if (typeid(_rule_8) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_8.parse(iter);
      if (newIter == iter && !_rule_8.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 9:
      if (typeid(_rule_9) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_9.parse(iter);
      if (newIter == iter && !_rule_9.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 10:
      if (typeid( _rule_10) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_10.parse(iter);
      if (newIter == iter && !_rule_10.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 11:
      if (typeid(_rule_11) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_11.parse( iter );
      if (newIter == iter && !_rule_11.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 12:
      if (typeid(_rule_12) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_12.parse(iter);
      if (newIter == iter && !_rule_12.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 13:
      if (typeid(_rule_13) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_13.parse(iter);
      if (newIter == iter && !_rule_13.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 14:
      if (typeid(_rule_14) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_14.parse(iter);
      if (newIter == iter && !_rule_14.isOptional())
        return startIter;
      iter = newIter;
      break;
    case 15:
      if (typeid(_rule_15) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_15.parse( iter );
      if (newIter == iter && !_rule_15.isOptional())
        return startIter;
      iter = newIter;
      break;
    }
  }
  return startIter;
}

char* parseTokens(const char* _t, ABNFTokens& tokens)
 /// Fills tokens vector with parsed string.
 /// Returns the next off-set if the rule is satisfied
 {
  char* _iter = const_cast<char*>(_t);

  char* startIter = _iter;
  char* iter = _iter;
  char* newIter;

  for (size_t i = 0; i < 16; i++)
  {
    switch(i)
    {
    case 0:
      if (typeid(_rule_0) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_0.parse(iter);
      if (newIter == iter && !_rule_0.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 1:
      if (typeid(_rule_1) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_1.parse(iter);
      if (newIter == iter && !_rule_1.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 2:
      if (typeid(_rule_2) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_2.parse(iter);
      if (newIter == iter && !_rule_2.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 3:
      if (typeid(_rule_3) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_3.parse(iter);
      if (newIter == iter && !_rule_3.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 4:
      if ( typeid(_rule_4) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_4.parse(iter);
      if (newIter == iter && !_rule_4.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 5:
      if ( typeid(_rule_5) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_5.parse(iter);
      if (newIter == iter && !_rule_5.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 6:
      if (typeid(_rule_6) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_6.parse(iter);
      if ( newIter == iter && !_rule_6.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 7:
      if (typeid(_rule_7) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_7.parse(iter);
      if (newIter == iter && !_rule_7.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 8:
      if (typeid(_rule_8) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_8.parse(iter);
      if (newIter == iter && !_rule_8.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 9:
      if (typeid(_rule_9) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_9.parse(iter);
      if (newIter == iter && !_rule_9.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 10:
      if (typeid( _rule_10) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_10.parse(iter);
      if (newIter == iter && !_rule_10.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 11:
      if (typeid(_rule_11) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_11.parse( iter );
      if (newIter == iter && !_rule_11.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 12:
      if (typeid(_rule_12) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_12.parse(iter);
      if (newIter == iter && !_rule_12.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 13:
      if (typeid(_rule_13) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_13.parse(iter);
      if (newIter == iter && !_rule_13.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 14:
      if (typeid(_rule_14) == typeid(ABNFLRNullRule))
        return iter;
   
      newIter = _rule_14.parse(iter);
      if (newIter == iter && !_rule_14.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    case 15:
      if (typeid(_rule_15) == typeid(ABNFLRNullRule))
        return iter;
      
      newIter = _rule_15.parse( iter );
      if (newIter == iter && !_rule_15.isOptional())
        return startIter;
      tokens.push_back(std::string(iter, newIter));
      iter = newIter;
      break;
    }
  }
  return startIter;
}

private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
  Rule_2 _rule_2;
  Rule_3 _rule_3;
  Rule_4 _rule_4;
  Rule_5 _rule_5;
  Rule_6 _rule_6;
  Rule_7 _rule_7;
  Rule_8 _rule_8;
  Rule_9 _rule_9;
  Rule_10 _rule_10;
  Rule_11 _rule_11;
  Rule_12 _rule_12;
  Rule_13 _rule_13;
  Rule_14 _rule_14;
  Rule_15 _rule_15;
};

template 
<
  typename Rule_0,
  typename Rule_1
>
class ABNFLRSequence2 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence2(){};
  /// Creates a new ABNFLRSequence object.

 char* parse(const char* _t)
 /// Returns the next off-set if the rule is satisfied
 {
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  t = nextIter;
 
  return t;
}

char* parseTokens(const char* _t, ABNFTokens& tokens)
/// Fills tokens vector with parsed string.
/// Returns the next off-set if the rule is satisfied
{
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;
 
  return t;
}

private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2
>
class ABNFLRSequence3 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence3(){};
  /// Creates a new ABNFLRSequence object.

 char* parse(const char* _t)
 /// Returns the next off-set if the rule is satisfied
 {
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_2.parse(t);
  if (nextIter == t && !_rule_2.isOptional())
    return startIter;
  t = nextIter;
  
  return t;
}

char* parseTokens(const char* _t, ABNFTokens& tokens)
/// Fills tokens vector with parsed string.
/// Returns the next off-set if the rule is satisfied
 {
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_2.parse(t);
  if (nextIter == t && !_rule_2.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;
  
  return t;
}

private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
  Rule_2 _rule_2;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2,
  typename Rule_3
>
class ABNFLRSequence4 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence4(){};
  /// Creates a new ABNFLRSequence object.

 char* parse(const char* _t)
 /// Returns the next off-set if the rule is satisfied
 {
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_2.parse(t);
  if (nextIter == t && !_rule_2.isOptional())
    return startIter;
  t = nextIter;

  nextIter = _rule_3.parse(t);
  if (nextIter == t && !_rule_3.isOptional())
    return startIter;
  t = nextIter;
  
  return t; 
 }

char* parseTokens(const char* _t, ABNFTokens& tokens)
/// Fills tokens vector with parsed string.
/// Returns the next off-set if the rule is satisfied
{
  char* t = const_cast<char*>(_t);
  char* startIter = t;

  char* nextIter = _rule_0.parse(t);
  if (nextIter == t && !_rule_0.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_1.parse(t);
  if (nextIter == t && !_rule_1.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_2.parse(t);
  if (nextIter == t && !_rule_2.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  nextIter = _rule_3.parse(t);
  if (nextIter == t && !_rule_3.isOptional())
    return startIter;
  tokens.push_back(std::string(t, nextIter));
  t = nextIter;

  return t; 
}

private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
  Rule_2 _rule_2;
  Rule_3 _rule_3;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2,
  typename Rule_3,
  typename Rule_4
>
class ABNFLRSequence5 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence5(){};
  /// Creates a new ABNFLRSequence object.

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    char* startIter = t;

    char* nextIter = _rule_0.parse(t);
    if (nextIter == t && !_rule_0.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_1.parse(t);
    if (nextIter == t && !_rule_1.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_2.parse(t);
    if (nextIter == t && !_rule_2.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_3.parse(t);
    if (nextIter == t && !_rule_3.isOptional())
      return startIter;
    t = nextIter; 

    nextIter = _rule_4.parse(t);
    if (nextIter == t && !_rule_4.isOptional())
      return startIter;
    t = nextIter; 

    return t; 
  }

  char* parseTokens(const char* _t, ABNFTokens& tokens)
  /// Fills tokens vector with parsed string.
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    char* startIter = t;

    char* nextIter = _rule_0.parse(t);
    if (nextIter == t && !_rule_0.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_1.parse(t);
    if (nextIter == t && !_rule_1.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_2.parse(t);
    if (nextIter == t && !_rule_2.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_3.parse(t);
    if (nextIter == t && !_rule_3.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter; 

    nextIter = _rule_4.parse(t);
    if (nextIter == t && !_rule_4.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter; 

    return t; 
  }
private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
  Rule_2 _rule_2;
  Rule_3 _rule_3;
  Rule_4 _rule_4;
};

template 
<
  typename Rule_0,
  typename Rule_1,
  typename Rule_2,
  typename Rule_3,
  typename Rule_4,
  typename Rule_5
>
class ABNFLRSequence6 : public ABNFBaseRule
  /// This is a helper class that allows a sequence of ABNF
  /// parser to be processed in a strict order.  Right now
  /// this tamplate only supports 16 parameters but nothing
  /// could hinder you to nest multiple Left To Right sequences
  /// within an outer LRSequence
{
public:
  ABNFLRSequence6(){};
  /// Creates a new ABNFLRSequence object.

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    char* startIter = t;

    char* nextIter = _rule_0.parse(t);
    if (nextIter == t && !_rule_0.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_1.parse(t);
    if (nextIter == t && !_rule_1.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_2.parse(t);
    if (nextIter == t && !_rule_2.isOptional())
      return startIter;
    t = nextIter;

    nextIter = _rule_3.parse(t);
    if (nextIter == t && !_rule_3.isOptional())
      return startIter;
    t = nextIter; 

    nextIter = _rule_4.parse(t);
    if (nextIter == t && !_rule_4.isOptional())
      return startIter;
    t = nextIter; 

    nextIter = _rule_5.parse(t);
    if (nextIter == t && !_rule_5.isOptional())
      return startIter;
    t = nextIter; 

    return t; 
  }

  char* parseTokens(const char* _t, ABNFTokens& tokens)
  /// Fills tokens vector with parsed string.
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    char* startIter = t;

    char* nextIter = _rule_0.parse(t);
    if (nextIter == t && !_rule_0.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_1.parse(t);
    if (nextIter == t && !_rule_1.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_2.parse(t);
    if (nextIter == t && !_rule_2.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_3.parse(t);
    if (nextIter == t && !_rule_3.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter; 

    nextIter = _rule_4.parse(t);
    if (nextIter == t && !_rule_4.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter;

    nextIter = _rule_5.parse(t);
    if (nextIter == t && !_rule_5.isOptional())
      return startIter;
    tokens.push_back(std::string(t, nextIter));
    t = nextIter; 

    return t; 
  }
private:
  Rule_0 _rule_0;
  Rule_1 _rule_1;
  Rule_2 _rule_2;
  Rule_3 _rule_3;
  Rule_4 _rule_4;
  Rule_5 _rule_5;
};
} } //OSS::ABNF

#endif  //ABNF_SIPLRSEQUENCE_INCLUDE
