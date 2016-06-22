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


#ifndef SIP_XOR_INCLUDED
#define SIP_XOR_INCLUDED

#include "OSS/build.h"

#if ENABLE_FEATURE_XOR

#include <vector>
#include "boost/array.hpp"
#include "OSS/OSS.h"
#include "OSS/SIP/SIP.h"
#include "OSS/RTP/RTPPacket.h"
#include "boost/function.hpp"

namespace OSS {
namespace SIP {

class OSS_API SIPXOR
  /// Implementation of a proprietary light-weight XOR encryption for SIP Messages
{
public:

  typedef boost::function<void (std::vector<char>&)> EncryptFunc;

  static void setKey(const char* key);
    /// Set the two byte XOR key

  static const char* getKey();
    /// Return the two byte XOR key

  static void enable(bool yes = true);
    /// Enable or disable XOR

  static bool isEnabled();
    /// Returns true if XOR is enabled

  static void sipEncrypt(boost::array<char, OSS_SIP_MAX_PACKET_SIZE>& packet, size_t& len);
    /// Encrypt a byte array

  static void sipDecrypt(boost::array<char, OSS_SIP_MAX_PACKET_SIZE>& packet, size_t& len);
    /// Decrypt a byte array

  static void rtpEncrypt(boost::array<char, RTP_PACKET_BUFFER_SIZE>& packet, size_t& len);
    /// Encrypt a byte array

  static void rtpDecrypt(boost::array<char, RTP_PACKET_BUFFER_SIZE>& packet, size_t& len);
    /// Decrypt a byte array

  static EncryptFunc rtpEncryptExternal;
  static EncryptFunc rtpDecryptExternal;
  static EncryptFunc sipEncryptExternal;
  static EncryptFunc sipDecryptExternal;
};

} } // OSS::SIP

#endif // ENABLE_FEATURE_XOR

#endif //  SIP_XOR_INCLUDED

