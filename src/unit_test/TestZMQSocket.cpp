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
  
  //
  // Test timeout
  //
  {
    std::string response;
    std::string cmd;
    std::string data;
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_FALSE(req.receiveReply(response, 2));
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 2));
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 2));
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(req.receiveReply(response, 2));
  }
  
  //
  // Test Poll
  //
  {
    std::string response;
    std::string cmd;
    std::string data;
    
    ZMQSocket::PollItem reqItem, repItem;
    ZMQSocket::PollItems reqItems, repItems;
    
    reqItem.events = ZMQ_POLLIN;
    reqItem.socket = *req.socket();
    reqItems.push_back(reqItem);
    
    repItem.events = ZMQ_POLLIN;
    repItem.socket = *rep.socket();
    repItems.push_back(repItem);
    
    ASSERT_TRUE(req.sendRequest("test", "data"));
    ASSERT_TRUE(ZMQSocket::poll(repItems, 2) > 0);
    ASSERT_TRUE(repItems[0].revents & ZMQ_POLLIN);
    ASSERT_TRUE(rep.receiveRequest(cmd, data, 0));
    
    ASSERT_TRUE(rep.sendReply("reply"));
    ASSERT_TRUE(ZMQSocket::poll(reqItems, 2) > 0);
    ASSERT_TRUE(reqItems[0].revents & ZMQ_POLLIN);
    ASSERT_TRUE(req.receiveReply(response, 0));
  }
  
  //
  // TEst PUB/SUB
  //
  {
    ZMQSocket publisher(ZMQSocket::PUB);
    ZMQSocket subscriber1(ZMQSocket::SUB);
    ZMQSocket subscriber2(ZMQSocket::SUB);
    ASSERT_TRUE(publisher.bind("tcp://127.0.0.1:50001"));
    OSS::thread_sleep(100);
    ASSERT_TRUE(subscriber1.connect("tcp://127.0.0.1:50001"));
    ASSERT_TRUE(subscriber2.connect("tcp://127.0.0.1:50001"));
    OSS::thread_sleep(100);
    ASSERT_TRUE(subscriber1.subscribe("test-event"));
    ASSERT_TRUE(subscriber2.subscribe("test-event"));
    OSS::thread_sleep(100);
    ASSERT_TRUE(publisher.publish("test-event event-data"));
    OSS::thread_sleep(100);
    std::string event;
    ASSERT_TRUE(subscriber1.receiveReply(event, 0));
    ASSERT_STREQ(event.c_str(), "test-event event-data");
    event = "";
    ASSERT_TRUE(subscriber2.receiveReply(event, 0));
    ASSERT_STREQ(event.c_str(), "test-event event-data");
  }
}


TEST(ZMQ, test_zmq_send_and_receive_inproc)
{
  ZMQSocket req(ZMQSocket::REQ);
  ZMQSocket rep(ZMQSocket::REP);
  
  ASSERT_TRUE(rep.bind("inproc://#1"));
  OSS::thread_sleep(1000);
  ASSERT_TRUE(req.connect("inproc://#1"));
  OSS::thread_sleep(1000);
  
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

#else

TEST(NullTest, null_test_zmq_pub_sub){}

#endif