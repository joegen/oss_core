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


#ifndef SIP_SIPTLSConnectionManager_INCLUDED
#define SIP_SIPTLSConnectionManager_INCLUDED


#include <set>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "SIPTLSConnection.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPTLSConnectionManager: private boost::noncopyable
  /// Manages open connections so that they may be cleanly stopped when the server
  /// needs to shut down.
{
public:
  SIPTLSConnectionManager(SIPFSMDispatch* pDispatch);
    /// Creates a new connection manager object.

  ~SIPTLSConnectionManager();
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

  void add(SIPTLSConnection::Ptr conn);
   /// Add the specified connection to the manager

  void start(SIPTLSConnection::Ptr conn);
    /// Add the specified connection to the manager and start it.

  void stop(SIPTLSConnection::Ptr conn);
    /// Stop the specified connection.

  void stopAll();
    /// Stop all connections.

private:
  std::set<SIPTLSConnection::Ptr> _connections;
  SIPFSMDispatch* _pDispatch;
};


} } // OSS::SIP
#endif // SIP_SIPTLSConnectionManager_INCLUDED

