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


#ifndef OSS_RAFTCONNECTION_H_INCLUDED
#define OSS_RAFTCONNECTION_H_INCLUDED


#include "OSS/OSS.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

extern "C"
{
  #include "OSS/RAFT/libraft.h"
};


namespace OSS {
namespace RAFT {

class RaftConsensus;

class RaftConnection
{
public:
  typedef boost::shared_ptr<RaftConnection> Ptr;
  
  RaftConnection(RaftConsensus* pRaft, RaftNode& node);
  virtual ~RaftConnection();

  virtual void shutdown() = 0;
  virtual int onSendRequestVote(msg_requestvote_t& data) = 0;
  virtual int onSendAppendEntries(msg_appendentries_t& data) = 0;
  virtual int onSendRequestVoteResponse(msg_requestvote_response_t& data) = 0;
  virtual int onSendAppendEntriesResponse(msg_appendentries_response_t& data) = 0;

  RaftNode& getNode();
  const RaftNode& getNode() const;

protected:
  RaftConsensus* _pRaft;
  RaftNode _node;
};


//
// Inlines
//
inline RaftNode& RaftConnection::getNode()
{
  return _node;
}

inline const RaftNode& RaftConnection::getNode() const
{
  return _node;
}

} } // OSS::RAFT


#endif // OSS_RAFTCONNECTION_H_INCLUDED

