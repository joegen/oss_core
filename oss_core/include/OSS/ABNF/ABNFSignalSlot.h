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

#ifndef ABNF_SIPSIGNALSLOT_INCLUDED
#define ABNF_SIPSIGNALSLOT_INCLUDED

#include "OSS/ABNF/ABNFBaseRule.h"

namespace OSS {

namespace ABNF {

template<typename Rule_T, typename Slot_T>
class OSS_API ABNFSignalSlot : public ABNFBaseRule
{
public:
  ABNFSignalSlot(Slot_T* slot) : _slot(slot)
  {
  }

  char* parse(const char* _t)
  {
    char* t = const_cast<char*>(_t);
    Rule_T rule;
    char * offSet = rule.parse(t);
    if (offSet > t)
    {
      if (rule.getRuleId() != ABNF_RULE_ID_NOT_SET)
      {
        std::string token;
        std::string(t, offSet);
        _slot->onABNFTokenParsed(rule.getRuleId(), token);
      }
    }
    return offSet;
  }
private:
  Slot_T * _slot;
};

template<
typename Rule_T, 
int ruleIDVal
>
class ABNFIdentifiable : public ABNFBaseRule
  /// This is a helper class to allow late assignment of the ruleId member var
{
public:
  ABNFIdentifiable()
  /// Creates a new ABNFIdentifiable Rule
  {
  }

  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    _rule.setRuleId(ruleIDVal);
    return _rule.parse(_t);
  }

  Rule_T _rule;
};

} } //OSS::ABNF
#endif

