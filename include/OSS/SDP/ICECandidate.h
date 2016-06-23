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


#ifndef SDP_ICECANDIDATE_H_INCLUDED
#define	SDP_ICECANDIDATE_H_INCLUDED

#include "OSS/UTL/CoreUtils.h"

namespace OSS {
namespace SDP {  


class ICECandidate
{
public:
  ICECandidate();
  ICECandidate(const ICECandidate& candidate);
  ICECandidate(const std::string& candidate);
  ~ICECandidate();
  ICECandidate& operator=(const ICECandidate& candidate);
  ICECandidate& operator=(const std::string& candidate);
  void swap(ICECandidate& candidate);
  const std::string& str();
  bool parseCandidate(const std::string& candidate);
  
  const std::string& getIdentifier() const;
  void setIdentifier(const std::string& identifier);
  
  unsigned int getComponent() const;
  void setComponent(unsigned int component);
  
  unsigned long getPriority() const;
  void setPriority(unsigned long priority);
  
  unsigned short getPort() const;
  void setPort(unsigned short port);
  
  const std::string& getIpAddress() const;
  void setIpAddress(const std::string& ipAddress);
  
  const std::string& getType() const;
  void setType(const std::string& type);
  
  const std::string& getProtocol() const;
  void setProtocol(const std::string& protocol);
  
  unsigned int getGeneration() const;
  void setGeneration(unsigned int generation);
  
  const std::string& getTcpType() const;
  void setTcpType(const std::string& tcpType);
  
  const std::string& getRAddr() const;
  void setRAddr(const std::string& raddr);
  
  unsigned short getRPort() const;
  void setRPort(unsigned short rport);
  
protected:
  void formatCandidate();
  unsigned int _component;
  unsigned long _priority;
  unsigned short _port;
  unsigned short _rport;
  unsigned int _generation;
  std::string _identifier;
  std::string _ip;
  std::string _protocol;
  std::string _type;
  std::string _tcpType;
  std::string _raddr;
  std::string _data;
};


//
// Inlines
//


inline const std::string& ICECandidate::getIdentifier() const
{
  return _identifier;
}
inline void ICECandidate::setIdentifier(const std::string& identifier)
{
  _identifier = identifier;
}

inline unsigned int ICECandidate::getComponent() const
{
  return _component;
}

inline void ICECandidate::setComponent(unsigned int component)
{
  _component = component;
}

inline unsigned long ICECandidate::getPriority() const
{
  return _priority;
}

inline void ICECandidate::setPriority(unsigned long priority)
{
  _priority = priority;
}

inline unsigned short ICECandidate::getPort() const
{
  return _port;
}

inline void ICECandidate::setPort(unsigned short port)
{
  _port = port;
}

inline const std::string& ICECandidate::getIpAddress() const
{
  return _ip;
}

inline void ICECandidate::setIpAddress(const std::string& ipAddress)
{
  _ip = ipAddress;
}

inline const std::string& ICECandidate::getType() const
{
  return _type;
}

inline void ICECandidate::setType(const std::string& type)
{
  _type = type;
}

inline const std::string& ICECandidate::getProtocol() const
{
  return _protocol;
}

inline void ICECandidate::setProtocol(const std::string& protocol)
{
  _protocol = protocol;
}

inline unsigned int ICECandidate::getGeneration() const
{
  return _generation;
}

inline void ICECandidate::setGeneration(unsigned int generation)
{
  _generation = generation;
}

inline const std::string& ICECandidate::getTcpType() const
{
  return _tcpType;
}
inline void ICECandidate::setTcpType(const std::string& tcpType)
{
  _tcpType = tcpType;
}

inline const std::string& ICECandidate::getRAddr() const
{
  return _raddr;
}

inline void ICECandidate::setRAddr(const std::string& raddr)
{
  _raddr = raddr;
}

inline unsigned short ICECandidate::getRPort() const
{
  return _rport;
}

inline void ICECandidate::setRPort(unsigned short rport)
{
  _rport = rport;
}

} } // OSS::SDP



#endif	// SDP_ICECANDIDATE_H_INCLUDED

