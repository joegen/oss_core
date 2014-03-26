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
#include "OSS/Core.h"
#include "OSS/IPAddress.h"


namespace OSS {
namespace P2P {

class Overlay : boost::noncopyable
{
public:
  struct Node
  {
    std::string key;
    std::string name;
    OSS::IPAddress hostPort;
  };

  struct UserData
  {
    Overlay* instance;
  };

  typedef std::map<std::string, Node> NodeList;
  typedef boost::function<void(const Node&, bool )> UpdateCallback;
  typedef boost::function<void(const std::string&, const std::string&, int, const std::string&)> MessageCallback;
  typedef std::map<int, MessageCallback> MessageCallbackList;

  Overlay();
  
  ~Overlay();

  bool init(unsigned short port);
    // Create an overlay listening on a the specified port
    //
  bool join(const OSS::IPAddress& bootstrapHost);
    // Join an overlay with bootstrap host
    //
  bool setLocalKey(const std::string& key);
    // Set the local identifier for this node
    //
  void onUpdate(const Node& node, bool joined);
    // Handler called when a host joined or left the overlay
    //
  void setUpdateHandler(const UpdateCallback& cb);
    // Set the onUpdateHandler
    //
  void onMessage(const std::string& senderKey, const std::string& messageKey, int messageType, const std::string& payload);
    // Handler called when a new message has arrived from the overlay
    //
  bool registerMessageType(int messageType, bool sendAck, MessageCallback cb);
    // Register a message type.  Any type that is not registered will be dropped
    //
  bool sendMessage(int messageType, const std::string& key, const std::string& payload);
    // Send a message to the nearest endpoint identified by key
    //
private:
  UpdateCallback _updateCb;
  MessageCallbackList _messageCbList;
  NodeList _nodeList;
  OSS_HANDLE _state;
  UserData _userData;
};


//
// Inlines
//
inline void Overlay::setUpdateHandler(const UpdateCallback& cb)
{
  _updateCb = cb;
}


} }


#endif	//P2P_OVERLAY_H_INCLUDED

