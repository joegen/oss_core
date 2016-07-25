#include "gtest/gtest.h"

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ

#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/SIP/SIPTransportService.h" 

using namespace OSS::ZMQ;



TEST(ZMQ, test_zmq_send_and_receive)
{
  ZMQSocket req(ZMQSocket::REQ);
  ZMQSocket rep(ZMQSocket::REP);
  
  ASSERT_TRUE(rep.bind("tcp://127.0.0.1:50000"));
  OSS::thread_sleep(1000);
  ASSERT_TRUE(req.connect("tcp://127.0.0.1:50000"));
  
  for (int i = 0; i < 100; i++)
  {
    std::string response;
    std::string cmd;
    std::string data;
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 100));
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(req.receiveReply(response, 100));
  }
}

TEST(ZMQ, test_zmq_send_and_receive_timeout)
{
  ZMQSocket req(ZMQSocket::REQ);
  ZMQSocket rep(ZMQSocket::REP);
  
  ASSERT_TRUE(rep.bind("tcp://127.0.0.1:50000"));
  OSS::thread_sleep(1000);
  ASSERT_TRUE(req.connect("tcp://127.0.0.1:50000"));
  

    std::string response;
    std::string cmd;
    std::string data;
    
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_FALSE(req.receiveReply(response, 100));
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 100));
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 100));
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(req.receiveReply(response, 1000));

}

#else

TEST(NullTest, null_test_zmq_pub_sub){}

#endif