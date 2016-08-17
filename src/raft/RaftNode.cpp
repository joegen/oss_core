
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

#include "OSS/RAFT/RaftNode.h"


namespace OSS {
namespace RAFT {

RaftNode::RaftNode() :
  _node(0)
{
}

RaftNode::RaftNode(raft_node_t* node) :
  _node(node)
{
}

RaftNode::RaftNode(const RaftNode& node)
{
  _node = node._node;
}

RaftNode::~RaftNode()
{
}

RaftNode& RaftNode::operator=(const RaftNode& node)
{
  _node = node._node;
  return *this;
}

RaftNode& RaftNode::operator=(raft_node_t* node)
{
  _node = node;
  return *this;
}

int RaftNode::getId() const
{
  if (!_node)
  {
    return -1;
  }
  return raft_node_get_id(_node);
}

int RaftNode::getNextIndex() const
{
  if (!_node)
  {
    return -1;
  }
  return raft_node_get_next_idx(_node);
}

int RaftNode::getMatchIndex() const
{
  if (!_node)
  {
    return -1;
  }
  return raft_node_get_match_idx(_node);
}

void* RaftNode::getUserData() const
{
  if (!_node)
  {
    return 0;
  }
  return raft_node_get_udata(_node);
}

void RaftNode::setUserData(void* userData)
{
  if (_node)
  {
    raft_node_set_udata(_node, userData);
  }
}


  

} } // OSS::RAFT


