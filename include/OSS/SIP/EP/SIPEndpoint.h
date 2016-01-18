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

#ifndef OSS_SIPENDPOINT_H_INCLUDED
#define	OSS_SIPENDPOINT_H_INCLUDED



#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPStack.h"


namespace OSS {
namespace SIP {
namespace EP {
  
  
class SIPEndpoint : boost::noncopyable
{
public:
  enum TransportType
  {
    TYPE_UDP,
    TYPE_TCP,
    TYPE_TLS,
    TYPE_WS,
    TYPE_WSS,
    NUM_TYPE
  };
  
  SIPEndpoint();
  
  ~SIPEndpoint();
        
  bool addTransport(TransportType transportType, unsigned short port);
  bool addTransport(TransportType transportType, const std::string& address, unsigned short port);
  bool addTransport(TransportType transportType, const OSS::Net::IPAddress& address);
  // Add transport to the endpoint.  This must be done
  // prior to calling run();)
  //  
  
  virtual bool runEndpoint();
  // Initialize the endpoint and start processing incoming SIP messages.
  // This function will return right away.  If it returns false,
  // something went wrong with initialization.  Consult logs.
  //
  
  virtual void stopEndpoint();
  // Stop processing incoming messages and deinitialize the endpoint.
  
  SIPStack& stack();
    /// Return a reference to the SIP stack
  
  const std::string& getUserAgentName() const;
    /// Returns the user agent name to be used by the B2BUA if set

  void setUserAgentName(const std::string& userAgentName);
    /// Set the user-agent name.
  
  bool getExternalAddress(const OSS::Net::IPAddress& internalIp, std::string& externalIp) const;
    /// Return the assigned external address for a particular transport.
    /// This is normally used in relation to messages that has to passthrough
    /// a firewall.

  bool getExternalAddress(const std::string& proto, const OSS::Net::IPAddress& internalIp, std::string& externalIp) const;
    /// Return the assigned external address for a particular transport.
    /// This is normally used in relation to messages that has to passthrough
    /// a firewall.

  bool getInternalAddress(
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known

  bool getInternalAddress(
    const std::string& proto,
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known
  
protected:
  virtual void handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// This is the incoming request callback that will be attached to the stack

  virtual void handleAckFor2xxTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport);
    /// Handler of ACK and 200 Ok retransmission
  
protected:
  SIPStack _stack;
  std::string _userAgentName;
};

//
// Inlines
//
inline SIPStack& SIPEndpoint::stack()
{
  return _stack;
}

inline const std::string& SIPEndpoint::getUserAgentName() const
{
  return _userAgentName;
}

inline void SIPEndpoint::setUserAgentName(const std::string& userAgentName)
{
  _userAgentName = userAgentName;
}

inline bool SIPEndpoint::getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getExternalAddress(internalIp, externalIp);
}

inline bool SIPEndpoint::getExternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& internalIp,
  std::string& externalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getExternalAddress(proto, internalIp, externalIp);
}

inline bool SIPEndpoint::getInternalAddress(
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getInternalAddress(
   externalIp, internalIp);
}

inline bool SIPEndpoint::getInternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getInternalAddress(
    proto, externalIp, internalIp);
}

} } } // OSS::SIP::EP

#endif // OSS_SIPENDPOINT_H_INCLUDED