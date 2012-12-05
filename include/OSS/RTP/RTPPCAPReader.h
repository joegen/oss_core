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

#ifndef RTPPCAPREADER_H_
#define RTPPCAPREADER_H_

#include <pcap/pcap.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/noncopyable.hpp>
#include "OSS/RTP/RTPPacket.h"


namespace OSS {
namespace RTP {


class RTPPCAPReader : boost::noncopyable
{
public:
  RTPPCAPReader();
  RTPPCAPReader(const std::string& filename);
  bool open(const std::string& filename);
  bool read(RTPPacket& packet);
private:
  pcap_t* _pCap;
};

//
// Inlines
//


} } // OSS::RTP



#endif /* RTPPCAPREADER_H_ */
