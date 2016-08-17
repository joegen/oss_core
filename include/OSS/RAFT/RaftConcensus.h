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
  
  
class RaftConcensus : public OSS::Thread
{
public:

  typedef RaftConnection Connection;
  typedef RaftNode Node;
  typedef std::map<int, Connection::Ptr> Connections;

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
    
  RaftConcensus();
  virtual ~RaftConcensus();

  virtual bool initialize(const Options& options);
  bool addNode(int node_id);
  
  //
  // RAFT Protocol Handlers.  All returns zero when successful
  //
  virtual int onSendRequestVote(Node& node, const msg_requestvote_t& data);
  virtual int onSendAppendEntries(Node& node, const msg_appendentries_t& data);
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
  OSS::mutex_critic_sec _raftMutex;
  OSS::mutex_critic_sec _connectionMutex;
  OSS::semaphore _sem;
  raft_server_t* _raft;
  Options _opt;
  Connections _connections;
};


//
// Inlines
//

inline const RaftConcensus::Options& RaftConcensus::opt() const
{
  return _opt;
}

inline RaftConcensus::Connection::Ptr RaftConcensus::findConnection(const Node& node)
{
  return findConnection(node.getId());
}


} } // OSS::RAFT

#endif	// OSS_RAFTSERVER_H_INCLUDED

