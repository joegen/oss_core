

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

#include <set>
#include <memory>

#include "gtest/gtest.h"
#include "OSS/BSON/BSONObject.h"
#include "OSS/BSON/BSONValue.h"


using OSS::BSON::BSONValue;
using OSS::BSON::BSONObject;

TEST(BSONTest, BSONValuePlain)
{
  BSONValue value, copy;
  
  value.setValue("value");
  ASSERT_TRUE(value.isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", value.asString().c_str());
  ASSERT_TRUE(static_cast<std::string&>(value) == "value");
  
  value.setValue(std::string("value"));
  ASSERT_TRUE(value.isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", value.asString().c_str());
  ASSERT_TRUE(static_cast<std::string&>(value) == "value");
  
  copy.setValue(value);
  ASSERT_TRUE(copy.isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", copy.asString().c_str());
  ASSERT_TRUE(static_cast<std::string&>(copy) == "value");
  
  value.setValue(false);
  ASSERT_TRUE(value.isType(BSONValue::TYPE_BOOL));
  ASSERT_FALSE(value.asBoolean());
  ASSERT_TRUE(static_cast<bool>(value) == false);
  
  value.setValue(int32_t(12345));
  ASSERT_TRUE(value.isType(BSONValue::TYPE_INT32));
  ASSERT_TRUE(value.asInt32() == 12345);
  ASSERT_TRUE(static_cast<int32_t>(value) == 12345);
  
  value.setValue(int64_t(12345));
  ASSERT_TRUE(value.isType(BSONValue::TYPE_INT64));
  ASSERT_TRUE(value.asInt64() == 12345);
  ASSERT_TRUE(static_cast<int64_t>(value) == 12345);
  
  value.setValue(double(123.45));
  ASSERT_TRUE(value.isType(BSONValue::TYPE_DOUBLE));
  ASSERT_TRUE(value.asDouble() == 123.45);
  ASSERT_TRUE(static_cast<double>(value) == 123.45);
  
  value.setValue(123.45);
  ASSERT_TRUE(value.isType(BSONValue::TYPE_DOUBLE));
  ASSERT_TRUE(value.asDouble() == 123.45);
  ASSERT_TRUE(static_cast<double>(value) == 123.45);
  
  value.setValue(12345);
  ASSERT_TRUE(value.isType(BSONValue::TYPE_INT32));
  ASSERT_TRUE(value.asInt32() == 12345);
  ASSERT_TRUE(static_cast<int32_t>(value) == 12345);
}

TEST(BSONTest, BSONValueArray)
{
  BSONValue value;
  BSONValue root(BSONValue::TypeArray());
  BSONValue::Array& valueArray = (BSONValue::Array&)root;
  
  value.setValue("value");
  valueArray.push_back(value);
  
  
  value.setValue(std::string("value"));
  valueArray.push_back(value);
   
  value.setValue(false);
  valueArray.push_back(value);
  
  value.setValue(int32_t(12345));
  valueArray.push_back(value);
  
  value.setValue(int64_t(12345));
  valueArray.push_back(value);
  
  value.setValue(double(123.45));
  valueArray.push_back(value);
  
  ASSERT_TRUE(valueArray[0].isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", valueArray[0].asString().c_str());
  ASSERT_TRUE(static_cast<std::string&>(valueArray[0]) == "value");
  
  ASSERT_TRUE(valueArray[1].isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", valueArray[1].asString().c_str());
  ASSERT_TRUE(static_cast<std::string&>(valueArray[1]) == "value");
  
  ASSERT_TRUE(valueArray[2].isType(BSONValue::TYPE_BOOL));
  ASSERT_FALSE(valueArray[2].asBoolean());
  ASSERT_TRUE(static_cast<bool>(valueArray[2]) == false);
  
  ASSERT_TRUE(valueArray[3].isType(BSONValue::TYPE_INT32));
  ASSERT_TRUE(valueArray[3].asInt32() == 12345);
  ASSERT_TRUE(static_cast<int32_t>(valueArray[3]) == 12345);
  
  ASSERT_TRUE(valueArray[4].isType(BSONValue::TYPE_INT64));
  ASSERT_TRUE(valueArray[4].asInt64() == 12345);
  ASSERT_TRUE(static_cast<int64_t>(valueArray[4]) == 12345);
  
  ASSERT_TRUE(valueArray[5].isType(BSONValue::TYPE_DOUBLE));
  ASSERT_TRUE(valueArray[5].asDouble() == 123.45);
  ASSERT_TRUE(static_cast<double>(valueArray[5]) == 123.45);
  
  //ASSERT_EQ(root.asArray().size(), 6);
  
  //
  // Used index
  //
  BSONValue root2(BSONValue::TypeArray());
  root2[4] = BSONValue(std::string("val_1"));
  root2[3] = BSONValue(bool(true));
  root2[2] = BSONValue(double(123.456));
  root2[1] = BSONValue(int32_t(123456));
  root2[0] = BSONValue(int64_t(123456));
  
  //ASSERT_EQ(root2.asArray().size(), 5);
}

#if 0 
TEST(BSONTest, BSONValueDocument)
{
  BSONValue value;
  BSONValue root(BSONValue::Document());
  BSONValue::Document& valueDocument = (BSONValue::Document&)root;
  
  value.setValue(std::string("value"));
  valueDocument["key1"] = value;
  
  value.setValue(bool(false));
  valueDocument["key2"] = value;
  
  value.setValue(double(123.456));
  valueDocument["key3"] = value;
  
  value.setValue(int32_t(123456));
  valueDocument["key4"] = value;
  
  value.setValue(int64_t(123456));
  valueDocument["key5"] = value;
  
  
  ASSERT_TRUE(valueDocument.find("key1")->second.isType(BSONValue::TYPE_STRING));
  ASSERT_STREQ("value", valueDocument.find("key1")->second.asString().c_str());
  ASSERT_TRUE(static_cast<const std::string&>((*valueDocument.find("key1")->second)) == "value");
  
  ASSERT_TRUE(valueDocument.find("key2")->second.isType(BSONValue::TYPE_BOOL));
  ASSERT_EQ(valueDocument.find("key2")->second.asBoolean(), false);
  ASSERT_EQ(static_cast<const bool&>((*valueDocument.find("key2")->second)), false);
  
  ASSERT_TRUE(valueDocument.find("key3")->second.isType(BSONValue::TYPE_DOUBLE));
  ASSERT_EQ(valueDocument.find("key3")->second.asDouble(), 123.456);
  ASSERT_EQ(static_cast<const double&>((*valueDocument.find("key3")->second)), 123.456);
  
  ASSERT_TRUE(valueDocument.find("key4")->second.isType(BSONValue::TYPE_INT32));
  ASSERT_EQ(valueDocument.find("key4")->second.asInt32(), 123456);
  ASSERT_EQ(static_cast<const int32_t&>((*valueDocument.find("key4")->second)), 123456);
  
  ASSERT_TRUE(valueDocument.find("key5")->second.isType(BSONValue::TYPE_INT64));
  ASSERT_EQ(valueDocument.find("key5")->second.asInt64(), 123456);
  ASSERT_EQ(static_cast<const int64_t&>((*valueDocument.find("key5")->second)), 123456);
  
  ASSERT_EQ(root.asDocument().size(), 5);
  
  for (BSONValue::Document::iterator iter = valueDocument.begin(); iter !=  valueDocument.end(); iter++)
  {
    if (iter->second.isType(BSONValue::TYPE_STRING))
    {
      ASSERT_STREQ(iter->second.asString().c_str(), "value");
    }
    else if (iter->second.isType(BSONValue::TYPE_BOOL))
    {
      ASSERT_EQ(iter->second.asBoolean(), false);
    }
    else if (iter->second.isType(BSONValue::TYPE_DOUBLE))
    {
      ASSERT_EQ(iter->second.asDouble(), 123.456);
    }
    else if (iter->second.isType(BSONValue::TYPE_INT32))
    {
      ASSERT_EQ(iter->second.asInt32(), 123456);
    }
    else if (iter->second.isType(BSONValue::TYPE_INT64))
    {
      ASSERT_EQ(iter->second.asInt64(), 123456);
    }
  }
}
#endif

TEST(BSONTest, BSONObject)
{
}