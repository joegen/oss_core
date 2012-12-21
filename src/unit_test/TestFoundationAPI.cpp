#include <gtest/gtest.h>

#include "OSS/BlockingQueue.h"
#include "OSS/Crypto.h"
#include <sstream>
#include "OSS/Core.h"
#include "OSS/UTL/AdaptiveDelay.h"

TEST(TestFoundation, blocking_queue)
{
  //
  // Test if semaphores can be set prior to a wait
  //
  OSS::semaphore sem(0,100);
  sem.set();
  ASSERT_TRUE(sem.tryWait(0));
  ASSERT_FALSE(sem.tryWait(0));

  //
  // Test semaphore/queue combo
  //
  OSS::BlockingQueue<int> q;
  for (int i = 0; i < 100; i++)
    q.enqueue(i);

  for (int i = 0; i < 100; i++)
  {
    int x;
    q.dequeue(x);
    ASSERT_EQ(i, x);
  }

  int x = 999;
  ASSERT_FALSE(q.try_dequeue(x, 100));
  ASSERT_EQ(x, 999);

  //
  // Verify md5 encryption
  //
  std::stringstream in;
  in << "this is a sample md5 input";
  std::ostringstream out;
  OSS::crypto_md5_encode(in, out);
  std::string md5 = OSS::string_md5_hash(in.str().c_str());
  ASSERT_STREQ(out.str().c_str(), md5.c_str());

  //
  // Verify md5 encryption using sip
  //
  std::stringstream a1;
  std::ostringstream ha1;

  a1 << "17772322706:callcentric.com:durak1";
  OSS::crypto_md5_encode(a1, ha1);

  std::stringstream a2;
  std::ostringstream ha2;

  a2 << "INVITE:sip:17772322706@callcentric.com;transport=udp";
  OSS::crypto_md5_encode(a2, ha2);
  std::string nonce = "0ae63a71bda5f5ac40ea3b73151b2062";
  
  std::stringstream hmd5;
  std::ostringstream hmd5response;

  hmd5 << ha1.str() << ":" << nonce << ":" << ha2.str();
  OSS::crypto_md5_encode(hmd5, hmd5response);
  ASSERT_STREQ(hmd5response.str().c_str(), "b1eef0d24f098ac498f1bf342c9eff12");

  std::vector<char> b64Out;
  OSS::crypto_base64_decode("Y29kZT0xMjM0LXVzZXJuYW1lPTMzMDQ1LW1hYz0wMDEyMDAxMjAwOQ==", b64Out);
  ASSERT_STREQ(&b64Out[0], "code=1234-username=33045-mac=00120012009");

  std::string replaceStr = "FreeSWITCH KarooBridge FreeSWITCH";
  OSS::string_replace(replaceStr, "FreeSWITCH", "KarooBridge");
  ASSERT_STREQ(replaceStr.c_str(), "KarooBridge KarooBridge KarooBridge");


}


TEST(TestFoundation, adaptive_timer)
{
  //
  // Test adaptive delay with 5 ms resolution running a span of 5 seconds;
  //

  const int testDuration = 5; // seconds
  const int resolution = 5; // milliseconds
  const int iterations = testDuration * (1000/resolution);

  OSS::UTL::AdaptiveDelay delay(resolution);
  OSS::UInt64 start = OSS::getTime();
  
  for (int i = 0; i < iterations; i++)
    delay.wait();

  // ASSERT_EQ(start + (resolution * iterations),  OSS::getTime());

  OSS::UInt64 expected = start + (resolution * iterations);
  OSS::UInt64 actual = OSS::getTime();

  //
  // Give or take 1 iteration 
  //
  if (actual > expected)
    ASSERT_TRUE( actual - expected <= resolution);
  else if (expected > actual)
    ASSERT_TRUE( expected - actual  <= resolution);
}