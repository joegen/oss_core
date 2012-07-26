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


#include "OSS/SIP/SIPXOR.h"

namespace OSS {
namespace SIP {

struct xor_sip_config
{
  bool enabled;
  bool pad_rtp;
  char key[3];
};
static struct xor_sip_config _xor_config = {false, false, "GS"};


SIPXOR::EncryptFunc SIPXOR::rtpEncryptExternal;
SIPXOR::EncryptFunc SIPXOR::rtpDecryptExternal;
SIPXOR::EncryptFunc SIPXOR::sipEncryptExternal;
SIPXOR::EncryptFunc SIPXOR::sipDecryptExternal;

void SIPXOR::setKey(const char* key)
{
  _xor_config.key[0] = *key;
  _xor_config.key[1] = *(++key);
  _xor_config.key[2] = '\0';
}

const char* SIPXOR::getKey()
{
  return _xor_config.key;
}

void SIPXOR::enable(bool yes)
{
  _xor_config.enabled = yes;
}

bool SIPXOR::isEnabled()
{
  return _xor_config.enabled;
}

void SIPXOR::sipEncrypt(boost::array<char, OSS_SIP_MAX_PACKET_SIZE>& packet, size_t& len)
{
  int size = len;
  int i = 0;
  if (!_xor_config.enabled)
    return;

  if (SIPXOR::sipEncryptExternal)
  {
    //
    // Call external handler
    //
    std::vector<char> input;
    input.reserve(len);
    for (int i = 0; i < len; i++)
      input.push_back(packet[i]);
    SIPXOR::sipEncryptExternal(input);

    for (int i = 0; i < input.size(); i++)
      packet[i] = input[i];
    len = input.size();
    return;
  }

  for(i = 0; i < size; i+=2)
  {
    packet[i] = packet[i] ^ _xor_config.key[0];
    packet[i+1] = packet[i+1] ^ _xor_config.key[1];
    if( i + 2 == size - 1 )
    {
      packet[i+2] = packet[i+2] ^ _xor_config.key[1];
      break;
    }
  }
}

void SIPXOR::sipDecrypt(boost::array<char, OSS_SIP_MAX_PACKET_SIZE>& packet, size_t& len)
{
  if (SIPXOR::sipDecryptExternal)
  {
    //
    // Call external handler
    //
    std::vector<char> input;
    input.reserve(len);
    for (int i = 0; i < len; i++)
      input.push_back(packet[i]);
    SIPXOR::sipDecryptExternal(input);

    for (int i = 0; i < input.size(); i++)
      packet[i] = input[i];
    len = input.size();
    return;
  }

  return sipEncrypt(packet, len);
}

void SIPXOR::rtpEncrypt(boost::array<char, 8192>& packet, size_t& len)
{
  int size = len;
  int i = 0;
  if (!_xor_config.enabled)
    return;

  if (SIPXOR::rtpEncryptExternal)
  {
    //
    // Call external handler
    //
    std::vector<char> input;
    input.reserve(len);
    for (int i = 0; i < len; i++)
      input.push_back(packet[i]);
    SIPXOR::rtpEncryptExternal(input);

    for (int i = 0; i < input.size(); i++)
      packet[i] = input[i];
    len = input.size();
    return;
  }

  for(i = 0; i < size; i+=2)
  {
    packet[i] = packet[i] ^ _xor_config.key[0];
    packet[i+1] = packet[i+1] ^ _xor_config.key[1];
    if( i + 2 == size - 1 )
    {
      packet[i+2] = packet[i+2] ^ _xor_config.key[1];
      break;
    }
  }

}

void SIPXOR::rtpDecrypt(boost::array<char, 8192>& packet, size_t& len)
{
  if (!_xor_config.enabled)
    return;

  if (!_xor_config.pad_rtp)
  {
    if (SIPXOR::rtpDecryptExternal)
    {
      //
      // Call external handler
      //
      std::vector<char> input;
      input.reserve(len);
      for (int i = 0; i < len; i++)
        input.push_back(packet[i]);
      SIPXOR::rtpDecryptExternal(input);

      for (int i = 0; i < input.size(); i++)
        packet[i] = input[i];
      len = input.size();
      return;
    }

    return rtpEncrypt(packet, len);
  }


  boost::array<char, 8192> oldPacket = packet;

  int boundary = 0;
  for (boundary = 0; ((int)packet[boundary]) == 0 && boundary < 8192; boundary++);

  if (len - boundary <= 0)
    return;

 
  for(int i = 0; i < len - boundary; i+=2)
  {
    packet[i] = oldPacket[i + boundary] ^ _xor_config.key[0];
    packet[i+1] = oldPacket[i+1 + boundary] ^ _xor_config.key[1];
    if( i + 2 == len - 1 )
    {
      packet[i+2] = oldPacket[i+2+boundary] ^ _xor_config.key[1];
      break;
    }
  }

  len = len - boundary;
}


} } // OSS::SIP

