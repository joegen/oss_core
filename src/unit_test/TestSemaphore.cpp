
#include "gtest/gtest.h"
#include "OSS/UTL/Semaphore.h"
#include "Poco/Semaphore.h"

using namespace OSS;


TEST(ThreadTest, test_counting_semaphore_basic)
{
  Semaphore sem0;
  
  for (int i = 0; i < 1000000; i++)
    sem0.signal();

  for (int i = 0; i < 1000000; i++)
    ASSERT_TRUE(sem0.wait(0));

  for (int i = 0; i < 500; i++)
    sem0.signal();

  for (int i = 0; i < 500; i++)
    ASSERT_TRUE(sem0.wait(100));

  for (int i = 0; i < 500; i++)
    ASSERT_FALSE(sem0.wait(0));

  for (int i = 0; i < 500; i++)
    sem0.signal();

  for (int i = 0; i < 500; i++)
    ASSERT_TRUE(sem0.wait(-100));

  for (int i = 0; i < 500; i++)
    sem0.signal();

  for (int i = 0; i < 500; i++)
    sem0.wait();
}

TEST(ThreadTest, test_counting_semaphore_poco_basic)
{
  Poco::Semaphore sem(10);
  
  for (int i = 0; i < 10; i++)
    ASSERT_TRUE(sem.tryWait(1));
  
   ASSERT_FALSE(sem.tryWait(1));
   
  for (int i = 0; i < 10; i++)
    sem.set();
   
  for (int i = 0; i < 10; i++)
    ASSERT_TRUE(sem.tryWait(1));
}