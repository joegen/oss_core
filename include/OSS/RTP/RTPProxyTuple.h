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


#ifndef RTP_RTPProxyTuple_INCLUDED
#define RTP_RTPProxyTuple_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_RTP

#include "OSS/RTP/RTPProxy.h"


namespace OSS {
namespace RTP {

class RTPProxySession;

class OSS_API RTPProxyTuple
{
public:
  RTPProxyTuple(RTPProxyManager* pManager, RTPProxySession* pSession, const std::string& identifier, bool isXORDisabled = false);
    /// Creates a new RTPProxyTuple

  ~RTPProxyTuple();
    /// Destroys RTPProxyTuple

  bool open(
    OSS::Net::IPAddress& leg1DataListener,
    OSS::Net::IPAddress& leg2DataListener,
    OSS::Net::IPAddress& leg1ControlListener,
    OSS::Net::IPAddress& leg2ControlListener);
    /// Opens the UDP Proxy sockets

  void start();
    /// Start polling socket events

  void stop();
    /// Stop Polling socket events

  RTPProxyManager*& manager();
    /// Returns a direct pointer to the manager

  const std::string& getIdentifier() const;
    /// Returns a reference to the identifier

  RTPProxy& data();
    /// Return a reference to the data socket proxy

  RTPProxy& control();
    /// Return a reference to the control socket proxy

  void setResizerSamples(int leg1, int leg2);
    /// Enable resizing of RTP packets
protected:
  RTPProxy::Ptr _data;
  RTPProxy::Ptr _control;
  std::string _identifier;
  RTPProxyManager* _pManager;
  RTPProxySession* _pSession;
};

//
// Inlines
//

inline const std::string& RTPProxyTuple::getIdentifier() const
{
  return _identifier;
}

inline RTPProxyManager*& RTPProxyTuple::manager()
{
  return _pManager;
}

inline RTPProxy& RTPProxyTuple::data()
{
  return *_data;
}

inline RTPProxy& RTPProxyTuple::control()
{
  return *_control;
}

inline void RTPProxyTuple::setResizerSamples(int leg1, int leg2)
{
  //
  // Only the data channel can be resized
  //
  _data->setResizerSamples(leg1, leg2);
}

} } // OSS::RTP

#endif // ENABLE_FEATURE_RTP

#endif // RTP_RTPProxyTuple_INCLUDED

