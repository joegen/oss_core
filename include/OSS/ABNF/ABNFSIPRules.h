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


#ifndef ABNF_SIPSIPRULES_INCLUDED
#define ABNF_SIPSIPRULES_INCLUDED


#include "OSS/ABNF/ABNFParser.h"


namespace OSS {

namespace ABNF {


typedef ABNFCharSpace ABNF_SIP_SP; /// Single space

typedef ABNFCharCRLF ABNF_SIP_CRLF;/// CRLF

typedef ABNFCharHTab ABNF_SIP_HTAB;/// Horizontal tab

typedef ABNFCharDQuote ABNF_SIP_DQUOTE;/// A double quote rule

typedef ABNFCharHex ABNF_SIP_HEXDIG;/// A Hex+Digit rule

typedef ABNFCharAlphaNumeric ABNF_SIP_alphanum;/// Rule stisfying digits and alphabet characters

typedef ABNFLRSequence2<ABNFLROptional<ABNFZeroOrMoreWhiteSpacesEndedByCRLF>, ABNFOneOrMoreWhiteSpaces> ABNF_SIP_LWS;/// RFC 3261 ABNF Rule for (LWS = [*WSP CRLF] 1*WSP)

typedef ABNFLROptional<ABNF_SIP_LWS> ABNF_SIP_SWS;/// RFC 3261 ABNF Rule for (SWS = [LWS]) 

typedef ABNFLRSequence3<ABNFZeroOrMoreWhiteSpaces, ABNFCharColon, ABNF_SIP_SWS> ABNF_SIP_HCOLON;/// RFC 3261 ABNF Rule for HCOLON	=   *( SP  / HTAB ) ":" SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharAsterisk, ABNF_SIP_SWS> ABNF_SIP_STAR;/// RFC 3261 ABNF Rule for STAR	  =  	SWS  	"*"  	SWS 

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharSlash, ABNF_SIP_SWS> ABNF_SIP_SLASH;/// RFC 3261 ABNF Rule for SLASH	= 	SWS 	"/" 	SWS   

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharEqualSign, ABNF_SIP_SWS> ABNF_SIP_EQUAL; /// RFC 3261 ABNF Rule for EQUAL	= 	SWS 	"=" 	SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharLParenthesis, ABNF_SIP_SWS> ABNF_SIP_LPAREN;/// RFC 3261 ABNF Rule for LPAREN	= 	SWS 	"(" 	SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharRParenthesis, ABNF_SIP_SWS> ABNF_SIP_RPAREN;/// RFC 3261 ABNF Rule for RPAREN	= 	SWS 	")" 	SWS

typedef ABNFLRSequence2<ABNF_SIP_SWS, ABNFCharLessThan> ABNF_SIP_LAQUOT;/// RFC 3261 ABNF Rule for LAQUOT	= 	SWS 	"<"

typedef ABNFLRSequence2<ABNFCharGreaterThan, ABNF_SIP_SWS> ABNF_SIP_RAQUOT;/// RFC 3261 ABNF Rule for RAQUOT	= 	">" 	SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharComma, ABNF_SIP_SWS> ABNF_SIP_COMMA;/// RFC 3261 ABNF Rule for COMMA	= 	SWS 	"," 	SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharSemiColon, ABNF_SIP_SWS> ABNF_SIP_SEMI;/// RFC 3261 ABNF Rule for SEMI	  = 	SWS 	";" 	SWS

typedef ABNFLRSequence3<ABNF_SIP_SWS, ABNFCharColon, ABNF_SIP_SWS> ABNF_SIP_COLON;/// RFC 3261 ABNF Rule for COLON	= 	SWS 	":" 	SWS

typedef ABNFLRSequence2<ABNF_SIP_SWS, ABNF_SIP_DQUOTE> ABNF_SIP_LDQUOT;/// RFC 3261 ABNF Rule for LDQUOT	= 	SWS 	DQUOTE

typedef ABNFLRSequence2<ABNF_SIP_DQUOTE,ABNF_SIP_SWS> ABNF_SIP_RDQUOT;/// RFC 3261 ABNF Rule for RDQUOT	= 	DQUOTE 	SWS

typedef ABNFRange<0x80, 0xBF> ABNF_SIP_UTF8_CONT;/// RFC 3261 ABNF Rule for UTF8-CONT       =  %x80-BF

typedef ABNFLRSequence2<ABNFCharPercent, ABNF_SIP_HEXDIG> ABNF_SIP_escaped; /// RFC 3261 ABNF Rule for escaped     =  "%" HEXDIG HEXDIG

typedef ABNFAnyOfChars<';', '/', '?', ':', '@', '&', '=', '+', '$', ','> ABNF_SIP_reserved; /// RFC 3261 ABNF Rule for reserved    =  ";" / "/" / "?" / ":" / "@" / "&" / "=" / "+" / "$" / ","

typedef ABNFAnyOfChars<'-', '_', '.', '!', '~', '*', '\'', '/'> ABNF_SIP_mark; /// RFC 3261 ABNF Rule for mark        =  "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"

typedef ABNFAnyOfChars<';', '=', '-', '_', '.', '!', '~', '*', '\'', '/'> ABNF_SIP_header_mark; /// RFC 3261 ABNF Rule for mark        =  "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"

typedef ABNFAnyOf<ABNF_SIP_alphanum, ABNF_SIP_mark> ABNF_SIP_unreserved;/// RFC 3261 ABNF Rule for unreserved  =  alphanum / mark  

typedef ABNFAnyOf<ABNF_SIP_alphanum, ABNF_SIP_header_mark> ABNF_SIP_header_unreserved;/// RFC 3261 ABNF Rule for unreserved  =  alphanum / mark

typedef ABNFAnyOfChars<'&', '=', '+', '$', ',', ';', '?', '/' >ABNF_SIP_user_unreserved; /// RFC 3261 ABNF Rule for user-unreserved  =  "&" / "=" / "+" / "$" / "," / ";" / "?" / "/"

typedef ABNFCharAlpha ABNF_SIP_ALPHA;

typedef ABNFCharDigit ABNF_SIP_DIGIT;

typedef ABNFAnyOfChars<'[',  ']', '/',  ':', '&', '+', '$'> ABNF_SIP_param_unreserved; /// RFC 3261 ABNF Rule for param-unreserved  =  "[" / "]" / "/" / ":" / "&" / "+" / "$"

typedef ABNFAnyOfMultiple3<ABNF_SIP_param_unreserved, ABNF_SIP_unreserved, ABNF_SIP_escaped> ABNF_SIP_paramchar;  /// RFC 3261 ABNF Rule forparamchar         =  param-unreserved / unreserved / escaped

typedef ABNFLoopUntil<ABNF_SIP_paramchar, ABNFLoopExitIfNul, 1, 1024> ABNF_SIP_pname; /// RFC 3261 ABNF Rule for pname =  1*paramchar

typedef ABNF_SIP_pname ABNF_SIP_pvalue; /// RFC 3261 ABNF Rule for pvalue =  1*paramchar

typedef ABNFAnyOfChars<'[', ']', '/', '?', ':', '+', '$'> ABNF_SIP_hnv_unreserved;/// RFC 3261 ABNF Rule for hnv-unreserved  =  "[" / "]" / "/" / "?" / ":" / "+" / "$"

typedef ABNFAnyOfMultiple3<ABNF_SIP_hnv_unreserved, ABNF_SIP_header_unreserved, ABNF_SIP_escaped> ABNF_SIP_hparamchar;/// hnv-unreserved / unreserved / escaped
typedef ABNFAnyOfMultiple3<ABNF_SIP_hnv_unreserved, ABNF_SIP_unreserved, ABNF_SIP_escaped> ABNF_SIP_hnparamchar;/// hnv-unreserved / unreserved / escaped

typedef ABNFLoopUntil<ABNF_SIP_hnparamchar, ABNFLoopExitIfNul, 1, 1024> ABNF_SIP_hname; /// RFC 3261 ABNF Rule for hname =  1*( hnv-unreserved / unreserved / escaped )

typedef ABNFLoopUntil<ABNF_SIP_hparamchar, ABNFLoopExitIfNul, 0, 1024> ABNF_SIP_hvalue; /// RFC 3261 ABNF Rule for hvalue =  *( hnv-unreserved / unreserved / escaped )

typedef ABNFAnyOfMultiple3<ABNF_SIP_reserved, ABNF_SIP_unreserved, ABNF_SIP_escaped> ABNF_SIP_uric; /// RFC 3261 ABNF Rule for uric  =  reserved / unreserved / escaped

typedef ABNFLoopUntil<ABNF_SIP_uric, ABNFLoopExitIfNul, 0, 1024>ABNF_SIP_query;///RFC 3261 ABNF Rule for query  =  *uric

typedef ABNFAnyOfMultiple3<ABNF_SIP_unreserved, ABNF_SIP_escaped, ABNFAnyOfChars< ';', '?', ':', '@', '&', '=', '+', '$', ','> >ABNF_SIP_uric_no_slash;/// RFC 3261 ABNF Rule for uric-no-slash  =  unreserved / escaped / ";" / "?" / ":" / "@" / "&" / "=" / "+" / "$" / ","

typedef ABNFAnyOfMultiple3<ABNF_SIP_unreserved, ABNF_SIP_escaped, ABNFAnyOfChars<':', '@', '&', '=', '+', '$', ','> >ABNF_SIP_pchar;/// pchar  =  unreserved / escaped / ":" / "@" / "&" / "=" / "+" / "$" / ","

typedef ABNFLoopUntil<ABNF_SIP_pchar, ABNFLoopExitIfNul, 0, 1024> ABNF_SIP_param;///RFC 3261 ABNF Rule for param   =  *pchar

} } //OSS::ABNF
#endif //ABNF_SIPSIPRULES_INCLUDED
