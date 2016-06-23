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



#include "OSS/RTP/RTPPCAPReader.h"

#if OSS_HAVE_PCAP

namespace OSS {
namespace RTP {


RTPPCAPReader::RTPPCAPReader() :
  _pCap(0)
{
}

RTPPCAPReader::RTPPCAPReader(const std::string& filename)
{
  open(filename);
}

bool RTPPCAPReader::open(const std::string& filename)
{
  char errbuff[PCAP_ERRBUF_SIZE];
  _pCap = pcap_open_offline(filename.c_str(), errbuff);
  return !!(_pCap);
}



bool RTPPCAPReader::read(RTPPacket& packet)
{
  if (!_pCap)
    return false;

  pcap_pkthdr* pHeader = 0;
  const u_char* pData = 0;
  bool ret = pcap_next_ex(_pCap, &pHeader, &pData) >= 0;
  if (ret)
  {
    const unsigned int UDP_HEADER_LEN = 42;
    const unsigned int RTP_HEADER_LEN = 12;
    if (pHeader->len > UDP_HEADER_LEN + RTP_HEADER_LEN)
    {
      return packet.parse(pData + UDP_HEADER_LEN, pHeader->len - UDP_HEADER_LEN);
    }
  }
  return ret;
}



} } // OSS::RTP

#endif // OSS_HAVE_PCAP

