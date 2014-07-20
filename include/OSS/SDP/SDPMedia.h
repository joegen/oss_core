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


#ifndef SIP_SDPMedia_H_INCLUDED
#define SIP_SDPMedia_H_INCLUDED


#include <boost/shared_ptr.hpp>

#include "OSS/SDP/SDP.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SDP/SDPHeaderList.h"


namespace OSS {
namespace SDP {


class OSS_API SDPMedia : public SDPHeaderList
{
public:
  enum Type
	{
		TYPE_NONE			    = 0x0000, 
		TYPE_AUDIO			  = 0x0001, 
		TYPE_VIDEO			  = 0x0002, 
		TYPE_FAX				  = 0x0008, 
		TYPE_DATA			    = 0x0010
	};

  enum Direction
  {
    MEDIA_UNSET,
    MEDIA_SEND_AND_RECEIVE,
    MEDIA_SEND_ONLY,
    MEDIA_RECEIVE_ONLY,
    MEDIA_INACTIVE
  };

  typedef boost::shared_ptr<SDPMedia> Ptr;
  typedef std::list<int> Payloads;
  typedef SDPHeaderList PayloadAttributes;
  typedef OSS::mutex_read_lock ReadLock;
  typedef OSS::mutex_write_lock WriteLock;

  SDPMedia();
    /// Creates an SDP Header

  explicit SDPMedia(const char* rawHeader);
    /// Create an SDP header from an unparsed header string

  SDPMedia(const SDPMedia& header);
    /// SDP Header copy constructor

  ~SDPMedia();
    /// Destroys the SDP header

  SDPMedia& operator = (const SDPMedia& media);
    /// Copy operator;

  void swap(SDPMedia& media);
    /// Exchange values with another SDPMedia

  void reset();
    /// Reset to a blank slate

  Type getMediaType() const;
    /// Return the media type (audio, video, fax, data)

  void setMediaType(Type type);
    /// Set the media type;

  const Payloads& getPayloads() const;
    /// Return the list of payload formats.

  void setPayloads(const Payloads& payloads);
    /// Set the list of supported payload types

  bool addPayload(int payload);
    /// Append a new payload at the end of the list.
    /// This will return false if the payload is already listed

  void removePayload(int payload);
    /// Remove a payload from the list of supported types.
    /// Take note that this does not remove the attributes for the payload
    /// being removed.  See removeAllAttributes() function instead

  bool hasPayload(int payload) const;

  const std::string& getAddress() const;
    /// Get the IP address binding for the media if c= headers is present

  void setAddressV4(const std::string& address);
    /// Set the IP4 address binding for the media

  void setAddressV6(const std::string& address);
    /// Set the IP6 address binding for the media

  unsigned short getDataPort() const;
    /// Return the data port;

  void setDataPort(unsigned short port);
    /// Set the data port

  unsigned short getControlPort() const;
    /// Return the control port;

  void setControlPort(unsigned short port);
    /// Set the control port

  void removeControlPort();
    /// Remove the control port attribute if it exists

  void setAttribute(int payload, const char* attribute, const std::string& value);
    /// Set the attribute data for a particular payload

  std::string getAttribute(int payload, const char* attribute) const;
    /// Get the attribute for a particual payload if present

  void removeAttribute(int payload, const char* attribute);
    /// Remove the attribute for a aprticular payload if it exists

  void setCommonAttribute(const char* attribute, const std::string& value);
    /// Set common attribute data

  std::string getCommonAttribute(const char* attribute) const;
    /// Get the common attribute value

  void removeCommonAttribute(const char* attribute);
    /// Remove the attribute

  void setFlagAttribute(const char* flag);
    /// Set a flag attribute.  An example of flag attributeis a=recvonly

  bool hasFlagAttribute(const char* flag) const;
    /// Return true if the flag attribute is set

  void removeFlagAttribute(const char* flag);
    /// Remove a flag attribute  


  void setRTPMap(int payload, const std::string& value);
    /// Set the RTP Map data for a particular payload

  std::string getRTPMap(int payload) const;
    /// Get the RTP map for a particual payload if present

  void removeRTPMap(int payload);
    /// Remove the RTP map for a aprticular payload if it exists

  void setFTMP(int payload, const std::string& value);
    /// Set the FMTP data for a particular payload

  std::string getFMTP(int payload) const;
    /// Get the FMTP for a particual payload if present

  void removeFMTP(int payload);
    /// Remove the FMTP for a aprticular payload if it exists

  unsigned short getPtime() const;
    /// Return the ptime attribute

  void setPtime(unsigned short ptime);
    /// Set ptime attribute

  void removePtime();
    /// Remove the ptime attribute

 
  Direction getDirection() const;
    /// Return the direction of media

  void setDirection(Direction direction);
    /// Set the direciton of media

protected:
  void internalSetAddress(const std::string& address, bool isV4);
  SDPMedia::iterator findAttributeIterator(int payload, const char* attributeName);
  mutable OSS::mutex_read_write _rwMutex;
  unsigned short _dataPort;
  unsigned short _controlPort;
  std::string _address;
  Payloads _payloads;
  Type _type;
  unsigned short _ptime;
  Direction _direction;
};

//
// Inlines
//

inline void SDPMedia::setAddressV4(const std::string& address)
{
  internalSetAddress(address, true);
}

inline void SDPMedia::setAddressV6(const std::string& address)
{
  internalSetAddress(address, false);
}

inline void SDPMedia::setRTPMap(int payload, const std::string& value)
{
  setAttribute(payload, "rtpmap", value);
}

inline std::string SDPMedia::getRTPMap(int payload) const
{
  return getAttribute(payload, "rtpmap");
}

inline void SDPMedia::removeRTPMap(int payload)
{
  removeAttribute(payload, "rtpmap");
}

inline void SDPMedia::setFTMP(int payload, const std::string& value)
{
  setAttribute(payload, "fmtp", value);
}

inline std::string SDPMedia::getFMTP(int payload) const
{
  return getAttribute(payload, "fmtp");
}

inline void SDPMedia::removeFMTP(int payload)
{
  removeAttribute(payload, "fmtp");
}

} } // OSS::SDP
#endif // SIP_SDPMedia_H_INCLUDED

