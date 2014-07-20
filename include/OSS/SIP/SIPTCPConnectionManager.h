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


#ifndef SIP_SIPTCPConnectionManager_INCLUDED
#define SIP_SIPTCPConnectionManager_INCLUDED


#include <set>
#include <map>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "SIPTCPConnection.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPTCPConnectionManager: private boost::noncopyable
  /// Manages open connections so that they may be cleanly stopped when the server
  /// needs to shut down.
{
public:
  SIPTCPConnectionManager(SIPFSMDispatch* pDispatch);
    /// Creates a new connection manager object.

  ~SIPTCPConnectionManager();
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

  void add(SIPTCPConnection::Ptr conn);
   /// Add the specified connection to the manager

  void start(SIPTCPConnection::Ptr conn);
    /// Add the specified connection to the manager and start it.

  void stop(SIPTCPConnection::Ptr conn);
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

  SIPTCPConnection::Ptr findConnectionByAddress(const OSS::Net::IPAddress& target);
    /// Find a connection to a specific target if it exists

  SIPTCPConnection::Ptr findConnectionById(OSS::UInt64 identifier);
private:
  OSS::mutex_read_write _rwConnectionsMutex;
  OSS::UInt64 _currentIdentifier;
  std::map<OSS::UInt64, SIPTCPConnection::Ptr> _connections;
  SIPFSMDispatch* _pDispatch;
  unsigned short _portBase;
  unsigned short _portMax;
};


//
// Inlines
//

inline unsigned short SIPTCPConnectionManager::getPortBase() const
{
  return _portBase;
}
  
inline void SIPTCPConnectionManager::setPortBase(unsigned short port)
{
  _portBase = port;
}
  
inline unsigned short SIPTCPConnectionManager::getPortMax() const
{
  return _portMax;
}
  
inline void SIPTCPConnectionManager::setPortmax(unsigned short port)
{
  _portMax = port;
}


} } // OSS::SIP
#endif // SIP_SIPTCPConnectionManager_INCLUDED

