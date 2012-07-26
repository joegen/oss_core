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


#ifndef ABNF_SIPRULEITERATION_INCLUDE
#define ABNF_SIPRULEITERATION_INCLUDE


#include "OSS/ABNF/ABNFBaseRule.h"


namespace OSS {
namespace ABNF {


template< 
  char c0 = 0x00,char c1 = 0x00,char c2 = 0x00,char c3 = 0x00,char c4 = 0x00,
  char c5 = 0x00,char c6 = 0x00,char c7 = 0x00,char c8 = 0x00,char c9 = 0x00
>
struct ABNFLoopExitChars
  /// This struct defines a function exit_rule() called within an iteration rule
  /// which evaluates the current parser off-set value if it marks a loop exit point.
  /// The default behavior is to exit the iteration loop when the nul (0x00) character
  /// is encountered.  Implementors are expected to come up with a parser specific 
  /// exit rules when using the rule iteration templates
{
  bool exit_rule(const char* t)
  /// Returns true if the iteration should exit based on the current pointer value
  {
    return *t==c0||*t==c1||*t==c2||*t==c3||*t==c4||*t==c5||*t==c6||*t==c7||*t==c8||*t==c9;
  }
};

typedef ABNFLoopExitChars<0x00> ABNFLoopExitIfNul;
/// A c type string exit rule

template<typename Rule_T, typename Exit_T, size_t MinSize_T, size_t MaxSize_T>
class ABNFLoopUntil : public ABNFBaseRule
{
public:
  ABNFLoopUntil()
  /// Create a new ABNFLoopUntil rule
  {
  }

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    if (MinSize_T == 0)
      _optional = true;

    char* t = const_cast<char*>(_t);

    if (*t == '\0')
      return t;

    char* startIter = t;
    Exit_T x_trait;
    size_t i = 0;
    for( i = 0; i < MaxSize_T; i++ )
    {
      if( x_trait.exit_rule( t ) )
        break;

      Rule_T rule_t;
      char * newT = rule_t.parse( t );
      if(newT == t)
        break;

      t = newT;
    }

    if (i < MinSize_T)
      return startIter;

    //if (strict && i <= MaxSize_T - 1)
    //  return startIter;

    return t;
  }

  char* parseTokens(const char* _t, ABNFTokens& tokens)
  /// Fills tokens vector with parsed string.
  /// Returns the next off-set if the rule is satisfied
  {
    if (MinSize_T == 0)
      _optional = true;

    char* t = const_cast<char*>(_t);
    char* startIter = t;
    Exit_T x_trait;
    size_t i = 0;
    for( i = 0; i < MaxSize_T; i++ )
    {
      if( x_trait.exit_rule( t ) )
        break;

      Rule_T rule_t;
      char * newT = rule_t.parse( t );
      if(newT == t)
        break;
      tokens.push_back(std::string(t, newT));
      t = newT;
    }

    if (i < MinSize_T)
      return startIter;

    return t;
  }
};

} } //OSS::ABNF
#endif //ABNF_SIPRULEITERATION_INCLUDE



