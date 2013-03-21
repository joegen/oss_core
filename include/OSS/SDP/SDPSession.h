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


#ifndef SIP_SDPSession_H_INCLUDED
#define SIP_SDPSession_H_INCLUDED


#include "OSS/SDP/SDP.h"
#include "OSS/SDP/SDPMedia.h"
#include "OSS/Exception.h"

namespace OSS {
namespace SDP {

OSS_CREATE_INLINE_EXCEPTION(SDPException, OSS::IOException, "SDP Parser Exception");

class OSS_API SDPSession : public SDPHeaderList
{
public:
  typedef OSS::mutex_read_lock ReadLock;
  typedef OSS::mutex_write_lock WriteLock;
  typedef std::list<SDPMedia::Ptr> MediaDescriptions;

  SDPSession();
    /// Creates an SDP Header

  explicit SDPSession(const char* session);
    /// Create an SDP header from an unparsed header string

  SDPSession(const SDPSession& session);
    /// SDP Header copy constructor

  ~SDPSession();
    /// Destroys the SDP header

  SDPSession& operator = (const SDPSession& session);
    /// Copy operator;

  void swap(SDPSession& session);
    /// Exchange values with another SDPSession

  void reset();
    /// Reset to a blank slate

  std::string toString() const;

  void addMedia(SDPMedia::Ptr media);
    /// add a new media to the description

  SDPMedia::Ptr getMedia(SDPMedia::Type type, size_t index = 0) const;
    /// Get a pointer to a media description.  Changes made
    /// to the media will atumatically reflect to the session

  SDPMedia::Ptr getMedia(size_t index) const;
    /// Get a pointer to a media description

  bool removeMedia(SDPMedia::Type type, size_t index);
    /// Remove the media description

  size_t getMediaCount(SDPMedia::Type type = SDPMedia::TYPE_NONE)const;
    /// Return the number of media descriptions for a certain type
    ///
    /// The default behavior is to return the total media count regardless of media type

  std::string findHeader(char name) const;
    /// Return the value of a ceratin session header

  bool removeHeader(char name);
    /// Remove the header

  std::string getAddress() const;
    /// Get the IP address binding for the session if c= headers is present

  void changeAddress(const std::string& address, const char* version = "IP4");
    /// Set the sessin wide IP Address binding for media.  Take note that
    /// a c lines must exist prior to calling this function

protected:
  
  SDPSession::iterator findIterator(char name);
  
  mutable OSS::mutex_read_write _rwMutex;
  //
  // Session description
  //



  MediaDescriptions _mediaDescriptions;
};

//
// Inlines
//

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const SDPSession& h)
{
  os << h.toString();
  return os;
}

} }// OSS::SDP
#endif // SIP_SDPSession_H_INCLUDED 
