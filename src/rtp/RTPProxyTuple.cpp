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


#include "OSS/RTP/RTPProxyTuple.h"
#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/RTP/RTPProxySession.h"

namespace OSS {
namespace RTP {


RTPProxyTuple::RTPProxyTuple(RTPProxyManager* pManager, RTPProxySession* pSession, const std::string& identifier, bool isXORDisabled) :
  _data(new RTPProxy(RTPProxy::Data, pManager, pSession, identifier + "-data", isXORDisabled)),//TODO:magic value
  _control(new RTPProxy(RTPProxy::Control, pManager, pSession, identifier + "-control", isXORDisabled)),//TODO:magic value
  _identifier(identifier),
  _pManager(pManager),
  _pSession(pSession)
{
  
}

RTPProxyTuple::~RTPProxyTuple()
{
  stop();
}

bool RTPProxyTuple::open(
  OSS::Net::IPAddress& leg1DataListener,
  OSS::Net::IPAddress& leg2DataListener,
  OSS::Net::IPAddress& leg1ControlListener,
  OSS::Net::IPAddress& leg2ControlListener)
{
	 //TODO:document how retry is calculated
  int retry = (_pManager->getUDPPortMax() - _pManager->getUDPPortBase()) / 2;
  
  for (int i = 0; i < retry; i++)
  {
    unsigned short leg1DataPort = _pManager->getNextAvailablePortTuple();
    unsigned short leg2DataPort = _pManager->getNextAvailablePortTuple();
    leg1DataListener.setPort(leg1DataPort);
    leg1ControlListener.setPort(leg1DataPort + 1);
    leg2DataListener.setPort(leg2DataPort);
    leg2ControlListener.setPort(leg2DataPort + 1);
    if (_data->open(leg1DataListener, leg2DataListener) &&_control->open(leg1ControlListener, leg2ControlListener))
      return true;
    _data->stop();
    _control->stop();
  }
  return false;
}

void RTPProxyTuple::start()
{
  OSS_LOG_INFO(_pSession->logId() << " RTP Session" << _identifier << " STARTED");
  _data->start();
  _control->start();
}

void RTPProxyTuple::stop()
{
  OSS_LOG_INFO(_pSession->logId() << " RTP Session" << _identifier << " STOPPED");
  _data->close();
  _control->close();
}



} } // OSS::RTP
