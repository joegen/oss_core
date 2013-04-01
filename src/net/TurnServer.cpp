
/*
 * Copyright (C) 2013  OSS Software Solutions
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

//
// turnserver is covered by the following license :
//

/*
 * Copyright (C) 2012 Citrix Systems
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include "mainrelay.h"
#include "OSS/Net/TurnServer.h"



namespace OSS {
namespace Net {


TurnServer* TurnServer::_pInstance = 0;

TurnServer::Config::Config()
{
  listeningPort = DEFAULT_STUN_PORT;
  tlsListeningPort = DEFAULT_STUN_TLS_PORT;
  altListeningPort = DEFAULT_ALT_STUN_PORT;
  altTlsListeningPort = DEFAULT_ALT_STUN_TLS_PORT;
  relayThreads = 0;
  minPort = LOW_DEFAULT_PORTS_BOUNDARY;
  maxPort = HIGH_DEFAULT_PORTS_BOUNDARY;
  userQuota = 0;
  totalQuota = 0;
  maxBps = 0;
  noUdp = false;
  noTcp = false;
  noTls = false;
  noDtls = false;
  fingerPrint = false;
  ltCredMech = false;
  noAuth = true;

}

TurnServer::TurnServer() :
  _config(),
  _pRunThread(0),
  _isRunning(false)
{
}

TurnServer::~TurnServer()
{
  stop();
}

void TurnServer::stop()
{
  _isRunning = false;
  if (_pRunThread)
  {
    _pRunThread->join();
    delete _pRunThread;
  }
}

TurnServer& TurnServer::instance()
{
  if (!TurnServer::_pInstance)
    TurnServer::_pInstance = new TurnServer();
  return *TurnServer::_pInstance;
}

void TurnServer::destroyInstance()
{
  delete TurnServer::_pInstance;
  TurnServer::_pInstance = 0;
}


void TurnServer::run()
{
  if (!_isRunning && !_pRunThread)
  {
    _pRunThread = new boost::thread(boost::bind(&TurnServer::run, this));
    return;
  }else if (_isRunning)
  {
    return;
  }

  _isRunning = true;

  std::ostringstream args;
  args << "TurnServer" << " -n ";

  if (!_config.listeningDevice.empty())
    args << "--listening-device " << _config.listeningDevice << " ";


  for (std::vector<std::string>::iterator iter = _config.listeningIp.begin(); iter != _config.listeningIp.end(); iter++)
    args << "-L " << *iter << " ";

  args << "--listening-port " << _config.listeningPort << " ";
  args << "--tls-listening-port " << _config.tlsListeningPort << " ";
  args << "--alt-listening-port " << _config.altListeningPort << " ";
  args << "--alt-tls-listening-port " << _config.altTlsListeningPort << " ";

  if (!_config.relayInterfaceDevice.empty())
    args << "--relay-device " << _config.relayInterfaceDevice << " ";

  if (!_config.relayIp.empty())
    args << "--relay-ip " << _config.relayIp << " ";

  if (!_config.externalIp.empty())
    args << "--external-ip " << _config.externalIp << " ";

  if (_config.relayThreads)
    args << "--relay-threads " << _config.relayThreads << " ";

  args << "--min-port " << _config.minPort << " ";
  args << "--max-port " << _config.maxPort << " ";

  if (!_config.user.empty())
    args << "--user " << _config.user << " ";

  if (!_config.realm.empty())
    args << "--realm " << _config.realm << " ";

  if (_config.userQuota)
    args << "--user-quota " << _config.userQuota << " ";

  if (_config.totalQuota)
    args << "--total-quota " << _config.totalQuota << " ";

  if (_config.maxBps)
    args << "--max-bps " << _config.maxBps << " ";

  if (!_config.cert.empty())
    args << "--cert " << _config.cert << " ";


  if (!_config.pkey.empty())
    args << "--pkey " << _config.pkey << " ";

  if (_config.noUdp)
    args << "--no-udp" << " ";

  if (_config.noTcp)
    args << "--no-tcp" << " ";

  if (_config.noTls)
    args << "--no-tls" << " ";

  if (_config.noDtls)
    args << "--no-dtls" << " ";

  if (_config.fingerPrint)
    args << "--fingerprint" << " ";

  if (_config.ltCredMech)
    args << "--lt-cred-mech" << " ";

  if (_config.noAuth)
    args << "--no-auth";

  std::vector<std::string> tokens = OSS::string_tokenize(args.str(), " ");
  char** argv;
  OSS::vectorToCArray(tokens, &argv);
  turn_server_run(tokens.size(), argv);

  freeCArray(tokens.size(), &argv);
}



} }  // OSS::Net



