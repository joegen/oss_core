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

#ifndef OSS_TURNSEVER_H_INCLUDED
#define	OSS_TURNSEVER_H_INCLUDED


#include "OSS/Core.h"
#include "OSS/Net.h"
#include "OSS/Thread.h"


namespace OSS {
namespace Net {


class TurnServer
{
public:
  struct Config
  {
    Config();

    std::string listeningDevice;
      /// Listener interface device (optional functionality, Linux only).
      /// The turnserver process must have root privileges to bind the
      /// listening endpoint to a device. If turnserver must run as a
      /// process without root privileges, then just do not use this setting.

    std::vector<std::string> listeningIp;
      /// Listener IP address of relay server.
      /// Multiple listeners can be specified:
      /// ... -L ip1 -L ip2 -L ip3..."
      /// If no IP(s) specified, then all IPv4 and
      /// IPv6 system IPs will be used for listening.
      /// The same ip(s) can be used as both listening and relay ip(s).

    int listeningPort;
      /// TURN listener port for UDP and TCP listeners (Default: 3478).

    int tlsListeningPort;
      /// TURN listener port for TLS and DTLS listeners (Default: 5349).

    int altListeningPort;
      /// Alternative listening port for UDP and TCP listeners
      /// (default value is 3479). This is needed for RFC 5780 support
      /// (STUN extension specs, NAT behavior discovery). The TURN Server
      /// supports RFC 5780 only if it is started with more than one
      /// listening IP address of the same family (IPv4 or IPv6).

    int altTlsListeningPort;
      /// lternative listening port for TLS and DTLS protocols.
      /// Default value is 5350.

    std::string relayInterfaceDevice;
      /// Relay interface device for relay sockets
      /// (optional, Linux only).

    std::string relayIp;
      /// Relay address (the local IP address that
      /// will be used to relay the packets to the
      /// peer). Multiple relay addresses may be used:
      /// ... -E ip1 -E ip2 -E ip3 ...
      /// If no relay IP(s) specified, then all
      /// non-loopback system IPs will be used.
      /// The same ip(s) can be used as both listening and relay ip(s).


    std::string externalIp;
      /// "External" TURN Server address if the server is behind NAT.
      /// In the server-behind-NAT situation, only one relay address must be used, and
      /// that single relay address must be mapped by NAT to the 'external' IP.
      /// For this 'external' IP, NAT must forward ports directly (relayed port 12345
      /// must be always mapped to the same 'external' port 12345).
      /// This value, if not empty, is returned in XOR-RELAYED-ADDRESS field.
      /// By default, this value is empty, and the real relay IP address is used.

    int relayThreads;
      /// number of extra threads to handle the
      /// established connections.
      /// By default, everything in handled in single thread
      /// (the default number of extra threads is 0).

    int minPort;
      /// Lower bound of the UDP port range for relay
      /// endpoints allocation.
      /// Default value is 49152, according to RFC 5766.

    int maxPort;
      /// Upper bound of the UDP port range for relay
      /// endpoints allocation.
      /// Default value is 65535, according to RFC 5766.

    std::string user;
      /// User account, in the column-separated
      /// form 'username:key'.
      /// The key is either the user password, or
      /// the key is generated
      /// by turnadmin command. In the second case,
      /// the key must be prepended with '0x' symbols.
      /// The key is calculated over the user name,
      /// the realm, and the user password.

    std::string realm;
      /// Realm to be used for all users.

    int userQuota;
      /// Per-user allocations quota: how many concurrent
      /// allocations a user can create.

    int totalQuota;
      /// Total allocations quota: global limit on concurrent allocations.

    int maxBps;
      /// Max bytes-per-second bandwidth a TURN session is allowed to handle
      /// (input and output network streams combined). Anything above that limit
      /// will be dropped.

    std::string cert;
      /// Certificate file, PEM format. Same file
      /// search rules applied as for the configuration
      /// file. If both --no-tls and --no-dtls options
      /// are specified, then this parameter is not needed.
      /// Default value is turn_server_cert.pem.

    std::string pkey;
    /// Private key file, PEM format. Same file
    /// search rules applied as for the configuration
    /// file. If both --no-tls and --no-dtls options
    /// are specified, then this parameter is not needed.
    /// Default value is turn_server_pkey.pem.

    bool noUdp;
      /// Do not start UDP listeners.

    bool noTcp;
      /// Do not start TCP listeners.

    bool noTls;
      /// Do not start TLS listeners.

    bool noDtls;
      /// Do not start DTLS listeners.

    bool fingerPrint;
      /// Use fingerprints in the TURN messages. If an incoming request
      ///	contains fingerprint, then TURN server will always add
      ///	fingerprints to the messages in this session, regardless of the
      ///	per-server setting.

    bool ltCredMech;
      /// Use long-term credential mechanism.

    bool noAuth;
      /// Do not use any credential mechanism, allow anonymous access.
      /// Opposite to -a option. This is default option when no
      /// authentication-related options are set.
      /// By default, no credential mechanism is used -
      /// any user is allowed.
  };

  ~TurnServer();
  void run();
  void stop();
  Config& config();

  static TurnServer& instance();
  static void destroyInstance();
protected:
  TurnServer();
  Config _config;
  boost::thread* _pRunThread;
  bool _isRunning;
  static TurnServer* _pInstance;
};

//
// Inlines
//

inline TurnServer::Config& TurnServer::config()
{
  return _config;
}

} }  // OSS::Net

#endif	/* TURNSEVER_H */

