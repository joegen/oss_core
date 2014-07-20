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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USvoidE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef P2P_OVERLAY_H_INCLUDED
#define	P2P_OVERLAY_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <map>
#include <vector>
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace P2P {

#define OVERLAY_MIN_MSG_TYPE 128

class Overlay : boost::noncopyable
{
public:
  struct Node
  {
    std::string key;
    std::string name;
    OSS::Net::IPAddress hostPort;
  };

  struct UserData
  {
    Overlay* instance;
  };

  typedef std::map<std::string, Node> NodeList;
  typedef boost::function<void(const Node&, bool )> UpdateCallback;
  typedef boost::function<void(const std::string&, int, const std::string&)> MessageCallback;
  typedef std::map<int, MessageCallback> MessageCallbackList;
  typedef std::vector<std::string> LeafSet;

  Overlay();
  
  ~Overlay();

  bool init(unsigned short port, int gracePeriod, int pingInterval);
  bool init(unsigned short port);
    // Create an overlay listening on a the specified port
    // gracePeriod: Number of seconds elapsed before a failed node can be accepted back to the overlay
    // pingInterval: Number of seconds the ping thread will check health of the leafset participants
    //
  bool join(const OSS::Net::IPAddress& bootstrapHost);
    // Join an overlay with bootstrap host
    //
  void onUpdate(const Node& node, bool joined);
    // Handler called when a host joined or left the overlay
    //
  void setUpdateHandler(const UpdateCallback& cb);
    // Set the onUpdateHandler
    //
  void onMessage(const std::string& messageKey, int messageType, const std::string& payload);
    // Handler called when a new message has arrived from the overlay
    //
  bool registerMessageType(int messageType, bool sendAck, MessageCallback cb);
    // Register a message type.  Any type that is not registered will be dropped.
    // messageType MUST be greater or equal to OVERLAY_MIN_MSG_TYPE
    //
  bool sendMessage(int messageType, const std::string& key, const std::string& payload);
    // Send a message to the nearest endpoint identified by key
    //
  void getNodeList(NodeList& nodes) const;
    // Returns a reference to node lists
    //
  void getHostList(NodeList& nodes) const;
    // Returns a reference to host lists
    //
  bool getNode(const std::string& nodeId, Node& node) const;
    // Returns the node element identified by nodeId
    //
  bool getHost(const OSS::Net::IPAddress& hostPort, Node& node) const;
    // Returns the node element identified by host:port
    //
  const std::string& getNodeId() const;
    // Returns the local node id
    //
  bool getRightLeafSet(LeafSet& leafset);
  bool getRightLeafSet(LeafSet& leafset, int maxNodes) const;
    // Returns the id collection of nodes to the right of the overlay

private:
  UpdateCallback _updateCb;
  MessageCallbackList _messageCbList;
  NodeList _nodeList;
  mutable OSS::mutex_read_write _nodeListMutex;
  NodeList _hostList;
  mutable OSS::mutex_read_write _hostListMutex;
  OSS_HANDLE _state;
  UserData _userData;
  std::string _nodeId;
};


//
// Inlines
//
inline void Overlay::setUpdateHandler(const UpdateCallback& cb)
{
  _updateCb = cb;
}

inline const std::string& Overlay::getNodeId() const
{
  return _nodeId;
}

inline bool Overlay::getRightLeafSet(LeafSet& leafset)
{
  return getRightLeafSet(leafset, INT_MAX);
}


} }


#endif	//P2P_OVERLAY_H_INCLUDED

