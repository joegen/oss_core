/*
 * Copyright (C) OSS Software Solutions
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



#include "OSS/Net/DTLSSocketInterface.h"


namespace OSS {
namespace Net {

  
DTLSSocketInterface::DTLSSocketInterface(DTLSSession::Type type) :
  _session(type)
{
  _pBio = OSS::Net::DTLSBio::Ptr(new DTLSBio());
  _pBio->attachSocket(this);
  _session.attachBIO(_pBio);
}
  
DTLSSocketInterface::~DTLSSocketInterface()
{
}

} } // OSS::Net