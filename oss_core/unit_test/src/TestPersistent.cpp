


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
#include "OSS/Persistent/DataType.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/OSSSIP.h"
#include "OSS/SIP/SBC/SBCManager.h"

using OSS::Persistent::DataType;
using OSS::Persistent::ClassType;
using OSS::Persistent::PersistenceException;

TEST(PersistentTest, data_type_volatile)
{
  DataType stringType, charPointerType, boolType, intType, floatType, doubleType, longLongType;

  stringType = std::string("This is a string");
  ASSERT_STREQ("This is a string", static_cast<const std::string&>(stringType).c_str());
  ASSERT_STREQ("This is a string", static_cast<const char*>(stringType));
  
  charPointerType = "This is a const char pointer";
  ASSERT_STREQ("This is a const char pointer", static_cast<const char*>(charPointerType));
  ASSERT_STREQ("This is a const char pointer", static_cast<const std::string&>(charPointerType).c_str());

  DataType clone = charPointerType;
  ASSERT_STREQ("This is a const char pointer", static_cast<const char*>(clone));
  ASSERT_STREQ("This is a const char pointer", static_cast<const std::string&>(clone).c_str());

  boolType = true;
  ASSERT_TRUE((bool)boolType);
  boolType = false;
  ASSERT_FALSE((bool)boolType);

  int intT = 9999;
  intType = intT;
  ASSERT_EQ((int)intT, 9999);

  float floatT = 9999.99;
  floatType = floatT;
  ASSERT_EQ((float)9999.99, (float)floatType);

  double doubleT = 9999.9999;
  doubleType = doubleT;
  ASSERT_EQ((double)9999.9999, (double)doubleType);

  long long longLongT = 1234567890;
  longLongType = longLongT;
  ASSERT_EQ((long long)1234567890, (long long)longLongType); 
}


TEST(PersistentTest, createSafeFileName)
{
  ClassType::createSafeFileName("z9hG4bK-d87543-419889160-1--d87543");
}
