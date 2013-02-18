
// Library: OSS Software Solutions Application Programmer Interface
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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

//
// WebSocket++ has the following license :
//

/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef SIPWEBSOCKETCONNECTIONMANAGER_H
#define	SIPWEBSOCKETCONNECTIONMANAGER_H

#include <set>
#include <map>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "SIPWebSocketConnection.h"
#include "OSS/Thread.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class SIPWebSocketConnectionManager: private boost::noncopyable
  /// Manages open connections so that they may be cleanly stopped when the server
  /// needs to shut down.
{
public:
  SIPWebSocketConnectionManager(SIPFSMDispatch* pDispatch);
    /// Creates a new connection manager object.

  ~SIPWebSocketConnectionManager();
    /// Destroys the connection manager.

  void initialize(const boost::filesystem::path& cfgDirectory);
    /// Initialize the manager configuration using the configuration path specified.
    /// If an error occurs, this method will throw a PersistenceException.
    ///
    /// Take note that the configuration directory must be both readable and writeble
    /// by the user that owns the process.

  void deinitialize();
    /// Deinitialize the manager.  This is usually called when the application
    /// is about the exit.  This is the place where the manager performs final
    /// trash management.

  void add(SIPWebSocketConnection::Ptr conn);
   /// Add the specified connection to the manager

  void start(SIPWebSocketConnection::Ptr conn);
    /// Add the specified connection to the manager and start it.

  void stop(SIPWebSocketConnection::Ptr conn);
    /// Stop the specified connection.

  void stopAll();
    /// Stop all connections.

  unsigned short getPortBase() const;
    /// Return the port base to be used for TCP client connections

  void setPortBase(unsigned short port);
    /// Set the port base.  the default is 10000

  unsigned short getPortMax() const;
    /// Return the port max to be used for TCP client connections

  void setPortmax(unsigned short port);
    /// Set the port max.  the default is 12000

  SIPWebSocketConnection::Ptr findConnectionByAddress(const OSS::IPAddress& target);
    /// Find a connection to a specific target if it exists

  SIPWebSocketConnection::Ptr findConnectionById(OSS::UInt64 identifier);

  SIPWebSocketConnection::Ptr findConnectionByPtr(websocketpp::server::connection_ptr ptr);
private:
  OSS::mutex_read_write _rwConnectionsMutex;
  std::map<OSS::UInt64, SIPWebSocketConnection::Ptr> _connections;
  SIPFSMDispatch* _pDispatch;
  unsigned short _portBase;
  unsigned short _portMax;
};


//
// Inlines
//

inline unsigned short SIPWebSocketConnectionManager::getPortBase() const
{
  return _portBase;
}

inline void SIPWebSocketConnectionManager::setPortBase(unsigned short port)
{
  _portBase = port;
}

inline unsigned short SIPWebSocketConnectionManager::getPortMax() const
{
  return _portMax;
}

inline void SIPWebSocketConnectionManager::setPortmax(unsigned short port)
{
  _portMax = port;
}

inline SIPWebSocketConnection::Ptr SIPWebSocketConnectionManager::findConnectionByPtr(websocketpp::server::connection_ptr ptr)
{
  return findConnectionById((OSS::UInt64)ptr.get());
}


} } // OSS::SIP

#endif	/* SIPWEBSOCKETCONNECTIONMANAGER_H */

