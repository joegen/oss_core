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


#ifndef SIP_SIPUDPListener_INCLUDED
#define SIP_SIPUDPListener_INCLUDED


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "OSS/SIP/SIPListener.h"
#include "OSS/SIP/SIPUDPConnection.h"
#include "OSS/STUN/STUNClient.h"
#include "OSS/STUN/STUNMappedAddress.h"

namespace OSS {
namespace SIP {

class SIPFSMDispatch;

class OSS_API SIPUDPListener: 
  public SIPListener,
  private boost::noncopyable
{
public:
  typedef boost::shared_ptr<SIPUDPListener> Ptr;
  
  SIPUDPListener(
    SIPTransportService* pTransportService,
    const SIPTransportSession::Dispatch& dispatch,
    const std::string& address, 
    const std::string& port);
    /// Construct the server to listen on the specified TCP address and port.

  virtual ~SIPUDPListener();
    /// Destroys the server.

  virtual void run();
    /// Run the server's io_service loop.

  virtual void handleStart();
    /// Handle a request to start the server.
  
  virtual void handleStop();
    /// Handle a request to stop the server.
  
  virtual void restart(boost::system::error_code& e);
    /// Restart a temporarily closed listener.  
  
  virtual void closeTemporarily(boost::system::error_code& e);
    /// Temporarily close the transport with a intention to restart it later on
  
  virtual bool canBeRestarted() const;
    /// returns true if the listener can safely be restarted

  boost::asio::ip::udp::socket* _socket;
    /// Socket shared by all connections 

  SIPUDPConnection::Ptr connection();

#if ENABLE_FEATURE_STUN  
  const OSS::STUN::STUNClient::Ptr& getStunClient();
  
  OSS::Net::IPAddress detectNATBinding(const std::string& stunServer);
#endif
  
private:
  virtual void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0);
    /// Handle completion of an asynchronous accept operation.

  SIPUDPConnection::Ptr _pNewConnection;
    /// The next connection to be accepted.

  SIPTransportSession::Dispatch _dispatch;
    /// The handler for all incoming requests.

  friend class SIPUDPConnection;

#if ENABLE_FEATURE_STUN
  OSS::STUN::STUNClient::Ptr _pStunClient;
  OSS::STUN::STUNMappedAddress _stunMappedAddress;
#endif
  
};

//
// Inlines
//

inline SIPUDPConnection::Ptr SIPUDPListener::connection()
{
  return _pNewConnection;
}

#if ENABLE_FEATURE_STUN
  /// Return the stun client shared pointer
inline const OSS::STUN::STUNClient::Ptr& SIPUDPListener::getStunClient()
{
  return _pStunClient;
}
#endif


} } // OSS::SIP
#endif // SIP_SIPUDPListener_INCLUDED

