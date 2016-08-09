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
#include "OSS/build.h"
#include "OSS/BSON/BSON.h"



using OSS::BSON::BSONObject;
using OSS::BSON::BSONValue;
using OSS::BSON::BSONString;
using OSS::BSON::BSONBool;
using OSS::BSON::BSONDouble;
using OSS::BSON::BSONInt32;
using OSS::BSON::BSONInt64;
using OSS::BSON::BSONArray;
using OSS::BSON::BSONDocument;
using OSS::BSON::BSONQueue;


TEST(BSONTest, BSONDoc)
{
  BSONDocument doc;
  doc["string"] = BSONString("This is a UTF8 string");
  doc["bool"] = BSONBool(false);
  doc["double"] = BSONDouble(123.456);
  doc["int32"] = BSONInt32(123456);
  doc["int64"] = BSONInt64(123456);
  
  doc["array"][0] = BSONString("Element 1");
  doc["array"][1] = BSONString("Element 2");
  doc["array"][2] = BSONString("Element 3");
  
  doc["document"]["key1"] = BSONString("Element 1");
  doc["document"]["key2"] = BSONString("Element 2");
  doc["document"]["key3"] = BSONString("Element 3");
  
  doc["document"]["key4"][0] = BSONString("Element 1");
  doc["document"]["key4"][1] = BSONString("Element 2");
  doc["document"]["key4"][2] = BSONString("Element 3");
  
  BSONValue item;
  item = doc["string"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_STRING));
  ASSERT_TRUE(item.asString() == "This is a UTF8 string");
  item = doc["bool"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_BOOL));
  ASSERT_TRUE(item.asBoolean() == false);
  item = doc["double"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_DOUBLE));
  ASSERT_TRUE(item.asDouble() == 123.456);
  item  =doc["int32"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_INT32));
  ASSERT_TRUE(item.asInt32() == 123456);
  item = doc["int64"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_INT64));
  ASSERT_TRUE(item.asInt64() == 123456);
  
  ASSERT_TRUE(doc["array"][0].asString() == "Element 1");
  ASSERT_TRUE(doc["array"][1].asString() == "Element 2");
  ASSERT_TRUE(doc["array"][2].asString() == "Element 3");
  
  ASSERT_TRUE(doc["document"]["key1"].asString() == "Element 1");
  ASSERT_TRUE(doc["document"]["key2"].asString() == "Element 2");
  ASSERT_TRUE(doc["document"]["key3"].asString() == "Element 3");
  
  ASSERT_TRUE(doc["document"]["key4"][0].asString() == "Element 1");
  ASSERT_TRUE(doc["document"]["key4"][1].asString() == "Element 2");
  ASSERT_TRUE(doc["document"]["key4"][2].asString() == "Element 3");
  
  ASSERT_TRUE(doc.get("document.key4.0").isType(BSONValue::TYPE_STRING));
  ASSERT_TRUE(doc.get("document.key4.0").asString() == "Element 1");
  ASSERT_TRUE(doc.get("document.key5.0").isType(BSONValue::TYPE_UNDEFINED));
  ASSERT_TRUE(doc.get("document.key4.5").isType(BSONValue::TYPE_UNDEFINED));
  
  doc.get("document.key4.0") = BSONString("Test get() function");
  ASSERT_TRUE(doc.get("document.key4.0").asString() == "Test get() function");
  
  ASSERT_EQ(doc.size(), 7);
  ASSERT_EQ(doc["document"].size(), 4);
  ASSERT_EQ(doc["document"]["key4"].size(), 3);
  
  BSONObject bson;
  doc.toBSON(bson);
  ASSERT_TRUE(doc.toJSON() == bson.stringify()); 
}

#if ENABLE_FEATURE_ZMQ

TEST(BSONTest, BSONBSONQueue)
{
  BSONQueue producer(BSONQueue::PRODUCER, "q1");
  BSONQueue consumer(BSONQueue::CONSUMER, "q1");
  
  BSONDocument work;
  work["string"] = BSONString("This is a UTF8 string");
  work["bool"] = BSONBool(false);
  work["double"] = BSONDouble(123.456);
  work["int32"] = BSONInt32(123456);
  work["int64"] = BSONInt64(123456);
  
  work["array"][0] = BSONString("Element 1");
  work["array"][1] = BSONString("Element 2");
  work["array"][2] = BSONString("Element 3");
  
  work["document"]["key1"] = BSONString("Element 1");
  work["document"]["key2"] = BSONString("Element 2");
  work["document"]["key3"] = BSONString("Element 3");
  
  work["document"]["key4"][0] = BSONString("Element 1");
  work["document"]["key4"][1] = BSONString("Element 2");
  work["document"]["key4"][2] = BSONString("Element 3");
  
  ASSERT_TRUE(producer.enqueue(work));
  
  BSONDocument doc;
  ASSERT_TRUE(consumer.dequeue(doc));
  
  BSONValue item;
  item = doc["string"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_STRING));
  ASSERT_TRUE(item.asString() == "This is a UTF8 string");
  item = doc["bool"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_BOOL));
  ASSERT_TRUE(item.asBoolean() == false);
  item = doc["double"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_DOUBLE));
  ASSERT_TRUE(item.asDouble() == 123.456);
  item  =doc["int32"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_INT32));
  ASSERT_TRUE(item.asInt32() == 123456);
  item = doc["int64"];
  ASSERT_TRUE(item.isType(BSONValue::TYPE_INT64));
  ASSERT_TRUE(item.asInt64() == 123456);
  
  ASSERT_TRUE(doc["array"][0].asString() == "Element 1");
  ASSERT_TRUE(doc["array"][1].asString() == "Element 2");
  ASSERT_TRUE(doc["array"][2].asString() == "Element 3");
  
  ASSERT_TRUE(doc["document"]["key1"].asString() == "Element 1");
  ASSERT_TRUE(doc["document"]["key2"].asString() == "Element 2");
  ASSERT_TRUE(doc["document"]["key3"].asString() == "Element 3");
  
  ASSERT_TRUE(doc["document"]["key4"][0].asString() == "Element 1");
  ASSERT_TRUE(doc["document"]["key4"][1].asString() == "Element 2");
  ASSERT_TRUE(doc["document"]["key4"][2].asString() == "Element 3");
}

#endif


