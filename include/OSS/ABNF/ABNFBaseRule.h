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


#ifndef ABNF_SIPBASERULE_INCLUDE
#define ABNF_SIPBASERULE_INCLUDE


#include <typeinfo>
#include <vector>
#include <string>

#include "OSS/OSS.h"


namespace OSS {
namespace ABNF {


static const int ABNF_RULE_ID_NOT_SET = -1;

template<int ruleIDVal>
class ABNFBaseRuleIdentifiable
  /// Base class for all ABNF rules
{
public:
  ABNFBaseRuleIdentifiable() : 
      _ruleId(ruleIDVal),
      _optional(false)
  /// Creates a new ABNF Rule
  {
  }

  char* parse(const char* _t)
  /// Parse the nul-terminated string if it satisfies the rule.
  /// The correct implementation of the parse
  /// method is to return the next offSet where
  /// the next rule would begin parsing.  If the
  /// string did not satisfy the rule, the parse
  /// implementation should return the original 
  /// pointer back to the caller so that the next rule
  /// may be tested exactly at the same position. 
  /// Implementations may simply assign the bytes between
  /// the previous off-set and the current returned off-set
  /// to a string to preserve the parsed token.
  {
    return const_cast<char*>(_t);
  }

  int getRuleId() const{ return _ruleId; }
  void setRuleId(int id){ _ruleId = id; }
  bool isOptional(){ return _optional; };
protected:
  int _ruleId;
  bool _optional;
};

template<typename Rule_T>
struct ABNFEvaluate
{
  bool operator()(const char* _t)
  {
    Rule_T rule;
    char * offSet = rule.parse(_t);
    return *offSet == '\0';
  }
};

typedef ABNFBaseRuleIdentifiable<ABNF_RULE_ID_NOT_SET> ABNFBaseRule;

typedef std::vector<std::string> ABNFTokens;

OSS_API char*
#if OSS_OS_FAMILY_WINDOWS
__cdecl
#endif
findNextIterFromString(const std::string& _value, const char* _t);

OSS_API char*
#if OSS_OS_FAMILY_WINDOWS
__cdecl
#endif
findLastIterFromString(const std::string& _value, const char* _t);

template <typename T>
char* findNextIterFromRule(const char* _t)
{
  char* startIter = const_cast<char*>(_t);
  char* newIter = startIter;
  T p;
  bool cont = true;
  while(cont)
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
      while(cont)
      {
        char* currentIter = p.parse(newIter);
        if (currentIter == newIter)
          return newIter;
        else
          newIter = currentIter;
      }
    }
  }
  return startIter;
}


} } //OSS::ABNF
#endif //ABNF_SIPBASERULE_INCLUDE
