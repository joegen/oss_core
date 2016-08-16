
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

#include "OSS/UTL/Thread.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/LMDB/LMDatabase.h"


using OSS::LMDB::LMDatabase;


TEST(LMDBTest, TestLMDBGetSet)
{
  LMDatabase lmdb;
  LMDatabase::Options opt;
  opt.name = "test_db";
  opt.size_mb = 1;
  
  ASSERT_TRUE(lmdb.initialize(opt));
  std::string  strData = "string";
  double doubleData = 123.456;
  bool boolData = true;
  int32_t int32Data = 123456;
  int64_t int64Data = 123456;
  
  LMDatabase::Transaction transaction(&lmdb);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_TRUE(lmdb.clear(transaction));
    ASSERT_EQ(lmdb.count(transaction), 0);
  } while (false);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_TRUE(lmdb.set(transaction, "string", strData));
    ASSERT_TRUE(lmdb.set(transaction, "double", doubleData));
    ASSERT_TRUE(lmdb.set(transaction, "bool", boolData));
    ASSERT_TRUE(lmdb.set(transaction, "int32", int32Data));
    ASSERT_TRUE(lmdb.set(transaction, "int64", int64Data));
  } while (false);
  
  strData = "";
  doubleData = 0.00;
  boolData = false;
  int32Data = 0;
  int64Data = 0;
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_EQ(lmdb.count(transaction), 5);
    ASSERT_TRUE(lmdb.get(transaction, "string", strData));
    ASSERT_TRUE(lmdb.get(transaction, "double", doubleData));
    ASSERT_TRUE(lmdb.get(transaction, "bool", boolData));
    ASSERT_TRUE(lmdb.get(transaction, "int32", int32Data));
    ASSERT_TRUE(lmdb.get(transaction, "int64", int64Data));
  } while (false);
  
  ASSERT_STREQ(strData.c_str(), "string");
  ASSERT_EQ(doubleData, 123.456);
  ASSERT_EQ(boolData, true);
  ASSERT_EQ(int32Data, 123456);
  ASSERT_EQ(int64Data, 123456);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_TRUE(lmdb.del(transaction, "string"));
    ASSERT_TRUE(lmdb.del(transaction, "double"));
    ASSERT_TRUE(lmdb.del(transaction, "bool"));
    ASSERT_TRUE(lmdb.del(transaction, "int32"));
    ASSERT_TRUE(lmdb.del(transaction, "int64"));
    ASSERT_EQ(lmdb.count(transaction), 0);
  } while (false);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_FALSE(lmdb.get(transaction, "string", strData));
    ASSERT_FALSE(lmdb.get(transaction, "double", doubleData));
    ASSERT_FALSE(lmdb.get(transaction, "bool", boolData));
    ASSERT_FALSE(lmdb.get(transaction, "int32", int32Data));
    ASSERT_FALSE(lmdb.get(transaction, "int64", int64Data));
  } while (false);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    for (int32_t i = 0; i < 1000; i++)
    {
      const std::string key = OSS::string_from_number<int32_t>(i);
      ASSERT_TRUE(lmdb.set(transaction, key, i));
    }
    ASSERT_EQ(lmdb.count(transaction), 1000);
  } while (false);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    LMDatabase::Cursor cursor;
    ASSERT_TRUE(lmdb.createCursor(transaction, cursor));
    while (cursor.next())
    {
      int32_t val = (int32_t) (*((int32_t*)cursor.value().data()));
      ASSERT_EQ(val, OSS::string_to_number<int32_t>(cursor.key().c_str()));
    }
    
    ASSERT_TRUE(cursor.find("888"));
    ASSERT_EQ(888, (int32_t) (*((int32_t*)cursor.value().data())));
    
    ASSERT_FALSE(cursor.find("8888"));
    ASSERT_TRUE(cursor.value().empty() && cursor.key().empty());
    
  } while (false);
  
  do {
    LMDatabase::TransactionLock lock(transaction);
    ASSERT_TRUE(lmdb.drop(transaction));
    ASSERT_EQ(lmdb.count(transaction), 0);
  } while (false);
  
}


class Inserter : public OSS::Thread
{
public:
  Inserter(LMDatabase& db) :
    _db(db),
    _trn(&_db),
    _index(0)
  {
  }
  
  void insert()
  {
    LMDatabase::TransactionLock lock(_trn);
    for (int i = 0; i < 100; i++)
    {
      std::string key = OSS::string_from_number<std::size_t>(_index++);
      ASSERT_TRUE(_db.set(_trn, key, "Inserted Value"));
    }
  }
  
  void main()
  {
    while (_index < 1000)
    {
      insert();
      OSS::thread_sleep(10);
    }
  }
  
  LMDatabase& _db;
  LMDatabase::Transaction _trn;
  std::size_t _index;
};


class Deleter : public OSS::Thread
{
public:
  Deleter(LMDatabase& db) :
    _db(db),
    _trn(&_db),
    _index(0)
  {
  }
  
  void pop()
  {
    LMDatabase::TransactionLock lock(_trn);
    LMDatabase::Cursor cursor;
    ASSERT_TRUE(_db.createCursor(_trn, cursor));
    while(cursor.next())
    {
      ASSERT_FALSE(cursor.key().empty());
      ASSERT_TRUE(_db.del(_trn, cursor.key()));
      _index++;
    }
  }
  
  void main()
  {
    while (_index < 1000)
    {
      pop();
      OSS::thread_sleep(10);
    }
  }
  
  LMDatabase& _db;
  LMDatabase::Transaction _trn;
  std::size_t _index;
};


TEST(LMDBTest, TestLMDBThreaded)
{
  LMDatabase lmdb;
  LMDatabase::Options opt;
  opt.name = "test_db";
  opt.size_mb = 1;
 
  ASSERT_TRUE(lmdb.initialize(opt));
  
  Inserter updater(lmdb);
  Inserter inserter(lmdb);
  Deleter deleter(lmdb);
  
  inserter.run();
  updater.run();
  deleter.run();
  
  inserter.waitForTermination();
  updater.waitForTermination();
  deleter.waitForTermination();
  
  LMDatabase::Transaction transaction(&lmdb);
  LMDatabase::TransactionLock lock(transaction);
  ASSERT_TRUE(lmdb.drop(transaction));
}

