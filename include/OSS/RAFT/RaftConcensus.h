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


namespace OSS {
namespace RAFT {
  
  
#define RAFT_ELECTION_TIMEOUT_MS 2000;
#define RAFT_PERIODIC_TIMER_MS 1000;
  
  
class RaftConcensus : public OSS::Thread
{
public:
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
  // Callbacks.  All returns zero when successful
  //
  virtual int onSendRequestVote(const RaftNode& node, const msg_requestvote_t& data) = 0;
  virtual int onSendAppendEntries(const RaftNode& node, const msg_appendentries_t& data) = 0;
  virtual int onApplyEntry(const raft_entry_t& entry) = 0;
  virtual int onAppendEntry(const raft_entry_t& entry, int index) = 0;
  virtual int onPersistVote(int vote) = 0;
  virtual int onPersistTerm(int vote) = 0;
  virtual void onSufficientLogs(const RaftNode& node) = 0;
  
  const Options& opt() const;
protected:
  virtual void main();
  virtual void onTerminate();
  
  void callPeriodicTimer();
  void becomeMaster();

private:
  OSS::mutex_critic_sec _raftMutex;
  OSS::semaphore _sem;
  raft_server_t* _raft;
  Options _opt;
};


//
// Inlines
//

inline const RaftConcensus::Options& RaftConcensus::opt() const
{
  return _opt;
}

} } // OSS::RAFT

#endif	// OSS_RAFTSERVER_H_INCLUDED

