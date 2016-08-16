
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

extern "C" { 
  #include "OSS/RAFT/libraft.h" 
}

#include "OSS/UTL/CoreUtils.h"
#include "OSS/RAFT/RaftServer.h"


namespace OSS {
namespace RAFT {
  
static raft_cbs_t raft_server_funcs;

static void raft_server_seed_random()
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
static int raft_server_send_requestvote(
  raft_server_t* raft,
  void* user_data,
  raft_node_t* node,
  msg_requestvote_t* m)
{
  std::cout << ((RaftServer*)user_data)->opt().node_id <<  "----> raft_server_send_requestvote" << std::endl;
  return 0;
}

//
// Raft callback for sending append entries message
//
static int raft_server_send_appendentries(
  raft_server_t* raft,
  void* user_data,
  raft_node_t* node,
  msg_appendentries_t* m )
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_send_appendentries" << std::endl;
  return 0;
}

//
// Raft callback for applying an entry to the finite state machine
//
static int raft_server_applylog(
  raft_server_t* raft,
  void* user_data,
  raft_entry_t* ety)
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_applylog" << std::endl;
  return 0;
}

//
// Raft callback for saving voted_for field to disk.
// This only returns when change has been made to disk.
//
static int raft_server_persist_vote(
    raft_server_t* raft,
    void *user_data,
    const int voted_for )
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_persist_vote" << std::endl;
  return 0;
}

//
// Raft callback for saving term field to disk.
// This only returns when change has been made to disk. 
//
static int raft_server_persist_term(
  raft_server_t* raft,
  void* user_data,
  const int current_term )
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_persist_term" << std::endl;
  return 0;
}

//
// Raft callback for appending an item to the log
//
static int raft_server_log_offer(
    raft_server_t* raft,
    void* user_data,
    raft_entry_t* ety,
    int ety_idx )
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_log_offer" << std::endl;
  return 0;
}

//
// Raft callback for removing the first entry from the log
// note this is provided to support log compaction in the future
//
static int raft_server_log_poll(
    raft_server_t* raft,
    void* user_data,
    raft_entry_t* entry,
    int ety_idx )
{
  std::cout << ((RaftServer*)user_data)->opt().node_id <<  "----> raft_server_log_poll" << std::endl;
  return 0;
}

//
// Raft callback for deleting the most recent entry from the log.
// This happens when an invalid leader finds a valid leader and has to delete
// superseded log entries. 
//
static int raft_server_log_try_pop(
  raft_server_t* raft,
  void* user_data,
  raft_entry_t* entry,
  int ety_idx)
{
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_log_try_pop" << std::endl;
  return 0;
}

//
// Non-voting node now has enough logs to be able to vote.
// Append a finalization cfg log entry.
//
static void raft_server_node_has_sufficient_logs(
    raft_server_t* raft,
    void *user_data,
    raft_node_t* node)
{ 
  std::cout << ((RaftServer*)user_data)->opt().node_id << "----> raft_server_node_has_sufficient_logs" << std::endl;
}

//
// Raft callback for displaying debugging information
//
static void raft_server_log(
  raft_server_t* raft, 
  raft_node_t* node, 
  void *user_data,
  const char *buf)
{
  std::cout << ((RaftServer*)user_data)->opt().node_id <<  "----> raft_server_log " << buf << std::endl;
}
  
void raft_server_init_func()
{
  static bool init_func = false;
  if (!init_func)
  {
     raft_server_funcs.send_requestvote = raft_server_send_requestvote;
     raft_server_funcs.send_appendentries = raft_server_send_appendentries;
     raft_server_funcs.applylog = raft_server_applylog;
     raft_server_funcs.persist_vote = raft_server_persist_vote;
     raft_server_funcs.persist_term = raft_server_persist_term;
     raft_server_funcs.log_offer = raft_server_log_offer;
     raft_server_funcs.log_poll = raft_server_log_poll;
     raft_server_funcs.log_pop = raft_server_log_try_pop;
     raft_server_funcs.node_has_sufficient_logs = raft_server_node_has_sufficient_logs;
     raft_server_funcs.log = raft_server_log;
  }
}


RaftServer::RaftServer() :
  _raft(0)
{
}

RaftServer::~RaftServer()
{
  if (_raft)
  {
    raft_free(_raft);
  }
}

bool RaftServer::initialize(const Options& options)
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
  raft_server_seed_random();
  
  //
  // Initialize callbacks
  //
  raft_server_init_func();
  raft_set_callbacks(_raft, &raft_server_funcs, this);
  
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

bool RaftServer::addNode(int node_id)
{
  OSS::mutex_critic_sec_lock lock(_raftMutex);
  raft_node_t* node = raft_add_node(_raft, this, node_id, node_id == _opt.node_id);
  return !!(*node);
}

void RaftServer::callPeriodicTimer()
{
  OSS::mutex_critic_sec_lock lock(_raftMutex);
  raft_periodic(_raft, _opt.periodic_timer_ms);
}

void RaftServer::onTerminate()
{
  _sem.set();
}

void RaftServer::main()
{
  while (!_terminateFlag && !_sem.wait(_opt.periodic_timer_ms))
  {
    callPeriodicTimer();
  }
}

void RaftServer::becomeMaster()
{
  OSS::mutex_critic_sec_lock lock(_raftMutex);
  raft_become_leader(_raft);
}

} } // OSS::RAFT


