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


#ifndef SIP_OSSSIP_INCLUDED
#define SIP_OSSSIP_INCLUDED


#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <list>

#include "OSS/OSS.h"

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/Core.h"
#include "OSS/Net.h"
#include "OSS/DNS.h"
#include "OSS/Thread.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPTransportService.h"


namespace OSS {
namespace SIP {


class OSS_API OSSSIP
  /// This class is the main entry point for all the SIP Stack functions.
  ///
  /// There can be multiple instances of OSSSIP objects in a single application
  /// making it possible to create a multitude of different UA types all residing
  /// inside a homegenous application namespace
{
public:

  OSSSIP();
    /// Creates a new OSSSIP Object

  ~OSSSIP();
    /// Destroys the OSSSIP Object;
 
  bool& enableUDP();
    /// Set this to true/false to enable or disable UDP transport.
    ///
    /// This must be set before calling the OSSSIP::run() method.
  
  bool& enableTCP();
    /// Set this to true/false to enable or disable TLS transport.
    ///
    /// This must be set before calling the OSSSIP::run() method.

  bool& enableTLS();
    /// Set this to true/false to enable or disable TLS transport.
    ///
    /// This must be set before calling the OSSSIP::run() method.

  OSS::socket_address_list& udpListeners();
    /// Returns the UDP Listener vector.
    ///
    /// All interfaces where the UDP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSSSIP::OSS::IPAddress.
    ///
    /// This must be set before calling the OSSSIP::run() method.

  OSS::socket_address_list& tcpListeners();
    /// Returns the TCP Listener vector.
    ///
    /// All interfaces where the TCP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::IPAddress.
    ///
    /// This must be set before calling the OSSSIP::run() method

  OSS::socket_address_list& wsListeners();
    /// Returns the WebSocket Listener vector.
    ///
    /// All interfaces where the WebSocket listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts
    /// a tuple object of type OSS::OSS::IPAddress.
    ///
    /// This must be set before calling the OSSSIP::run() method

  OSS::socket_address_list& tlsListeners();
    /// Returns the TLS Listener vector.
    ///
    /// All interfaces where the TLS listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::IPAddress.
    ///
    /// This must be set before calling the OSSSIP::run() method

  std::string& tlsCertFile();
    /// Set this to specify the absolute path for the certificate file for TLS.

  std::string& tlsDiffieHellmanParamFile();
    /// Set this to specify the absolute path for the Diffie-Hellman parameter file for TLS.

  std::string& tlsPassword();
    /// Set the certificate password for TLS

  void transportInit();
    /// Initialize the SIP Transport.
    ///
    /// This function must be called before a call to run()
    /// If initTransportFromConfig() is used, transportInit will be called
    /// automatically so there is no need to call it before calling run.


  void transportInit(unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short wsPortBase, unsigned short wsPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax);
    /// Initialize the SIP Transport.  This is similar to transportInit()
    /// Except that ports are determined based on the first available port
    /// within the given port ranges.   The transport vectors are
    /// updated accordingly as a successful binding is obtained.
    ///
    /// This function must be called before a call to run()
    /// If initTransportFromConfig() is used, transportInit will be called
    /// automatically so there is no need to call it before calling run.

  
  void initTransportFromConfig(const boost::filesystem::path& cfgFile);
    /// Initialize the sip stack properties from a preexisting CFG files.
    /// This method will throw PersistenceException if the file is none
    /// existent or the file can't be parsed

#if 0
  void initTransportFromConfig(const boost::filesystem::path& cfgFile,
    unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax);
    /// Initialize the sip stack properties from a preexisting CFG files.
    /// This method will throw PersistenceException if the file is none
    /// existent or the file can't be parsed. This is similar to initTransportFromConfig()
    /// Except that ports are determined based on the first available port
    /// within the given port ranges.   The transport vectors are
    /// updated accordingly as a successful binding is obtained.
#endif
  
  void run();
    /// Starts the OSSSIP event subsytem.
    ///
    /// This will block until a call to stop() received.
    /// Once the run method has exited, it is 
    /// possible to run() the system again after changing
    /// the properties of OSSSIP.  One good example
    /// is when a there is a need to change listener address.
    ///
    /// This will throw an exception if a problem is encountered.

  void stop();
    /// Stops the OSSSIP event subsytem.
    ///
    /// This function will block until both the fsm and transport
    /// subsystems have abandoned all pending work safely.
    ///
    /// All pending call to read and write methods will return with 
    /// an io exception.

  void sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    SIPTransaction::Callback callback);
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
    /// Since the actual tranport address to be used for the request via
    /// is yet to be known from the transport layer at this point (specially TCP and TLS),
    /// the via address will be modified automatically by the transport.
    /// Existing parameters in the via will not be changed, neither would
    /// the transport add any to the existing via parameters.  Parameters
    /// such as branch and rport MUST already be present in the existing via.
    ///
    /// This function call is a none blocking call.  All responses
    /// will be sent back through the SIPTransaction::Callback function.
    /// If an error occured, the callback function will receive
    /// the SIPException as the first parameter and must always be checked
    /// prior to processing of the rest of the callback parameters.
    /// Normal cause of errors are transaction timeouts.
    ///
    /// This function may throw a SIPException if the request cannot be processed.
    ///


  void sendRequestDirect(const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress);
    /// This method will send the SIPMessage to the target
    /// without creating a transaction.  No transaction means
    /// the message will not be retransmitted as well as
    /// responses to the message (if it is a request) will
    /// not be tracked by a callback method.  This function
    /// is normally used to relay orphaned requests like ACK
    /// and 200 OK for INVITE transactions.


  void setRequestHandler(const SIPTransaction::RequestCallback& handler);
    /// This function sets the callback handler for incoming requests.
    ///
    /// When a new server is created, the transaction will propagate
    /// the request to the application layer via this callback.
    ///
    /// If this callback is not set, the request will be silently dropped.

  void setAckFor2xxTransactionHandler(const SIPFSMDispatch::UnknownTransactionCallback& handler);
    /// This function sets the callback handler for ACK and 200 OK retranmissions.
    ///
    /// If this callback is not set, the request will be silently dropped.


  SIPTransportService& transport();
    /// Return a reference to the transport service
  
private:

  //
  // FSM Parameters
  //

  SIPFSMDispatch _fsmDispatch;

  //
  // Transport Parameters
  //
  bool _enableUDP;
  bool _enableTCP;
  bool _enableWS;
  bool _enableTLS;
  OSS::socket_address_list _udpListeners;
  OSS::socket_address_list _tcpListeners;
  OSS::socket_address_list _wsListeners;
  OSS::socket_address_list _tlsListeners;
  std::string _tlsCertFile;
  std::string _tlsDiffieHellmanParamFile;
  std::string _tlsPassword;
};

typedef OSSSIP SIPStack;

//
// Inlines
//

inline bool& OSSSIP::enableUDP()
{
  return _enableUDP;
}
  
inline bool& OSSSIP::enableTCP()
{
  return _enableTCP;
}

inline bool& OSSSIP::enableTLS()
{
  return _enableTLS;
}

inline OSS::socket_address_list& OSSSIP::udpListeners()
{
  return _udpListeners;
}

inline OSS::socket_address_list& OSSSIP::tcpListeners()
{
  return _tcpListeners;
}

inline OSS::socket_address_list& OSSSIP::wsListeners()
{
  return _wsListeners;
}

inline OSS::socket_address_list& OSSSIP::tlsListeners()
{
  return _tlsListeners;
}

inline std::string& OSSSIP::tlsCertFile()
{
  return _tlsCertFile;
}

inline std::string& OSSSIP::tlsDiffieHellmanParamFile()
{
  return _tlsDiffieHellmanParamFile;
}

inline std::string& OSSSIP::tlsPassword()
{
  return _tlsPassword;
}

inline void OSSSIP::setRequestHandler(const SIPTransaction::RequestCallback& handler)
{
  _fsmDispatch.requestHandler() = handler;
}

inline void OSSSIP::setAckFor2xxTransactionHandler(const SIPFSMDispatch::UnknownTransactionCallback& handler)
{
  _fsmDispatch.ackFor2xxTransactionHandler() = handler;
}

inline SIPTransportService& OSSSIP::transport()
{
  return _fsmDispatch.transport();
}

} } // OSS::SIP
#endif // SIP_OSSSIP_INCLUDED
