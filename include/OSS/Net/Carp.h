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

//
// UCARP is covered by the following license :
//

/*
 * Copyright (c) 2004-2010 Frank Denis <j at ucarp.org> with the help of all
 * contributors.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef OSS_CARP_H_INCLUDED
#define	OSS_CARP_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_CARP

#include <boost/thread.hpp>
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/ServiceOptions.h"


namespace OSS {
namespace Net {


class Carp
{
  //
  // This class implements Common Address Redundancy Protocol using
  // ucarp - http://www.pureftpd.org/project/ucarp
  //
public:
  typedef boost::function<void(int)> StateChangeCallback;
  typedef boost::function<void()> GratuitousArpCallback;
  
protected:
  Carp();

public:
  ~Carp();
  
  enum carp_state{ INIT = 0, BACKUP, MASTER };
  struct Config
  {
    Config();
    std::string interface; // bind interface <if>
    std::string srcip; // source (real) IP address of that host
    int vhid; // virtual IP identifier (1-255)
    std::string pass; // password
    std::string passfile; // read password from file
    bool preempt; // becomes a master as soon as possible
    bool neutral; // don't run downscript at start if backup
    std::string addr; // virtual shared IP address
    int advbase; // (-b <seconds>): advertisement frequency
    int advskew; // advertisement skew (0-255)
    std::string upscript; // run <file> to become a master
    std::string downscript; // run <file> to become a backup
    std::string garpscript; // run <file> when a gratuitous ARP is sent
    int deadratio; // ratio to consider a host as dead
    bool shutdown; // shutdown script at exit
    bool ignoreifstate; // ignore interface state (down, no carrier)
    bool nomcast; // use broadcast (instead of multicast) advertisements
    std::string subnet; // subnet to be used for the virtual IP
  };
  
  static Carp* instance();
  static bool getMacAddress(const std::string& ethInterface, unsigned char* hwaddr);
  static bool sendGratuitousArp(const std::string& ethInterface, const std::string& ipAddress);

  Config& config();
  
  bool parseOptions(ServiceOptions& options);

  void run();
  
  static void setStateChangeHandler(const StateChangeCallback& handler);
  
  static void setGratuitousArpHandler(const GratuitousArpCallback& handler);
  
  static void signal_exit();
private:
  static void on_state_change(int state);
  static void on_gratuitous_arp();
  static void on_log_event(int level, const char* log);
  static void on_received_signal(int sig);

  static Carp* _pInstance;
  Config _config;
  boost::thread* _pRunThread;
  
  static StateChangeCallback _stateChangeHandler;
  static GratuitousArpCallback _gratuitousArpHandler;
};

//
// INLINES
//

inline Carp::Config& Carp::config()
{
  return _config;
}




} } // OSS::Net


#endif // ENABLE_FEATURE_CARP

#endif	/* CARP_H */

