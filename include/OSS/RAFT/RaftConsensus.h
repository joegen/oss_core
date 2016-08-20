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

#ifndef OSS_RAFTSERVER_H_INCLUDED
#define	OSS_RAFTSERVER_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/UTL/Thread.h"
#include "OSS/RAFT/RaftNode.h"
#include "OSS/RAFT/RaftConnection.h"
#include <map>


namespace OSS {
namespace RAFT {
  
  
#define RAFT_ELECTION_TIMEOUT_MS 2000;
#define RAFT_PERIODIC_TIMER_MS 1000;
  
  
class RaftConsensus : public OSS::Thread
{
public:

  typedef RaftConnection Connection;
  typedef RaftNode Node;
  typedef std::map<int, Connection::Ptr> Connections;
  typedef std::map<int, Node> Nodes;

  struct Options
  {
    Options()
    {
      node_id = 0;
      election_timeout_ms = RAFT_ELECTION_TIMEOUT_MS;
      periodic_timer_ms = RAFT_PERIODIC_TIMER_MS;
      is_master = false;
    }
    int node_id;
    int election_timeout_ms;
    int periodic_timer_ms;
    bool is_master;
  };
    
  RaftConsensus();
  virtual ~RaftConsensus();

  virtual bool initialize(const Options& options);
  
  //
  // Node related
  //
  bool addNode(int node_id);
  bool addNoneVotingNode(int node_id);
  void removeNode(int node_id);
  bool findNode(int node_id, Node& node);
  
  //
  // RAFT Protocol Handlers.  All returns zero when successful
  //
  virtual int onSendRequestVote(Node& node, msg_requestvote_t& data);
  virtual int onSendAppendEntries(Node& node, msg_appendentries_t& data);
  virtual int onReceivedRequestVote(const Connection::Ptr& pConnection, msg_requestvote_t& data);
  virtual int onReceivedAppendEntries(const Connection::Ptr& pConnection, msg_appendentries_t& data);
  virtual int onReceivedRequestVoteResponse(const Connection::Ptr& pConnection, msg_requestvote_response_t& data);
  virtual int onReceivedAppendEntriesResponse(const Connection::Ptr& pConnection, msg_appendentries_response_t& data);
  
  
  virtual int onApplyEntry(const raft_entry_t& entry);
  virtual int onAppendEntry(const raft_entry_t& entry, int index);
  virtual int onPersistVote(int vote);
  virtual int onPersistTerm(int vote);
  virtual void onSufficientLogs(Node& node);

  //
  // Connection related handlers
  //
  virtual Connection::Ptr createConnection(Node& node) = 0;

  const Options& opt() const;

protected:
  virtual void main();
  virtual void onTerminate();
  
  void callPeriodicTimer();
  void becomeMaster();

  Connection::Ptr findConnection(const Node& node);
  Connection::Ptr findConnection(int id);
  Connection::Ptr findOrCreateConnection(Node& node);
  void storeConnection(int id, const Connection::Ptr& pConnection);
  void removeConnection(int id);

private:
  OSS::mutex _raftMutex;
  OSS::mutex_critic_sec _connectionMutex;
  OSS::semaphore _sem;
  raft_server_t* _raft;
  Options _opt;
  Connections _connections;
  Nodes _nodes;
};


//
// Inlines
//

inline const RaftConsensus::Options& RaftConsensus::opt() const
{
  return _opt;
}

inline RaftConsensus::Connection::Ptr RaftConsensus::findConnection(const Node& node)
{
  return findConnection(node.getId());
}


} } // OSS::RAFT

#endif	// OSS_RAFTSERVER_H_INCLUDED

