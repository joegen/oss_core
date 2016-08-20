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

#include "OSS/RAFT/RaftConsensus.h"


using OSS::RAFT::RaftConsensus;
using OSS::RAFT::RaftNode;
using OSS::RAFT::RaftConnection;

class Server;

class MyConnection : public RaftConnection
{
public:
  Server* _pServer;
  
  MyConnection(Server* server, RaftNode& node);
  void shutdown();
  int onSendRequestVote(msg_requestvote_t& data);
  int onSendAppendEntries( msg_appendentries_t& data);
  int onSendRequestVoteResponse(msg_requestvote_response_t& data);
  int onSendAppendEntriesResponse(msg_appendentries_response_t& data);
};

class Server : public RaftConsensus
{
public:
  Server();
  Connection::Ptr createConnection(Node& node);
  Connection::Ptr _pConnection;
  Server* _peer;
};


Server::Server() :
  _peer(0)
{
}

RaftConsensus::Connection::Ptr Server::createConnection(Node& node)
{
  return _pConnection;
}

  
MyConnection::MyConnection(Server* server, RaftNode& node) :
  RaftConnection((RaftConsensus*)server, node),
  _pServer(server)
{ 
}
void MyConnection::shutdown() 
{
}

int MyConnection::onSendRequestVote(msg_requestvote_t& data) 
{ 
  std::cout << "MyConnection::onSendRequestVote node.id=" << _node.getId() << std::endl;
  _pServer->_peer->onReceivedRequestVote(_pServer->_pConnection, data);
  return 0; 
}

int MyConnection::onSendAppendEntries( msg_appendentries_t& data) 
{ 
  std::cout << "MyConnection::onSendAppendEntries node.id=" << _node.getId() << std::endl;
  _pServer->_peer->onReceivedAppendEntries(_pServer->_pConnection, data);
  return 0; 
}

int MyConnection::onSendRequestVoteResponse(msg_requestvote_response_t& data) 
{ 
  std::cout << "MyConnection::onSendRequestVoteResponse node.id=" << _node.getId() << std::endl;
  _pServer->_peer->onReceivedRequestVoteResponse(_pServer->_peer->_pConnection, data);
  return 0; 
}

int MyConnection::onSendAppendEntriesResponse(msg_appendentries_response_t& data) 
{ 
  std::cout << "MyConnection::onSendAppendEntriesResponse node.id=" << _node.getId() << std::endl;
  _pServer->_peer->onReceivedAppendEntriesResponse(_pServer->_peer->_pConnection, data);
  return 0; 
}


TEST(RAFTTest, TestRaftConsensus)
{
  Server server; 
  Server member;
  RaftConsensus::Options sopt, mopt;
  
  sopt.is_master = true;
  sopt.node_id = 1;
  
  mopt.is_master = false;
  mopt.node_id = 2;
  
  server._peer = &member;
  member._peer = &server;
  
  ASSERT_TRUE(server.initialize(sopt));
  ASSERT_TRUE(member.initialize(mopt));
  
  ASSERT_TRUE(server.addNode(mopt.node_id));
  RaftNode node;
  ASSERT_TRUE(server.findNode(mopt.node_id, node));
  server._pConnection = RaftConsensus::Connection::Ptr(new MyConnection(&server, node));
  
  ASSERT_TRUE(member.addNode(sopt.node_id));
  ASSERT_TRUE(member.findNode(sopt.node_id, node));
  member._pConnection = RaftConsensus::Connection::Ptr(new MyConnection(&member, node));
  
  
  member.run();
  server.run();
  
  OSS::thread_sleep(5000);
  server.stop();
  member.stop();
  
}

