/*
Basic Assertions

These assertions do basic true/false condition testing.
Fatal assertion 	Nonfatal assertion 	Verifies
ASSERT_TRUE(condition); 	EXPECT_TRUE(condition); 	condition is true
ASSERT_FALSE(condition); 	EXPECT_FALSE(condition); 	condition is false

Remember, when they fail, ASSERT_* yields a fatal failure and returns from the current function, while EXPECT_* yields a nonfatal failure, allowing the function to continue running. In either case, an assertion failure means its containing test fails.

Availability: Linux, Windows, Mac.
Binary Comparison

This section describes assertions that compare two values.

Fatal assertion 	Nonfatal assertion 	Verifies
ASSERT_EQ(expected, actual);	EXPECT_EQ(expected, actual);	expected == actual
ASSERT_NE(val1, val2); 	EXPECT_NE(val1, val2); 	val1 != val2
ASSERT_LT(val1, val2); 	EXPECT_LT(val1, val2); 	val1 < val2
ASSERT_LE(val1, val2); 	EXPECT_LE(val1, val2); 	val1 <= val2
ASSERT_GT(val1, val2); 	EXPECT_GT(val1, val2); 	val1 > val2
ASSERT_GE(val1, val2); 	EXPECT_GE(val1, val2); 	val1 >= val2

In the event of a failure, Google Test prints both val1 and val2 . In ASSERT_EQ* and EXPECT_EQ* (and all other equality assertions we'll introduce later), you should put the expression you want to test in the position of actual, and put its expected value in expected, as Google Test's failure messages are optimized for this convention.

Value arguments must be comparable by the assertion's comparison operator or you'll get a compiler error. Values must also support the << operator for streaming to an ostream. All built-in types support this.

These assertions can work with a user-defined type, but only if you define the corresponding comparison operator (e.g. ==, <, etc). If the corresponding operator is defined, prefer using the ASSERT_*() macros because they will print out not only the result of the comparison, but the two operands as well.

Arguments are always evaluated exactly once. Therefore, it's OK for the arguments to have side effects. However, as with any ordinary C/C++ function, the arguments' evaluation order is undefined (i.e. the compiler is free to choose any order) and your code should not depend on any particular argument evaluation order.

ASSERT_EQ() does pointer equality on pointers. If used on two C strings, it tests if they are in the same memory location, not if they have the same value. Therefore, if you want to compare C strings (e.g. const char*) by value, use ASSERT_STREQ() , which will be described later on. In particular, to assert that a C string is NULL, use ASSERT_STREQ(NULL, c_string) . However, to compare two string objects, you should use ASSERT_EQ.

Macros in this section work with both narrow and wide string objects (string and wstring).

Availability: Linux, Windows, Mac.
String Comparison

The assertions in this group compare two C strings. If you want to compare two string objects, use EXPECT_EQ, EXPECT_NE, and etc instead.

Fatal assertion 	Nonfatal assertion 	Verifies
ASSERT_STREQ(expected_str, actual_str); 	EXPECT_STREQ(expected_str, actual_str); 	the two C strings have the same content
ASSERT_STRNE(str1, str2); 	EXPECT_STRNE(str1, str2); 	the two C strings have different content
ASSERT_STRCASEEQ(expected_str, actual_str);	EXPECT_STRCASEEQ(expected_str, actual_str); 	the two C strings have the same content, ignoring case
ASSERT_STRCASENE(str1, str2);	EXPECT_STRCASENE(str1, str2); 	the two C strings have different content, ignoring case

Note that "CASE" in an assertion name means that case is ignored.

*STREQ* and *STRNE* also accept wide C strings (wchar_t*). If a comparison of two wide strings fails, their values will be printed as UTF-8 narrow strings.

A NULL pointer and an empty string are considered different.

Availability: Linux, Windows, Mac.

See also: For more string comparison tricks (substring, prefix, suffix, and regular expression matching, for example), see the [AdvancedGuide Advanced Google Test Guide].
*/

#include "gtest/gtest.h"

#include "OSS/SIP/SIPAuthorization.h"
#include "OSS/SIP/SIPDigestAuth.h"

using namespace OSS::SIP;

std::string user = "test-user";
std::string password = "test-password";
std::string method = "INVITE";
std::string realm = "domain.com";
std::string uri = "sip:12345@domain.com";
std::string algorithm = "MD5";
std::string qop = "auth";
std::string nonce = SIPDigestAuth::digestCreateA1Hash(user, password, realm);
  
TEST(TestDigestAuth, SIPProxyAuthenticate)
{
  /*
  challenge        =  "Digest" digest-challenge

  digest-challenge  = 1#( realm | [ domain ] | nonce |
                      [ opaque ] |[ stale ] | [ algorithm ] |
                      [ qop-options ] | [auth-param] )


  domain            = "domain" "=" <"> URI ( 1*SP URI ) <">
  URI               = absoluteURI | abs_path
  nonce             = "nonce" "=" nonce-value
  nonce-value       = quoted-string
  opaque            = "opaque" "=" quoted-string
  stale             = "stale" "=" ( "true" | "false" )
  algorithm         = "algorithm" "=" ( "MD5" | "MD5-sess" |
                       token )
  qop-options       = "qop" "=" <"> 1#qop-value <">
  qop-value         = "auth" | "auth-int" | token
   */ 
  
  //
  // Test creation
  //
  SIPAuthorization pa1;
 
  pa1.setAlgorithm(algorithm.c_str());
  ASSERT_STREQ(algorithm.c_str(), pa1.getAlgorithm().c_str());
  
  pa1.setRealm(realm.c_str());
  ASSERT_STREQ(realm.c_str(), pa1.getRealm().c_str());
   
  pa1.setNonce(nonce.c_str());
  ASSERT_STREQ(nonce.c_str(), pa1.getNonce().c_str());
  
  pa1.setQop(qop.c_str());
  ASSERT_STREQ(qop.c_str(), pa1.getQop().c_str());
  
  //
  // Test creation from string
  //
  SIPAuthorization pa2(pa1.data());
  ASSERT_STREQ(algorithm.c_str(), pa2.getAlgorithm().c_str());
  ASSERT_STREQ(realm.c_str(), pa2.getRealm().c_str());
  ASSERT_STREQ(nonce.c_str(), pa2.getNonce().c_str());
  ASSERT_STREQ(qop.c_str(), pa2.getQop().c_str());
  
  //
  // Test copy constructor
  //
  SIPAuthorization pa3(pa2);
  ASSERT_STREQ(algorithm.c_str(), pa3.getAlgorithm().c_str());
  ASSERT_STREQ(realm.c_str(), pa3.getRealm().c_str());
  ASSERT_STREQ(nonce.c_str(), pa3.getNonce().c_str());
  ASSERT_STREQ(qop.c_str(), pa3.getQop().c_str());
  
  //
  // Test equal operator
  //
  SIPAuthorization pa4;
  pa4 = pa3;
  ASSERT_STREQ(algorithm.c_str(), pa4.getAlgorithm().c_str());
  ASSERT_STREQ(realm.c_str(), pa4.getRealm().c_str());
  ASSERT_STREQ(nonce.c_str(), pa4.getNonce().c_str());
  ASSERT_STREQ(qop.c_str(), pa4.getQop().c_str());
  
  //
  // Test equal from string
  //
  SIPAuthorization pa5;
  pa5 = pa4.data();
  ASSERT_STREQ(algorithm.c_str(), pa5.getAlgorithm().c_str());
  ASSERT_STREQ(realm.c_str(), pa5.getRealm().c_str());
  ASSERT_STREQ(nonce.c_str(), pa5.getNonce().c_str());
  ASSERT_STREQ(qop.c_str(), pa5.getQop().c_str());
  
  
  //
  // Test creation of authorization
  //
  std::string a1;
  std::string a2;
  
  a1 = SIPDigestAuth::digestCreateA1Hash(user, password, realm);
  ASSERT_TRUE(!a1.empty());
  
  a2 = SIPDigestAuth::digestCreateA2Hash(uri, method.c_str());
  ASSERT_TRUE(!a2.empty());
  
  std::string auth = SIPDigestAuth::digestCreateAuthorization(a1, nonce, a2);
  ASSERT_TRUE(!auth.empty());
  
  std::cout << auth << std::endl;
}

