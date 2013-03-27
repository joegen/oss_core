
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
#include "OSS/Logger.h"
#include "OSS/Net/Carp.h"
#include "OSS/Net/oss_carp.h"


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

Carp::Carp()
{
}

Carp::~Carp()
{
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


bool Carp::parseOptions(ServiceOptions& options)
{
  //--interface=<if> (-i <if>): bind interface <if>
  options.addOptionString('i', "interface", "bind interface");
  //--srcip=<ip> (-s <ip>): source (real) IP address of that host
  options.addOptionString('s', "srcip", "source (real) IP address of that host");
  //--vhid=<id> (-v <id>): virtual IP identifier (1-255)
  options.addOptionInt('v', "vhid", "virtual IP identifier (1-255)");
  //--pass=<pass> (-p <pass>): password
  options.addOptionString('p', "pass", "password");
  //--passfile=<file> (-o <file>): read password from file
  options.addOptionString('o', "passfile", "read password from file");
  //--preempt (-P): becomes a master as soon as possible
  options.addOptionFlag('P', "preempt", "becomes a master as soon as possible");
  //--neutral (-n): don't run downscript at start if backup
  options.addOptionFlag('n', "neutral", "don't run downscript at start if backup");
  //--addr=<ip> (-a <ip>): virtual shared IP address
  options.addOptionString('a', "addr", "virtual shared IP address");
  //--advbase=<seconds> (-b <seconds>): advertisement frequency
  options.addOptionInt('b', "advbase", "advertisement frequency");
  //--advskew=<skew> (-k <skew>): advertisement skew (0-255)
  options.addOptionInt('k', "advskew", "advertisement skew (0-255)");
  //--upscript=<file> (-u <file>): run <file> to become a master
  options.addOptionString('u', "upscript", "run <file> to become a master");
  //--downscript=<file> (-d <file>): run <file> to become a backup
  options.addOptionString('d', "downscript", "run <file> to become a backup");
  //--garpscript=<file> (-g <file>): run <file> when a gratuitous ARP is sent
  options.addOptionString('g', "garpscript", "run <file> when a gratuitous ARP is sent");
  //--deadratio=<ratio> (-r <ratio>): ratio to consider a host as dead
  options.addOptionInt('r', "deadratio", "ratio to consider a host as dead");
  //--shutdown (-z): call shutdown script at exit
  options.addOptionFlag('z', "shutdown", "call shutdown script at exit");
  //--ignoreifstate (-S): ignore interface state (down, no carrier)
  options.addOptionFlag('S', "ignoreifstate", "ignore interface state (down, no carrier)");
  //--nomcast (-M): use broadcast (instead of multicast) advertisements
  options.addOptionFlag('M', "nomcast", "use broadcast (instead of multicast) advertisements");
  //--xparam=<value> (-x): extra parameter to send to up/down scripts
  options.addOptionString('x', "xparam", "extra parameter to send to up/down scripts");

  if (!options.parseOptions())
    return false;

  //
  // Initialize the config
  //
  return true;
}


} } // OSS::Net




