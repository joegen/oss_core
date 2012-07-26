// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
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


#ifndef SIP_SIPFSMDispatch_INCLUDED
#define SIP_SIPFSMDispatch_INCLUDED


#include "OSS/Cache.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPIctPool.h"
#include "OSS/SIP/SIPNictPool.h"
#include "OSS/SIP/SIPIstPool.h"
#include "OSS/SIP/SIPNistPool.h"
#include "OSS/SIP/SIPTransportService.h"

namespace OSS {
namespace SIP {


class OSS_API SIPFSMDispatch: private boost::noncopyable
	/// Base class for ICT, IST, NICT, NIST state machines
{
public:

  typedef boost::function<void(const SIPMessage::Ptr&, const SIPTransportSession::Ptr&)> UnknownTransactionCallback;
	
  SIPFSMDispatch();
		/// Creates the SIPFSMDispatch.
	
  ~SIPFSMDispatch();
		/// Destroys the SIPFSMDispatch

  void initialize(const boost::filesystem::path& cfgDirectory);
    /// Initialize the dispatch configuration using the configuration path specified.
    /// If an error occurs, this method will throw a PersistenceException.
    ///
    /// Take note that the configuration directory must be both readable and writeble
    /// by the user that owns the process.

  void deinitialize();
    /// Deinitialize the dispatch.  This is usually called when the application
    /// is about the exit.  This is the place where the manager performs final
    /// trash management.

  void onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// This method is called when a SIP message is received from the transport.
    ///
    /// The SIP message will simply be passed to the FSM 
    /// where the transaction state will be processed.
    ///
    /// Take note that this is called directly from the 
    /// transport proactor thread and should therefore
    /// not block and result to a transport sleep.

  void sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    SIPTransaction::Callback& callback);
    /// Send a new SIP (REQUEST) message to the Network.
    ///
    /// This is a none-blocking function call for sending
    /// SIP (REQUEST) messages to the network.  The local interface
    /// to be used must be always specified to add extra
    /// flexibility for applications that bridges multi-homed
    /// networks with complex routing rules.
    ///
    /// The remote address must be in the form of an IP address.
    /// OSSSIP supports both IPV4 and IPV6 destinations.
    /// DNS lookup will not be performed by the transport layer.
    /// Thus, this method expects that the remote address has 
    /// already been resolved using the mechanisms exposed by
    /// OSSADNS or a third party DNS client.
    ///
    /// This function call is a none blocking call.  All responses
    /// will be sent back through the SIPTransaction::Callback function.
    /// If an error occured, the callback function will receive
    /// the SIPException as the first parameter and must always be checked
    /// prior to processing of the rest of the callback parameters.
    /// Normal cause of errors are transaction timeouts.
		
  SIPTransportService& transport();
    /// Returns a raw pointer to the transport

  SIPTransaction::RequestCallback& requestHandler();
    /// Handler for incoming server transaction requests

  UnknownTransactionCallback& unknownInviteTransactionHandler();
    /// Handler for SIP messages not linked to a transaction

  void stop();
    /// Terminate all exiting transactions forcibly.
    ///
    /// Take note that this function call is normally
    /// called by the application stop*() routine and
    /// must never be called by any portion of the application.
    /// But just in case there is a reason to do so, this function
    /// ramains in public scope

  void blockIst(const std::string& id);
    /// Called by the IST transaction upon destruction to avoid an race conditions
    /// for INVITE request that might still arrive after the transaction
    /// has been destroyed

  void setEnableIctForking(bool enable);
    // Enable/Disable support for child transactions in ICT

  bool getEnableIctForking() const;
    // Returns true if ICT forking is enabled
private:
  SIPTransportService _transport;
  SIPIctPool _ict;
  SIPNictPool _nict;
  SIPIstPool _ist;
  SIPNistPool _nist;
  SIPTransaction::RequestCallback _requestHandler;
  UnknownTransactionCallback _unknownInviteTransactionHandler;
  StringPairCache _istBlocker;
  bool _enableIctForking;
};


//
// Inlines
//

inline void SIPFSMDispatch::setEnableIctForking(bool enable)
{
  _enableIctForking = enable;
}

inline bool SIPFSMDispatch::getEnableIctForking() const
{
  return _enableIctForking;
}

inline SIPTransportService& SIPFSMDispatch::transport()
{
  return _transport;
}

inline SIPTransaction::RequestCallback& SIPFSMDispatch::requestHandler()
{
  return _requestHandler;
}

inline SIPFSMDispatch::UnknownTransactionCallback& SIPFSMDispatch::unknownInviteTransactionHandler()
{
  return _unknownInviteTransactionHandler;
}

inline void SIPFSMDispatch::blockIst(const std::string& id)
{
  _istBlocker.add(id, id);
}

} } // namespace OSS::SIP
#endif // SIP_SIPFSMDispatch_INCLUDED


