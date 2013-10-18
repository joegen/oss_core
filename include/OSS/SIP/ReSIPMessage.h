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

#ifndef OSS_RESIPMESSAGE_H
#define	OSS_RESIPMESSAGE_H

#include "OSS/SIP/SIPMessage.h"
#include "resip/stack/SipMessage.hxx"
#include "resip/stack/MsgHeaderScanner.hxx"

namespace OSS {
namespace SIP {


  class ReSIPMessage : public SIPMessage
  {
  public:
    ReSIPMessage()
    {
    }

    explicit ReSIPMessage(const std::string& packet) :
      SIPMessage(packet)
    {
    }

    explicit ReSIPMessage(const SIPMessage& packet) :
      SIPMessage(packet)
    {
    }

    explicit ReSIPMessage(const resip::SipMessage& packet)
    {
      std::ostringstream strm;
      strm << packet;
      _data = strm.str();
      parse();
    }

    ReSIPMessage& operator=(const resip::SipMessage& packet)
    {
      std::ostringstream strm;
      strm << packet;
      _data = strm.str();

      _finalized = false;
      _startLine = "";
      _body = "";
      _badHeaders.clear();
      _headers.clear();
      _headerOffSet = 0;
      _expectedBodyLen = 0;
      _isResponse = boost::indeterminate;
      _isRequest = boost::indeterminate;

      parse();
      return *this;
    }

    resip::SipMessage asReSIPMessage()
    {
      resip::SipMessage msg;
      resip::Data data(_data);
      size_t len = data.size();
      char *buffer = new char[len + 5];
      msg.addBuffer(buffer);
      memcpy(buffer, data.data(), len);
      resip::MsgHeaderScanner msgHeaderScanner;
      msgHeaderScanner.prepareForMessage(&msg);
      char *unprocessedCharPtr;
      if (msgHeaderScanner.scanChunk(buffer, (unsigned int)len, &unprocessedCharPtr) != resip::MsgHeaderScanner::scrEnd)
      {
        return resip::SipMessage(0);
      }
      // no pp error
      unsigned int used = (unsigned int)(unprocessedCharPtr - buffer);

      if (used < len)
      {
        // body is present .. add it up.
        // NB. The Sip Message uses an overlay (again)
        // for the body. It ALSO expects that the body
        // will be contiguous (of course).
        // it doesn't need a new buffer in UDP b/c there
        // will only be one datagram per buffer. (1:1 strict)
        msg.setBody(buffer+used,UInt32(len-used));
      }

      return msg;
    }
  };

} } // OSS::SIP

#endif	// OSS_RESIPMESSAGE_H

