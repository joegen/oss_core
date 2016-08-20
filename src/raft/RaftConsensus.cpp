
// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include "OSS/UTL/CoreUtils.h"
#include "OSS/RAFT/RaftConsensus.h"


namespace OSS {
namespace RAFT {
  
static raft_cbs_t rc_funcs;

typedef RaftConsensus::Node Node;

static void rc_seed_random()
{
  static bool seeded = false;
  if (!seeded)
  {
    seeded = true;
    srand( (unsigned int)time( 0 ) + (unsigned int)getTicks() );
  }
}
//
// Raft callback for sending request vote message
//
static int rc_send_requestvote(
  raft_server_t* raft,
  void* user_data,
  raft_node_t* node,
  msg_requestvote_t* m)
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id <<  "----> rc_send_requestvote" << std::endl;
  Node raftNode(node);
  return ((RaftConsensus*)user_data)->onSendRequestVote(raftNode, *m);
}

//
// Raft callback for sending append entries message
//
static int rc_send_appendentries(
  raft_server_t* raft,
  void* user_data,
  raft_node_t* node,
  msg_appendentries_t* m)
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_send_appendentries" << std::endl;
  Node raftNode(node);
  return ((RaftConsensus*)user_data)->onSendAppendEntries(raftNode, *m);
}

//
// Raft callback for applying an entry to the finite state machine
//
static int rc_applylog(
  raft_server_t* raft,
  void* user_data,
  raft_entry_t* ety)
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_applylog" << std::endl;
  return ((RaftConsensus*)user_data)->onApplyEntry(*ety);
}

//
// Raft callback for saving voted_for field to disk.
// This only returns when change has been made to disk.
//
static int rc_persist_vote(
    raft_server_t* raft,
    void *user_data,
    const int voted_for )
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_persist_vote" << std::endl;
  return ((RaftConsensus*)user_data)->onPersistVote(voted_for);
}

//
// Raft callback for saving term field to disk.
// This only returns when change has been made to disk. 
//
static int rc_persist_term(
  raft_server_t* raft,
  void* user_data,
  const int current_term )
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_persist_term" << std::endl;
  return ((RaftConsensus*)user_data)->onPersistTerm(current_term);
}

//
// Raft callback for appending an item to the log
//
static int rc_log_offer(
    raft_server_t* raft,
    void* user_data,
    raft_entry_t* ety,
    int ety_idx )
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_log_offer" << std::endl;
  return ((RaftConsensus*)user_data)->onAppendEntry(*ety, ety_idx);
}

//
// Non-voting node now has enough logs to be able to vote.
// Append a finalization cfg log entry.
//
static void rc_node_has_sufficient_logs(
    raft_server_t* raft,
    void *user_data,
    raft_node_t* node)
{ 
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_node_has_sufficient_logs" << std::endl;
  Node raftNode(node);
  ((RaftConsensus*)user_data)->onSufficientLogs(raftNode);
}

//
// Raft callback for removing the first entry from the log
// note this is provided to support log compaction in the future
//
static int rc_log_poll(
    raft_server_t* raft,
    void* user_data,
    raft_entry_t* entry,
    int ety_idx )
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id <<  "----> rc_log_poll" << std::endl;
  return 0;
}

//
// Raft callback for deleting the most recent entry from the log.
// This happens when an invalid leader finds a valid leader and has to delete
// superseded log entries. 
//
static int rc_log_try_pop(
  raft_server_t* raft,
  void* user_data,
  raft_entry_t* entry,
  int ety_idx)
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id << "----> rc_log_try_pop" << std::endl;
  return 0;
}

//
// Raft callback for displaying debugging information
//
static void rc_log(
  raft_server_t* raft, 
  raft_node_t* node, 
  void *user_data,
  const char *buf)
{
  std::cout << ((RaftConsensus*)user_data)->opt().node_id <<  "----> rc_log " << buf << std::endl;
}
  
void rc_init_func()
{
  static bool init_func = false;
  if (!init_func)
  {
     rc_funcs.send_requestvote = rc_send_requestvote;
     rc_funcs.send_appendentries = rc_send_appendentries;
     rc_funcs.applylog = rc_applylog;
     rc_funcs.persist_vote = rc_persist_vote;
     rc_funcs.persist_term = rc_persist_term;
     rc_funcs.log_offer = rc_log_offer;
     rc_funcs.log_poll = rc_log_poll;
     rc_funcs.log_pop = rc_log_try_pop;
     rc_funcs.node_has_sufficient_logs = rc_node_has_sufficient_logs;
     rc_funcs.log = rc_log;
  }
}


RaftConsensus::RaftConsensus() :
  _raft(0)
{
}

RaftConsensus::~RaftConsensus()
{
  if (_raft)
  {
    raft_free(_raft);
  }
}

bool RaftConsensus::initialize(const Options& options)
{
  if (_raft)
  {
    return true;
  }
  
  _raft = raft_new();
  if (!_raft)
  {
    return false;
  }
  
  //
  // Seed the random number generator
  //
  rc_seed_random();
  
  //
  // Initialize callbacks
  //
  rc_init_func();
  raft_set_callbacks(_raft, &rc_funcs, this);
  
  //
  // add self
  //
  _opt = options;
  if (!addNode(_opt.node_id))
  {
    return false;
  }
  
  //
  // Start as master
  //
  if (_opt.is_master)
  {
    becomeMaster();
  }
  
  //
  // Set the election timeout
  //
  raft_set_election_timeout(_raft, _opt.election_timeout_ms);
   
  return true;
}

bool RaftConsensus::addNode(int node_id)
{
  OSS::mutex_lock lock(_raftMutex);
  raft_node_t* node = raft_add_node(_raft, this, node_id, node_id == _opt.node_id);
  if (node)
  {
    _nodes[node_id] = Node(node);
  }
  return !!(*node);
}

bool RaftConsensus::addNoneVotingNode(int node_id)
{
  OSS::mutex_lock lock(_raftMutex);
  raft_node_t* node = raft_add_non_voting_node(_raft, this, node_id, node_id == _opt.node_id);
  if (node)
  {
    _nodes[node_id] = Node(node);
  }
  return !!(*node);
}

void RaftConsensus::removeNode(int node_id)
{
  OSS::mutex_lock lock(_raftMutex);
  Nodes::iterator iter = _nodes.find(node_id);
  if (iter != _nodes.end())
  {
    raft_remove_node(_raft, iter->second.node());
    _nodes.erase(node_id);
  }
}

bool RaftConsensus::findNode(int node_id, Node& node)
{
  OSS::mutex_lock lock(_raftMutex);
  Nodes::iterator iter = _nodes.find(node_id);
  if (iter != _nodes.end())
  {
    node = iter->second;
    return true;
  }
  return false;
}
  
void RaftConsensus::callPeriodicTimer()
{
  OSS::mutex_lock lock(_raftMutex);
  raft_periodic(_raft, _opt.periodic_timer_ms);
}

void RaftConsensus::onTerminate()
{
  _sem.set();
}

void RaftConsensus::main()
{
  while (!_terminateFlag && !_sem.wait(_opt.periodic_timer_ms))
  {
    callPeriodicTimer();
  }
}

void RaftConsensus::becomeMaster()
{
  OSS::mutex_lock lock(_raftMutex);
  raft_become_leader(_raft);
}

int RaftConsensus::onSendRequestVote(Node& node, msg_requestvote_t& data)
{
  Connection::Ptr pConnection = findOrCreateConnection(node);
  if (pConnection)
  {
    return pConnection->onSendRequestVote(data);
  }
  return -1;
}

int RaftConsensus::onSendAppendEntries(Node& node, msg_appendentries_t& data)
{
  Connection::Ptr pConnection = findOrCreateConnection(node);
  if (pConnection)
  {
    return pConnection->onSendAppendEntries(data);
  }
  return -1;
}

int RaftConsensus::onReceivedRequestVote(const Connection::Ptr& pConnection, msg_requestvote_t& data)
{
  OSS::mutex_lock lock(_raftMutex);
  int ret = 0;
  msg_requestvote_response_t response;
  ret = raft_recv_requestvote(_raft, pConnection->getNode().node(), &data, &response);
  if (ret != 0)
  {
    return ret;
  }
  return pConnection->onSendRequestVoteResponse(response);
}

int RaftConsensus::onReceivedAppendEntries(const Connection::Ptr& pConnection, msg_appendentries_t& data)
{
  OSS::mutex_lock lock(_raftMutex);
  
  int ret = 0;
  msg_appendentries_response_t response;
  ret = raft_recv_appendentries(_raft, pConnection->getNode().node(), &data, &response);
  if (ret != 0)
  {
    return ret;
  }
  return pConnection->onSendAppendEntriesResponse(response);
}

int RaftConsensus::onReceivedRequestVoteResponse(const Connection::Ptr& pConnection, msg_requestvote_response_t& data)
{
  OSS::mutex_lock lock(_raftMutex);
  return raft_recv_requestvote_response(_raft, pConnection->getNode().node(), &data);
}
int RaftConsensus::onReceivedAppendEntriesResponse(const Connection::Ptr& pConnection, msg_appendentries_response_t& data)
{
  OSS::mutex_lock lock(_raftMutex);
  return raft_recv_appendentries_response(_raft, pConnection->getNode().node(), &data);
}

int RaftConsensus::onApplyEntry(const raft_entry_t& entry)
{
  return 0;
}

int RaftConsensus::onAppendEntry(const raft_entry_t& entry, int index)
{
  return 0;
}

int RaftConsensus::onPersistVote(int vote)
{
  return 0;
}

int RaftConsensus::onPersistTerm(int vote)
{
  return 0;
}

void RaftConsensus::onSufficientLogs(Node& node)
{
}


RaftConsensus::Connection::Ptr RaftConsensus::findConnection(int id)
{
  OSS::mutex_critic_sec_lock lock(_connectionMutex);
  Connection::Ptr pConnection;
  Connections::iterator iter = _connections.find(id);
  if ( iter != _connections.end())
  {
    pConnection = iter->second;
  }
  return pConnection;
}

RaftConsensus::Connection::Ptr RaftConsensus::findOrCreateConnection(Node& node)
{
  Connection::Ptr pConnection;
  pConnection = findConnection(node.getId());
  if (!pConnection)
  {
    pConnection = createConnection(node);
    if (pConnection)
    {
      storeConnection(node.getId(), pConnection);
    }
  }
  return pConnection;
}

void RaftConsensus::storeConnection(int id, const Connection::Ptr& pConnection)
{
  OSS::mutex_critic_sec_lock lock(_connectionMutex);
  _connections[id] = pConnection;
}

void RaftConsensus::removeConnection(int id)
{
  Connection::Ptr pConnection;
  pConnection = findConnection(id);
  if (!pConnection)
  {
    pConnection->shutdown();
    OSS::mutex_critic_sec_lock lock(_connectionMutex);
    _connections.erase(id);
  }
}

} } // OSS::RAFT


