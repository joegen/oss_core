
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

#include <pcap.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <cstdlib>
#include <cassert>
#include <csignal>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <execinfo.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>

extern "C"
{
  #include "OSS/Net/oss_carp.h"
}

#include "OSS/Logger.h"
#include "OSS/Net/Carp.h"
#include "OSS/Exec/Command.h"


namespace OSS {
namespace Net {


const int PCAP_BUFFER_LENGTH = 1500;
const int PCAP_TIMEOUT = 1000;
#ifdef PF_PACKET
# define HWINFO_DOMAIN PF_PACKET
#else
# define HWINFO_DOMAIN PF_INET
#endif
#ifdef SOCK_PACKET
# define HWINFO_TYPE SOCK_PACKET
#else
# define HWINFO_TYPE SOCK_DGRAM
#endif

Carp* Carp::_pInstance = 0;

Carp::Carp() :
  _pRunThread(0)
{
}

Carp::~Carp()
{
  if (_pRunThread)
  {
    _pRunThread->join();
    delete _pRunThread;
  }
}

Carp* Carp::instance()
{
  if (!Carp::_pInstance)
    Carp::_pInstance = new Carp();
  return Carp::_pInstance;
}

bool Carp::getMacAddress(const std::string& ethInterface, unsigned char* hwaddr)
{
  const char* interface = ethInterface.c_str();
  int s;

  if ((s = socket(HWINFO_DOMAIN, HWINFO_TYPE, 0)) == -1)
  {
      OSS_LOG_ERROR("Carp::getMacAddress:  " << strerror(errno));
      return false;
  }
#ifdef SIOCGIFHWADDR
  {
      struct ifreq ifr;

      if (strlen(interface) >= sizeof ifr.ifr_name)
      {
          OSS_LOG_ERROR("Carp::getMacAddress:  Interface name too long");
          return false;
      }
      strncpy(ifr.ifr_name, interface, sizeof ifr.ifr_name);
      if (ioctl(s, SIOCGIFHWADDR, &ifr) != 0)
      {
          OSS_LOG_ERROR("Carp::getMacAddress: Unable to get hardware info about an interface: " << strerror(errno))
          (void) close(s);
          return false;
      }
      switch (ifr.ifr_hwaddr.sa_family)
      {
      case ARPHRD_ETHER:
      case ARPHRD_IEEE802:
          break;
      default:
          OSS_LOG_ERROR("Carp::getMacAddress: Unknown hardware type " << (unsigned int) ifr.ifr_hwaddr.sa_family);
          return false;
      }
      memcpy(hwaddr, &ifr.ifr_hwaddr.sa_data, sizeof hwaddr);
  }
#elif defined(HAVE_GETIFADDRS)
  {
      struct ifaddrs *ifas;
      struct ifaddrs *ifa;
      struct sockaddr_dl *sadl;
      struct ether_addr *ea;

      if (getifaddrs(&ifas) != 0) {
          return false;
      }
      ifa = ifas;
      while (ifa != NULL) {
          if (strcmp(ifa->ifa_name, interface) == 0 &&
              ifa->ifa_addr->sa_family == AF_LINK) {
              sadl = (struct sockaddr_dl *) ifa->ifa_addr;
              if (sadl == NULL || sadl->sdl_type != IFT_ETHER ||
                  sadl->sdl_alen <= 0) {
                  return false;
              }
              ea = (struct ether_addr *) LLADDR(sadl);
              memcpy(hwaddr, ea, sizeof hwaddr);

              return true;
          }
          ifa = ifa->ifa_next;
      }
      return false;
  }
#elif defined(SIOCGLIFNUM)
  {
      struct lifconf lifc;
      struct lifnum lifn;
      struct lifreq *lifr;
      caddr_t *lifrspace;
      struct arpreq arpreq;

      lifn.lifn_flags = 0;
      lifn.lifn_family = AF_INET;
      if (ioctl(s, SIOCGLIFNUM, &lifn) < 0) {
        return false;
      }
      if (lifn.lifn_count <= 0) {
         return false;
      }
      lifc.lifc_family = lifn.lifn_family;
      lifc.lifc_len = lifn.lifn_count * sizeof *lifr;
      lifrspace = malloc(lifc.lifc_len);
      lifc.lifc_buf = (caddr_t) lifrspace;
      if (ioctl(s, SIOCGLIFCONF, &lifc) < 0) {
          logfile(LOG_ERR, _("ioctl SIOCGLIFCONF error"));
          free(lifrspace);
          return -1;
      }
      lifr = lifc.lifc_req;
for(;;) {
    if (lifn.lifn_count <= 0) {
  free(lifrspace);
  return false;
    }
    lifn.lifn_count--;
          if (strcmp(lifr->lifr_name, interface) == 0) {
              break;
          }
          lifr++;
      }
      memcpy(&arpreq.arp_pa, &lifr->lifr_addr, sizeof arpreq.arp_pa);
      free(lifrspace);
      if (ioctl(s, SIOCGARP, &arpreq) != 0) {
        return false;
      }
      memcpy(hwaddr, &arpreq.arp_ha.sa_data, sizeof hwaddr);
  }
#endif
  return true;
}

bool Carp::sendGratuitousArp(const std::string& ethInterface, const std::string& ipAddress)
{
  unsigned char hwaddr[6];
  struct in_addr vaddr;

  if (!getMacAddress(ethInterface, hwaddr))
    return false;

  if (inet_pton(AF_INET, ipAddress.c_str(), &vaddr) != 1)
  {
    OSS_LOG_ERROR("Carp::sendGratuitousArp: Invalid IP address " << ipAddress );
    return false;
  }

  char errorBuffer[PCAP_ERRBUF_SIZE];
  pcap_t* pDeviceHandle = pcap_open_live(ethInterface.c_str(), PCAP_BUFFER_LENGTH, 0, PCAP_TIMEOUT, errorBuffer);

  if (!pDeviceHandle)
  {
    OSS_LOG_ERROR("Carp::sendGratuitousArp: " << errorBuffer);
    return false;
  }

  int dev_desc_fd = pcap_fileno(pDeviceHandle);
  //
  // From this point onwards, code is extracted from ucarp garp.c
  //

  struct ether_header eh;
  static unsigned char arp[28] = {
          0x00, 0x01,   /* MAC address type */
          0x08, 0x00,   /* Protocol address type */
          0x06, 0x04,   /* MAC address size, protocol address size */
          0x00, 0x01,   /* OP (1=request, 2=reply) */
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Sender MAC */
          0x00, 0x00, 0x00, 0x00,               /* Sender IP */
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff,   /* Target MAC */
          0xff, 0xff, 0xff, 0xff                /* Target IP */
  };
  unsigned char *pkt;
  int rc;

  if (ETHER_ADDR_LEN > 6)
  {
    return false;
  }

  /*
   * - Gratuitous ARPs should use requests for the highest interoperability.
   * - Target MAC and IP should match sender
   * http://www1.ietf.org/mail-archive/web/dhcwg/current/msg03797.html
   * http://en.wikipedia.org/wiki/Address_Resolution_Protocol
   * http://ettercap.sourceforge.net/forum/viewtopic.php?t=2392
   * http://wiki.ethereal.com/Gratuitous_ARP
   */
  arp[7] = 0x01;                                 /* request op */
  memcpy(&arp[8], hwaddr, sizeof hwaddr);        /* Sender MAC */
  memcpy(&arp[14], &vaddr.s_addr, (size_t) 4U);  /* Sender IP */
  memcpy(&arp[18], hwaddr, sizeof hwaddr);       /* Target MAC */
  memcpy(&arp[24], &vaddr.s_addr, (size_t) 4U);  /* Target IP */

  memset(&eh, 0, sizeof eh);
  memcpy(&eh.ether_shost, hwaddr, sizeof hwaddr);
  memset(&eh.ether_dhost, 0xff, ETHER_ADDR_LEN);
  eh.ether_type = htons(ETHERTYPE_ARP);

  if ((pkt = (unsigned char*)malloc(sizeof eh + sizeof arp)) == NULL) {
      OSS_LOG_ERROR("Carp::sendGratuitousArp: out of memory to send gratuitous ARP");
      return false;
  }
  memcpy(pkt, &eh, sizeof eh);
  memcpy(pkt + sizeof eh, arp, sizeof arp);

  do
  {
    rc = write(dev_desc_fd, pkt, sizeof eh + sizeof arp);
  } while (rc < 0 && errno == EINTR);
  
  if (rc < 0)
  {
      OSS_LOG_ERROR("Carp::sendGratuitousArp: write error " << strerror(errno));
      free(pkt);
      return false;
  }
  free(pkt);

  pcap_close(pDeviceHandle);
  return true;
}


Carp::Config::Config()
{
  vhid = 0; // virtual IP identifier (1-255)
  preempt = false; // becomes a master as soon as possible
  neutral = false; // don't run downscript at start if backup
  advbase = 0; // (-b <seconds>): advertisement frequency
  advskew = 0; // advertisement skew (0-255)
  deadratio = 0; // ratio to consider a host as dead
  shutdown = false; // shutdown script at exit
  ignoreifstate = false; // ignore interface state (down, no carrier)
  nomcast = false; // use broadcast (instead of multicast) advertisements
}
bool Carp::parseOptions(ServiceOptions& options)
{
  options.addDaemonOptions();

  //--interface=<if> (-i <if>): bind interface <if>
  options.addOptionString('i', "interface", "bind interface", ServiceOptions::CommandLineOption, true);
  //--srcip=<ip> (-s <ip>): source (real) IP address of that host
  options.addOptionString('s', "srcip", "source (real) IP address of that host", ServiceOptions::CommandLineOption, true);
  //--vhid=<id> (-v <id>): virtual IP identifier (1-255)
  options.addOptionInt('V', "vhid", "virtual IP identifier (1-255)", ServiceOptions::CommandLineOption, true);
  //--pass=<pass> (-p <pass>): password
  options.addOptionString('p', "pass", "password", ServiceOptions::CommandLineOption, false);
  //--passfile=<file> (-o <file>): read password from file
  options.addOptionString('o', "passfile", "read password from file", ServiceOptions::CommandLineOption, false);
  //--preempt (-P): becomes a master as soon as possible
  options.addOptionFlag('E', "preempt", "becomes a master as soon as possible", ServiceOptions::CommandLineOption);
  //--neutral (-n): don't run downscript at start if backup
  options.addOptionFlag('n', "neutral", "don't run downscript at start if backup", ServiceOptions::CommandLineOption);
  //--addr=<ip> (-a <ip>): virtual shared IP address
  options.addOptionString('a', "addr", "virtual shared IP address", ServiceOptions::CommandLineOption, true);
  //--advbase=<seconds> (-b <seconds>): advertisement frequency
  options.addOptionInt('b', "advbase", "advertisement frequency", ServiceOptions::CommandLineOption, false);
  //--advskew=<skew> (-k <skew>): advertisement skew (0-255)
  options.addOptionInt('k', "advskew", "advertisement skew (0-255)", ServiceOptions::CommandLineOption, false);
  //--upscript=<file> (-u <file>): run <file> to become a master
  options.addOptionString('u', "upscript", "run <file> to become a master", ServiceOptions::CommandLineOption, true);
  //--downscript=<file> (-d <file>): run <file> to become a backup
  options.addOptionString('d', "downscript", "run <file> to become a backup", ServiceOptions::CommandLineOption, true);
  //--garpscript=<file> (-g <file>): run <file> when a gratuitous ARP is sent
  options.addOptionString('g', "garpscript", "run <file> when a gratuitous ARP is sent", ServiceOptions::CommandLineOption, false);
  //--deadratio=<ratio> (-r <ratio>): ratio to consider a host as dead
  options.addOptionInt('r', "deadratio", "ratio to consider a host as dead", ServiceOptions::CommandLineOption, false);
  //--shutdown (-z): call shutdown script at exit
  options.addOptionFlag('z', "shutdown", "call shutdown script at exit", ServiceOptions::CommandLineOption);
  //--ignoreifstate (-S): ignore interface state (down, no carrier)
  options.addOptionFlag('S', "ignoreifstate", "ignore interface state (down, no carrier)", ServiceOptions::CommandLineOption);
  //--nomcast (-M): use broadcast (instead of multicast) advertisements
  options.addOptionFlag('M', "nomcast", "use broadcast (instead of multicast) advertisements", ServiceOptions::CommandLineOption);
  //--xparam=<value> (-x): extra parameter to send to up/down scripts
  options.addOptionString('x', "xparam", "extra parameter to send to up/down scripts", ServiceOptions::CommandLineOption, false);

  if (!options.parseOptions())
    return false;

  //
  // Initialize the config
  //


  //--interface=<if> (-i <if>): bind interface <if>
  options.getOption("interface", _config.interface);
  //--srcip=<ip> (-s <ip>): source (real) IP address of that host
  options.getOption("srcip", _config.srcip);
  //--vhid=<id> (-v <id>): virtual IP identifier (1-255)
  options.getOption("vhid", _config.vhid);
  //--pass=<pass> (-p <pass>): password
  options.getOption("pass", _config.pass);
  //--passfile=<file> (-o <file>): read password from file
  options.getOption("passfile", _config.passfile);
  //--preempt (-P): becomes a master as soon as possible
  _config.preempt = options.hasOption("preempt", true);
  //--neutral (-n): don't run downscript at start if backup
  _config.neutral = options.hasOption("neutral", true);
  //--addr=<ip> (-a <ip>): virtual shared IP address
  options.getOption("addr", _config.addr);
  //--advbase=<seconds> (-b <seconds>): advertisement frequency
  options.getOption("advbase", _config.advbase);
  //--advskew=<skew> (-k <skew>): advertisement skew (0-255)
  options.getOption("advskew", _config.advskew);
  //--upscript=<file> (-u <file>): run <file> to become a master
  options.getOption("upscript", _config.upscript);
  //--downscript=<file> (-d <file>): run <file> to become a backup
  options.getOption("downscript", _config.downscript);
  //--garpscript=<file> (-g <file>): run <file> when a gratuitous ARP is sent
  options.getOption("garpscript", _config.garpscript);
  //--deadratio=<ratio> (-r <ratio>): ratio to consider a host as dead
  options.getOption("deadratio", _config.deadratio);
  //--shutdown (-z): call shutdown script at exit
  _config.shutdown = options.hasOption("shutdown", true);
  //--ignoreifstate (-S): ignore interface state (down, no carrier)
  _config.ignoreifstate = options.hasOption("ignoreifstate", true);
  //--nomcast (-M): use broadcast (instead of multicast) advertisements
  _config.nomcast = options.hasOption("nomcast", true);
  //--xparam=<value> (-x): extra parameter to send to up/down scripts
  options.getOption("xparam", _config.xparam);


  return true;
}

void Carp::run()
{
  if (!_pRunThread)
  {
    _pRunThread = new boost::thread(boost::bind(&Carp::run, this));
    return;
  }

  std::ostringstream args;
  args << "Carp ";

  if (!_config.interface.empty())
    args << " --interface " << _config.interface;

  if (!_config.srcip.empty())
    args << " --srcip " << _config.srcip;

  if (_config.vhid)
    args << " --vhid " << _config.vhid;

  if (!_config.pass.empty())
    args << " --pass " << _config.pass;

  if (!_config.passfile.empty())
    args << " --passfile " << _config.passfile;

  if (_config.preempt)
    args << " --preempt ";

  if (_config.neutral)
    args << " --neutral ";

  if (!_config.addr.empty())
    args << " --addr " << _config.addr;

  if (_config.advbase)
    args << " --advbase " << _config.advbase;

  if (!_config.advskew)
    args << " --advskew " << _config.advskew;

  if (!_config.upscript.empty())
    args << " --upscript " << _config.upscript;

  if (!_config.downscript.empty())
    args << " --downscript " << _config.downscript;

  if (!_config.garpscript.empty())
    args << " --garpscript " << _config.garpscript;

  if (_config.deadratio)
    args << " --deadratio " << _config.deadratio;

  if (_config.shutdown)
    args << " --shutdown ";

  if (_config.ignoreifstate)
    args << " --ignoreifstate";

  if (_config.nomcast)
    args << " --nomcast";

  if (!_config.xparam.empty())
    args << " --xparam " << _config.xparam;

  char** argv;
  std::vector<std::string> tokens = OSS::string_tokenize(args.str(), " ");
  OSS::vectorToCArray(tokens, &argv);

  carp_set_state_callback(&Carp::on_state_change);
  carp_set_garp_callback(&Carp::on_gratuitous_arp);
  carp_set_log_callback(&Carp::on_log_event);
  carp_set_signal_handler(&Carp::on_received_signal);

  oss_carp_run(tokens.size(), argv);

  freeCArray(tokens.size(), &argv);
}

void Carp::on_state_change(int state)
{
  //
  // Add your custom code here
  //
}

void Carp::on_gratuitous_arp()
{
  if (carp_get_garp_script())
  {
    OSS::Exec::Command cmd(carp_get_garp_script());
    cmd.execute();
  }
}

void Carp::on_log_event(int level, const char* log)
{
  if (!level)
    level = 1;

  OSS::log(log, (OSS::LogPriority)level);
}

void Carp::on_received_signal(int sig)
{
  switch (sig)
  {
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
      _exit(0);
    case SIGUSR1:
    case SIGUSR2:
      break;
  }
}


} } // OSS::Net




