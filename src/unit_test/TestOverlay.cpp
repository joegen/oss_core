
#include "gtest/gtest.h"
#include "OSS/P2P/Overlay.h"
#include "OSS/Thread.h"

using namespace OSS::P2P;

Overlay boot1;
Overlay node1;
Overlay node2;
Overlay node3;

#define HOST_IP "127.0.0.1"
#define BOOTSTRAP_PORT 7000
#define NODE1_PORT 7001
#define NODE2_PORT 7002
#define NODE3_PORT 7003
#define MSG_TYPE OVERLAY_MIN_MSG_TYPE

bool node1GotIt = false;
bool node2GotIt = false;
bool node3GotIt = false;

void update_handler_boot1(const Overlay::Node& info, bool joined)
{
#if 0
  std::cout << "update_handler_boot1 - info.name: " << info.name 
    << " info.key: " << info.key
    << " info.hostPort: " << info.hostPort
    << " joined: " << joined << std::endl;
#endif
}

void update_handler_node1(const Overlay::Node& info, bool joined)
{
#if 0
  std::cout << "update_handler_node1 - info.name: " << info.name
    << " info.key: " << info.key
    << " info.hostPort: " << info.hostPort
    << " joined: " << joined << std::endl;
#endif
}

void update_handler_node2(const Overlay::Node& info, bool joined)
{
#if 0
  std::cout << "update_handler_node2 - info.name: " << info.name
    << " info.key: " << info.key
    << " info.hostPort: " << info.hostPort
    << " joined: " << joined << std::endl;
#endif
}

void update_handler_node3(const Overlay::Node& info, bool joined)
{
#if 0
  std::cout << "update_handler_node3 - info.name: " << info.name
    << " info.key: " << info.key
    << " info.hostPort: " << info.hostPort
    << " joined: " << joined << std::endl;
#endif
}

void on_handle_message_node1(const std::string& messageKey, int messageType, const std::string& payload)
{
  node1GotIt = true;
#if 0
  std::cout << "on_handle_message_node1 - messageKey=" << messageKey << " messageType=" << messageType << " payload=" << payload << std::endl;
#endif
}

void on_handle_message_node2(const std::string& messageKey, int messageType, const std::string& payload)
{
  node2GotIt = true;
#if 0
  std::cout << "on_handle_message_node2 - messageKey=" << messageKey << " messageType=" << messageType << " payload=" << payload << std::endl;
#endif
}

void on_handle_message_node3(const std::string& messageKey, int messageType, const std::string& payload)
{
  node3GotIt = true;
#if 0
  std::cout << "on_handle_message_node2 - messageKey=" << messageKey << " messageType=" << messageType << " payload=" << payload << std::endl;
#endif
}

TEST(OverlayTest, overlay_init)
{
  ASSERT_TRUE(boot1.init(BOOTSTRAP_PORT));
  ASSERT_TRUE(node1.init(NODE1_PORT));
  ASSERT_TRUE(node2.init(NODE2_PORT));
  ASSERT_TRUE(node3.init(NODE3_PORT));

  ASSERT_TRUE(node1.registerMessageType(MSG_TYPE, true, boost::bind(on_handle_message_node1, _1, _2, _3)));
  ASSERT_TRUE(node2.registerMessageType(MSG_TYPE, true, boost::bind(on_handle_message_node2, _1, _2, _3)));
  ASSERT_TRUE(node3.registerMessageType(MSG_TYPE, true, boost::bind(on_handle_message_node3, _1, _2, _3)));
}

TEST(OverlayTest, overlay_join)
{
  OSS::IPAddress host(HOST_IP, BOOTSTRAP_PORT);

  boot1.setUpdateHandler(boost::bind(update_handler_boot1, _1, 2));
  node1.setUpdateHandler(boost::bind(update_handler_node1, _1, 2));
  node2.setUpdateHandler(boost::bind(update_handler_node2, _1, 2));
  node3.setUpdateHandler(boost::bind(update_handler_node3, _1, 2));

  ASSERT_TRUE(boot1.join(host));
  ASSERT_TRUE(node1.join(host));
  ASSERT_TRUE(node2.join(host));
  ASSERT_TRUE(node3.join(host));

  OSS::thread_sleep(500);

  Overlay::NodeList members;
  bool fb1 = false;
  bool fn1 = false;
  bool fn2 = false;
  bool fn3 = false;

  //
  // Check boot 1 if node 1...3 are all registered
  //
  boot1.getNodeList(members);
  for (Overlay::NodeList::const_iterator iter = members.begin(); iter != members.end(); iter++)
  {
    if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == BOOTSTRAP_PORT)
      fb1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE1_PORT)
      fn1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE2_PORT)
      fn2 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE3_PORT)
      fn3 = true;
  }

  ASSERT_TRUE(fn1);
  ASSERT_TRUE(fn2);
  ASSERT_TRUE(fn3);

  fb1 = false;
  fn1 = false;
  fn2 = false;
  fn3 = false;

  //
  // Check node 1 if nodes are all registered
  //
  node1.getNodeList(members);
  for (Overlay::NodeList::const_iterator iter = members.begin(); iter != members.end(); iter++)
  {
    if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == BOOTSTRAP_PORT)
      fb1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE1_PORT)
      fn1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE2_PORT)
      fn2 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE3_PORT)
      fn3 = true;
  }

  ASSERT_TRUE(fb1);
  ASSERT_TRUE(fn2);
  ASSERT_TRUE(fn3);

  fb1 = false;
  fn1 = false;
  fn2 = false;
  fn3 = false;

  //
  // Check node 2 if nodes are all registered
  //
  node2.getNodeList(members);
  for (Overlay::NodeList::const_iterator iter = members.begin(); iter != members.end(); iter++)
  {
    if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == BOOTSTRAP_PORT)
      fb1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE1_PORT)
      fn1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE2_PORT)
      fn2 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE3_PORT)
      fn3 = true;
  }

  ASSERT_TRUE(fb1);
  ASSERT_TRUE(fn1);
  ASSERT_TRUE(fn3);

  fb1 = false;
  fn1 = false;
  fn2 = false;
  fn3 = false;
  
  //
  // Check node 3 if nodes are all registered
  //
  node3.getNodeList(members);
  for (Overlay::NodeList::const_iterator iter = members.begin(); iter != members.end(); iter++)
  {
    if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == BOOTSTRAP_PORT)
      fb1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE1_PORT)
      fn1 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE2_PORT)
      fn2 = true;
    else if (iter->second.hostPort.toString() == HOST_IP &&  iter->second.hostPort.getPort() == NODE3_PORT)
      fn3 = true;
  }

  ASSERT_TRUE(fb1);
  ASSERT_TRUE(fn1);
  ASSERT_TRUE(fn2);

  Overlay::LeafSet leafSet;
  node1.getRightLeafSet(leafSet);
  ASSERT_EQ(leafSet.size(), 3);

  node2.getRightLeafSet(leafSet);
  ASSERT_EQ(leafSet.size(), 3);

  node3.getRightLeafSet(leafSet);
  ASSERT_EQ(leafSet.size(), 3);
}

TEST(OverlayTest, overlay_msg_broad_cast)
{
  Overlay::LeafSet leafset;
  boot1.getRightLeafSet(leafset);
  ASSERT_EQ(leafset.size(), 3);

  for (int i = 0; i < 3; i++)
    ASSERT_TRUE(node1.sendMessage(MSG_TYPE, leafset[i], "Hello!"));

  OSS::thread_sleep(100);

  ASSERT_TRUE(node1GotIt);
  ASSERT_TRUE(node2GotIt);
  ASSERT_TRUE(node3GotIt);
}

