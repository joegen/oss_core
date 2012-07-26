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


#ifndef ABNF_SIPPARSER_INCLUDE
#define ABNF_SIPPARSER_INCLUDE


#include "OSS/ABNF/ABNFBaseRule.h"
#include "OSS/ABNF/ABNFLRSequence.h"
#include "OSS/ABNF/ABNFStringLiteral.h"
#include "OSS/ABNF/ABNFLoopUntil.h"
#include "OSS/ABNF/ABNFAnyOf.h"
#include "OSS/ABNF/ABNFFindMatch.h"
#include "OSS/ABNF/ABNFWhileNot.h"


namespace OSS {
namespace ABNF {


class OSS_API ABNFOctet : public ABNFBaseRule
  /// This rule satisfies an unsigned character
  /// 8 BIT Data (%x00-FF)
{
public:
  ABNFOctet();
  /// Creates a new ABNF Octet rule.

  char* parse(const char* _t);
  /// Returns the next off-set.  This rule would normally be satisfied all the time
};

class OSS_API ABNFChar : public ABNFBaseRule
  /// This rule satisfies any 7-bit US-ASCII character, excluding NUL (%x01-7F) 
{ 
public:
  ABNFChar();
  /// Creates a new ABNF 7-bit US-ASCII character rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

template <char literal>
class ABNFCharComparison : public ABNFBaseRule
  /// This is a template rule that satisfies
  /// whether the current character of the parser
  /// pointer is exactly equal to the value of 
  /// the 'literal' template argument
{
public:
  char* parse(const char* _t)
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    if (*t == literal)
      t++;
    return t;
  } 
};

class OSS_API ABNFCharPrintable  : public ABNFBaseRule
  /// This rule will satisfy visible (printable) characters (%x21-7E)
{
public:
  ABNFCharPrintable();
  /// Creates a new printable character rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

class OSS_API ABNFCharAlpha : public ABNFBaseRule
  /// This rule would satify character alphabets A-Z / a-z (%x41-5A / %x61-7A)
{
public:
  ABNFCharAlpha();
  /// Create a new character alpha rule.

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

class OSS_API ABNFCharDigit : public ABNFBaseRule
  /// This rule would satify character digits 0-9 (%x30-39)
{ 
public:
  ABNFCharDigit();
  /// Create a new character digit rule.
  
  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

/** Any US-ASCII control character: (octets 0 - 31) and DEL (127) ( %x00-1F / %x7F)
        */
class OSS_API ABNFCharControl : public ABNFBaseRule
  /// This rule would satisfy any US-ASCII control character: (octets 0 - 31) and DEL (127) ( %x00-1F / %x7F)
{ 
public:
  ABNFCharControl();
  /// Create a new control character rule
  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

class OSS_API ABNFCharHex : public ABNFBaseRule
  /// This rule with satisfy DIGIT  / "A" / "B" / "C" / "D" / "E" / "F"
  /// According to the 'char-val' rule, letters (A-F) are case insensitive 
{ 
public:
  ABNFCharHex();
  /// Create a new hex character rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

template </*unsigned*/ char minValue, /*unsigned*/ char maxValue>
class ABNFRange : public ABNFBaseRule
/// This teplate rule would satisfy any character between minValue and maxValue
{
public:
  ABNFRange()
  {
  }
  /// Create a new range rule

  char * parse( const char* _t )
  /// Returns the next off-set if the rule is satisfied
  {
    char* t = const_cast<char*>(_t);
    if( *t >= minValue || *t <= maxValue )
      return ++t;
    return t;
  }
};

typedef ABNFCharComparison<0x09> ABNFCharHTab;
/// This rule would satisfy a TAB Character

typedef ABNFCharComparison<0x0D> ABNFCharCR;
/// This rule would satisfy a Carriage Redurn (%x0D)

typedef ABNFCharComparison<0x0A> ABNFCharLF;
/// This rule would satisfy a Linefeed (%x0A)

typedef ABNFLRSequence2<ABNFCharCR, ABNFCharLF> ABNFCharCRLF;
/// This rule satisfies CRLF (%x0D0A)

typedef ABNFCharComparison<0x20> ABNFCharSpace;
/// This rule satisfied a space character (%x20)

typedef ABNFCharComparison<0x22> ABNFCharDQuote;
/// This rule satisfies a double Quote Character (%x22)

typedef ABNFAnyOf< ABNFCharComparison<0x30>, ABNFCharComparison<0x31> > ABNFCharBit;
/// This rule satisfies a BIT character 0 / 1

typedef ABNFAnyOf< ABNFCharCR, ABNFCharLF > ABNFCharNewLine;
/// This rule satisfies new line (CR / LF)

typedef ABNFAnyOf< ABNFCharSpace, ABNFCharHTab > ABNFCharWhiteSpace;
/// This rule satisfies whitespace or a horizontal tab

typedef ABNFAnyOf< ABNFCharNewLine, ABNFCharWhiteSpace > ABNFCharWhiteSpaceOrNewLine;
/// This rule satisfies both white space or new line

typedef ABNFLoopUntil<ABNFCharWhiteSpace, ABNFLoopExitIfNul, 0, 256> ABNFZeroOrMoreWhiteSpaces;
/// This rule satisfies zero or more white spaces

typedef ABNFLoopUntil<ABNFCharWhiteSpace, ABNFLoopExitIfNul, 1, 256> ABNFOneOrMoreWhiteSpaces;
/// This rule satisfies one or more white spaces

typedef ABNFLRSequence2<ABNFZeroOrMoreWhiteSpaces, ABNFCharCRLF> ABNFZeroOrMoreWhiteSpacesEndedByCRLF;
/// This rule satisfies zero or more white spaces terminated by a single CRLF

typedef ABNFCharComparison<':'> ABNFCharColon;
/// This rule satisfies a colon character

typedef ABNFCharComparison<'('> ABNFCharLParenthesis;
/// This rule satisfies a left parenthesis character

typedef ABNFCharComparison<')'> ABNFCharRParenthesis;
/// This rule satisfies a right parenthesis character

typedef ABNFCharComparison<'<'> ABNFCharLessThan;
/// This rule satisfies a less-than '<' character

typedef ABNFCharComparison<'>'> ABNFCharGreaterThan;
/// This rule satisfies a greater-than '>' character

typedef ABNFCharComparison<'@'> ABNFCharAtSymbol;
/// This rule satisfies an at sign '@' character

typedef ABNFCharComparison<','> ABNFCharComma;
/// This rule satisfies a comma ',' character

typedef ABNFCharComparison<';'> ABNFCharSemiColon;
/// This rule satisfies a semi-colon ';' character

typedef ABNFCharComparison<'\\'> ABNFCharBackSlash;
/// This rule satisfies a back-slash '\' character

typedef ABNFCharComparison<'/'> ABNFCharSlash;
/// This rule satisfies a forward-slash '/' character

typedef ABNFCharComparison<'['> ABNFCharLSquareBracket;
/// This rule satisfies a forward-slash '/' character

typedef ABNFCharComparison<']'> ABNFCharRSquareBracket;
typedef ABNFCharComparison<'?'> ABNFCharQuestionMark;
typedef ABNFCharComparison<'='> ABNFCharEqualSign;
typedef ABNFCharComparison<'{'> ABNFCharOpeningBraces;
typedef ABNFCharComparison<'}'> ABNFCharClosingBraces;
typedef ABNFCharComparison<'*'> ABNFCharAsterisk;
typedef ABNFCharComparison<'-'> ABNFCharDash;
typedef ABNFCharComparison<'.'> ABNFCharDot;
typedef ABNFCharComparison<'!'> ABNFCharExclamation;
typedef ABNFCharComparison<'%'> ABNFCharPercent;
typedef ABNFCharComparison<'_'> ABNFCharUnderscore;
typedef ABNFCharComparison<'+'> ABNFCharPlusSign;
typedef ABNFCharComparison<'`'> ABNFCharBackQuote;
typedef ABNFCharComparison<'\''> ABNFCharSingleQuote;
typedef ABNFCharComparison<'~'> ABNFCharTilde;
typedef ABNFCharComparison<'$'> ABNFCharDollar;
typedef ABNFCharComparison<'&'> ABNFCharAmpersand;
typedef ABNFAnyOf<ABNFCharAlpha, ABNFCharDigit> ABNFCharAlphaNumeric;
typedef ABNFRange<0x61, 0x66> ABNFCharLCaseA2F;
typedef ABNFAnyOf<ABNFCharDigit, ABNFCharLCaseA2F> ABNFCharLCaseHex;


} } //OSS::ABNF
#endif //ABNF_SIPPARSER_INCLUDE

