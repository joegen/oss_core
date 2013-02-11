/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef FIREWALLRULE_H_
#define FIREWALLRULE_H_


#include <list>
#include <string>


namespace OSS {
namespace Net {


class FirewallRule
{
public:
  //
  // This is a wrapper class around libdnet firewall rule structure.
  // libdnet only supports ipchains which is now obsolete in most
  // recent linux kernels and replaced by iptables.
  //

  typedef std::list<FirewallRule> Table;

  enum Protocol
  {
    PROTO_IP = 0, /* dummy for IP */
    PROTO_HOPOPTS = PROTO_IP, /* IPv6 hop-by-hop options */
    PROTO_ICMP = 1, /* ICMP */
    PROTO_IGMP = 2, /* IGMP */
    PROTO_GGP = 3, /* gateway-gateway protocol */
    PROTO_IPIP = 4, /* IP in IP */
    PROTO_ST = 5, /* ST datagram mode */
    PROTO_TCP = 6, /* TCP */
    PROTO_CBT = 7, /* CBT */
    PROTO_EGP = 8, /* exterior gateway protocol */
    PROTO_IGP = 9, /* interior gateway protocol */
    PROTO_BBNRCC = 10 , /* BBN RCC monitoring */
    PROTO_NVP = 11 , /* Network Voice Protocol */
    PROTO_PUP = 12 , /* PARC universal packet */
    PROTO_ARGUS = 13 , /* ARGUS */
    PROTO_EMCON = 14 , /* EMCON */
    PROTO_XNET = 15 , /* Cross Net Debugger */
    PROTO_CHAOS = 16 , /* Chaos */
    PROTO_UDP = 17 , /* UDP */
    PROTO_MUX = 18 , /* multiplexing */
    PROTO_DCNMEAS = 19 , /* DCN measurement */
    PROTO_HMP = 20 , /* Host Monitoring Protocol */
    PROTO_PRM  = 21 , /* Packet Radio Measurement */
    PROTO_IDP = 22 , /* Xerox NS IDP */
    PROTO_TRUNK1 = 23 , /* Trunk-1 */
    PROTO_TRUNK2 = 24 , /* Trunk-2 */
    PROTO_LEAF1 = 25 , /* Leaf-1 */
    PROTO_LEAF2 = 26 , /* Leaf-2 */
    PROTO_RDP = 27 , /* "Reliable Datagram" proto */
    PROTO_IRTP = 28 , /* Inet Reliable Transaction */
    PROTO_TP = 29 , /* ISO TP class 4 */
    PROTO_NETBLT = 30 , /* Bulk Data Transfer */
    PROTO_MFPNSP = 31 , /* MFE Network Services */
    PROTO_MERITINP = 32 , /* Merit Internodal Protocol */
    PROTO_SEP = 33 , /* Sequential Exchange proto */
    PROTO_3PC = 34 , /* Third Party Connect proto */
    PROTO_IDPR = 35 , /* Interdomain Policy Route */
    PROTO_XTP = 36 , /* Xpress Transfer Protocol */
    PROTO_DDP = 37 , /* Datagram Delivery Proto */
    PROTO_CMTP = 38 , /* IDPR Ctrl Message Trans */
    PROTO_TPPP = 39 , /* TP++ Transport Protocol */
    PROTO_IL = 40 , /* IL Transport Protocol */
    PROTO_IPV6 = 41 , /* IPv6 */
    PROTO_SDRP = 42 , /* Source Demand Routing */
    PROTO_ROUTING = 43 , /* IPv6 routing header */
    PROTO_FRAGMENT = 44 , /* IPv6 fragmentation header */
    PROTO_RSVP = 46 , /* Reservation protocol */
    PROTO_GRE  = 47 , /* General Routing Encap */
    PROTO_MHRP = 48 , /* Mobile Host Routing */
    PROTO_ENA = 49 , /* ENA */
    PROTO_ESP = 50 , /* Encap Security Payload */
    PROTO_AH = 51 , /* Authentication Header */
    PROTO_INLSP = 52 , /* Integated Net Layer Sec */
    PROTO_SWIPE = 53 , /* SWIPE */
    PROTO_NARP = 54 , /* NBMA Address Resolution */
    PROTO_MOBILE = 55 , /* Mobile IP, RFC 2004 */
    PROTO_TLSP = 56 , /* Transport Layer Security */
    PROTO_SKIP = 57 , /* SKIP */
    PROTO_ICMPV6 = 58 , /* ICMP for IPv6 */
    PROTO_NONE = 59 , /* IPv6 no next header */
    PROTO_DSTOPTS = 60 , /* IPv6 destination options */
    PROTO_ANYHOST = 61 , /* any host internal proto */
    PROTO_CFTP = 62 , /* CFTP */
    PROTO_ANYNET = 63 , /* any local network */
    PROTO_EXPAK = 64 , /* SATNET and Backroom EXPAK */
    PROTO_KRYPTOLAN = 65 , /* Kryptolan */
    PROTO_RVD = 66 , /* MIT Remote Virtual Disk */
    PROTO_IPPC = 67 , /* Inet Pluribus Packet Core */
    PROTO_DISTFS = 68 , /* any distributed fs */
    PROTO_SATMON = 69 , /* SATNET Monitoring */
    PROTO_VISA = 70 , /* VISA Protocol */
    PROTO_IPCV = 71 , /* Inet Packet Core Utility */
    PROTO_CPNX = 72 , /* Comp Proto Net Executive */
    PROTO_CPHB = 73 , /* Comp Protocol Heart Beat */
    PROTO_WSN = 74 , /* Wang Span Network */
    PROTO_PVP = 75 , /* Packet Video Protocol */
    PROTO_BRSATMON = 76 , /* Backroom SATNET Monitor */
    PROTO_SUNND = 77 , /* SUN ND Protocol */
    PROTO_WBMON = 78 , /* WIDEBAND Monitoring */
    PROTO_WBEXPAK = 79 , /* WIDEBAND EXPAK */
    PROTO_EON = 80 , /* ISO CNLP */
    PROTO_VMTP = 81 , /* Versatile Msg Transport*/
    PROTO_SVMTP = 82 , /* Secure VMTP */
    PROTO_VINES = 83 , /* VINES */
    PROTO_TTP = 84 , /* TTP */
    PROTO_NSFIGP = 85 , /* NSFNET-IGP */
    PROTO_DGP = 86 , /* Dissimilar Gateway Proto */
    PROTO_TCF = 87 , /* TCF */
    PROTO_EIGRP = 88 , /* EIGRP */
    PROTO_OSPF = 89 , /* Open Shortest Path First */
    PROTO_SPRITERPC = 90 , /* Sprite RPC Protocol */
    PROTO_LARP = 91 , /* Locus Address Resolution */
    PROTO_MTP = 92 , /* Multicast Transport Proto */
    PROTO_AX25 = 93 , /* AX.25 Frames */
    PROTO_IPIPENCAP = 94 , /* yet-another IP encap */
    PROTO_MICP = 95 , /* Mobile Internet Ctrl */
    PROTO_SCCSP = 96 , /* Semaphore Comm Sec Proto */
    PROTO_ETHERIP = 97 , /* Ethernet in IPv4 */
    PROTO_ENCAP = 98 , /* encapsulation header */
    PROTO_ANYENC = 99 , /* private encryption scheme */
    PROTO_GMTP = 100, /* GMTP */
    PROTO_IFMP = 101, /* Ipsilon Flow Mgmt Proto */
    PROTO_PNNI = 102, /* PNNI over IP */
    PROTO_PIM = 103, /* Protocol Indep Multicast */
    PROTO_ARIS = 104, /* ARIS */
    PROTO_SCPS = 105, /* SCPS */
    PROTO_QNX = 106, /* QNX */
    PROTO_AN = 107, /* Active Networks */
    PROTO_IPCOMP = 108, /* IP Payload Compression */
    PROTO_SNP = 109, /* Sitara Networks Protocol */
    PROTO_COMPAQPEER = 110, /* Compaq Peer Protocol */
    PROTO_IPXIP = 111, /* IPX in IP */
    PROTO_VRRP = 112, /* Virtual Router Redundancy */
    PROTO_PGM = 113, /* PGM Reliable Transport */
    PROTO_ANY0HOP = 114, /* 0-hop protocol */
    PROTO_L2TP = 115, /* Layer 2 Tunneling Proto */
    PROTO_DDX = 116, /* D-II Data Exchange (DDX) */
    PROTO_IATP = 117, /* Interactive Agent Xfer */
    PROTO_STP = 118, /* Schedule Transfer Proto */
    PROTO_SRP = 119, /* SpectraLink Radio Proto */
    PROTO_UTI = 120, /* UTI */
    PROTO_SMP = 121, /* Simple Message Protocol */
    PROTO_SM = 122, /* SM */
    PROTO_PTP = 123, /* Performance Transparency */
    PROTO_ISIS = 124, /* ISIS over IPv4 */
    PROTO_FIRE = 125, /* FIRE */
    PROTO_CRTP = 126, /* Combat Radio Transport */
    PROTO_CRUDP = 127, /* Combat Radio UDP */
    PROTO_SSCOPMCE = 128, /* SSCOPMCE */
    PROTO_IPLT = 129, /* IPLT */
    PROTO_SPS = 130, /* Secure Packet Shield */
    PROTO_PIPE = 131, /* Private IP Encap in IP */
    PROTO_SCTP = 132, /* Stream Ctrl Transmission */
    PROTO_FC = 133, /* Fibre Channel */
    PROTO_RSVPIGN = 134, /* RSVP-E2E-IGNORE */
    PROTO_RAW = 255, /* Raw IP packets */
    PROTO_RESERVED = PROTO_RAW,  /* Reserved */
    PROTO_MAX = 255
  };

  enum Direction
  {
    DIR_IN = 1,
    DIR_OUT = 2
  };

  enum Operation
  {
    OP_ALLOW = 1,
    OP_BLOCK = 2
  };

  FirewallRule();

  FirewallRule(const FirewallRule& rule);

  FirewallRule(
    const std::string& device,
    const std::string& sourceAddress,
    unsigned short sourcePort,
    unsigned short sourceEndPort,
    const std::string& destinationAddress,
    unsigned short destinationPort,
    unsigned short destinationEndPort,
    int protocol,
    int direction,
    int operation
  );

  ~FirewallRule();

  FirewallRule& operator=(const FirewallRule& rule);

  void swap(FirewallRule& rule);

  void setDevice(const std::string& device);
  const std::string& getDevice() const;

  void setSourceAddress(const std::string& sourceAddress);
  const std::string& getSourceAddress() const;

  void setSourcePort(unsigned int sourcePort);
  unsigned int getSourcePort() const;

  void setSourceEndPort(unsigned int sourceEndPort);
  unsigned int getSourceEndPort() const;

  void setDestinationAddress(const std::string& destinationAddress);
  const std::string& getDestinationAddress() const;

  void setDestinationPort(unsigned short destinationPort);
  unsigned short getDestinationPort() const;

  void setDestinationEndPort(unsigned short destinationEndPort);
  unsigned short getDestinationEndPort() const;

  void setProtocol(Protocol protocol);
  int getProtocol() const;

  void setDirection(Direction direction);
  int getDirection() const;

  void setOperation(Operation operation);
  int getOperation() const;

  std::string str() const;

private:
  friend class Firewall;
  std::string _device;
  std::string _sourceAddress;
  unsigned short _sourcePort;
  unsigned short _sourceEndPort;
  std::string _destinationAddress;
  unsigned short _destinationPort;
  unsigned short _destinationEndPort;
  int _protocol;
  int _direction;
  int _operation;
};


//
// Inlines
//
inline void FirewallRule::setDevice(const std::string& device)
{
  _device = device;
}

inline const std::string& FirewallRule::getDevice() const
{
  return _device;
}

inline void FirewallRule::setSourceAddress(const std::string& sourceAddress)
{
  _sourceAddress = sourceAddress;
}

inline const std::string& FirewallRule::getSourceAddress() const
{
  return _sourceAddress;
}

inline void FirewallRule::setSourcePort(unsigned int sourcePort)
{
  _sourcePort = sourcePort;
}

inline unsigned int FirewallRule::getSourcePort() const
{
  return _sourcePort;
}

inline void FirewallRule::setSourceEndPort(unsigned int sourceEndPort)
{
  _sourceEndPort = sourceEndPort;
}

inline unsigned int FirewallRule::getSourceEndPort() const
{
  return _sourceEndPort;
}

inline void FirewallRule::setDestinationAddress(const std::string& destinationAddress)
{
  _destinationAddress = destinationAddress;
}

inline const std::string& FirewallRule::getDestinationAddress() const
{
  return _destinationAddress;
}

inline void FirewallRule::setDestinationPort(unsigned short destinationPort)
{
  _destinationPort = destinationPort;
}

inline unsigned short FirewallRule::getDestinationPort() const
{
  return _destinationPort;
}

inline void FirewallRule::setDestinationEndPort(unsigned short destinationEndPort)
{
  _destinationEndPort = destinationEndPort;
}

inline unsigned short FirewallRule::getDestinationEndPort() const
{
  return _destinationEndPort;
}

inline void FirewallRule::setProtocol(Protocol protocol)
{
  _protocol = protocol;
}

inline int FirewallRule::getProtocol() const
{
  return _protocol;
}

inline void FirewallRule::setDirection(Direction direction)
{
  _direction = direction;
}

inline int FirewallRule::getDirection() const
{
  return _direction;
}

inline void FirewallRule::setOperation(Operation operation)
{
  _operation = operation;
}

inline int FirewallRule::getOperation() const
{
  return _operation;
}

} } // OSS::Net


#endif /* FIREWALLRULE_H_ */
