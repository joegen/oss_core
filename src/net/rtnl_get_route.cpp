

  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include "OSS/OSS.h"
#include "OSS/Net/Net.h"
#include "OSS/Net/rtnl_get_route.h"

namespace OSS {
namespace Net {
  bool rtnl_get_route(RTNLRoutes& routes, bool includeLoopBack)
  {
    std::string target;
    return rtnl_get_route(routes, target, includeLoopBack);
  }
} }

#if OSS_PLATFORM_LINUX

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


namespace OSS {
namespace Net {

static RTNLRoutes gCachedRoutes;


static inline void parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
  unsigned short type;

  memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
  while( RTA_OK(rta, len) ) 
  {
    type = rta->rta_type;
    if (type <= max)
    {
      tb[type] = rta;
    }
    rta = RTA_NEXT(rta, len);
  }

  return ;
}


static inline bool append_route_entry(struct nlmsghdr *nlh, RTNLRoutes& routes, const std::string& target, bool includeLoopBack)
{
  struct rtmsg *rt_msg;
  struct rtattr *rt_attr;
  struct rtattr *rt_attrs[RTA_MAX+1];
  unsigned char dst_len;
  struct in_addr inaddr;
  char ifname[IF_NAMESIZE] = {0};
  bool has_entry = false;

  if( NULL==nlh ){
    return false;
  }

  rt_msg = (struct rtmsg *)NLMSG_DATA(nlh);
  dst_len = rt_msg->rtm_dst_len;

  rt_attr = RTM_RTA(rt_msg);
  parse_rtattr(rt_attrs, RTA_MAX, rt_attr, RTM_PAYLOAD(nlh));

  RTNLRouteEntry route;
  
  if( rt_attrs[RTA_GATEWAY] && RTA_PAYLOAD(rt_attrs[RTA_GATEWAY]) <= sizeof(inaddr))
  {
    memset(&inaddr, 0, sizeof(inaddr));
    memcpy(&inaddr, RTA_DATA(rt_attrs[RTA_GATEWAY]), RTA_PAYLOAD(rt_attrs[RTA_GATEWAY]));
    route.gateway = inet_ntoa(inaddr);
    has_entry = true;
  }

  
  if( rt_attrs[RTA_DST] && RTA_PAYLOAD(rt_attrs[RTA_DST]) <= sizeof(inaddr))
  {
    memset(&inaddr, 0, sizeof(inaddr));
    memcpy(&inaddr, RTA_DATA(rt_attrs[RTA_DST]), RTA_PAYLOAD(rt_attrs[RTA_DST]));
    std::ostringstream strm;
    strm << inet_ntoa(inaddr) << "/" << (int)dst_len;
    route.destination = strm.str();
    
    if (!target.empty())
    {
      if (!OSS::socket_address_cidr_verify(target, route.destination))
      {
        return false;
      }
    }
    
    has_entry = true;
  }
  else if (route.gateway.empty())
  {
    return false;
  }
    
  
  if( rt_attrs[RTA_OIF] )
  {
    route.device = if_indextoname(*((int *)RTA_DATA(rt_attrs[RTA_OIF])), ifname);
    if (!includeLoopBack && route.device == "lo")
    {
      return false;
    }
    has_entry = true;
  }
  else
  {
    return false;
  }
  
  if( rt_attrs[RTA_PREFSRC] && RTA_PAYLOAD(rt_attrs[RTA_PREFSRC]) <= sizeof(inaddr))
  {
    memset(&inaddr, 0, sizeof(inaddr));
    memcpy(&inaddr, RTA_DATA(rt_attrs[RTA_PREFSRC]), RTA_PAYLOAD(rt_attrs[RTA_PREFSRC]));
    route.source = inet_ntoa(inaddr);
    has_entry = true;
  }

  if (has_entry)
  {
    routes.push_back(route);
  }

  return has_entry = true;
}

bool rtnl_get_route(RTNLRoutes& routes, const std::string& target, bool includeLoopBack)
{

  int rtnl_sock;
  char recvbuff[2048] = {0};
  struct nlmsghdr *nlh;
  int done = 0;

  struct rtmetric
  {
    struct nlmsghdr nlh;
    struct rtmsg rt_msg;
  } metric;

  rtnl_sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE); 
  if( -1==rtnl_sock )
  {
    return false;
  }

  memset(&metric, 0, sizeof(metric));
  metric.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
  metric.nlh.nlmsg_type = RTM_GETROUTE;
  metric.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  metric.nlh.nlmsg_seq = time(NULL);
  metric.nlh.nlmsg_pid = 0;

  if( send(rtnl_sock, &metric, metric.nlh.nlmsg_len, 0) < 0 )
  {
    return false;
  }

  while(!done)
  {
    int recvlen = recv(rtnl_sock, recvbuff, sizeof(recvbuff), 0);
    if( recvlen < 0 )
    {
      return false;
    }
    else if( 0==recvlen )
    {
      break;	
    }

    nlh = (struct nlmsghdr *)recvbuff;
    for( ; NLMSG_OK(nlh, (unsigned)recvlen); nlh=NLMSG_NEXT(nlh, recvlen) )
    {
      if( NLMSG_DONE == nlh->nlmsg_type )
      {
        done = 1;
        break;
      }
      else if (NLMSG_ERROR == nlh->nlmsg_type)
      {
          return false;
      }
      else
      {
        append_route_entry(nlh, routes, target, includeLoopBack);
      }
    }
  }

  return !routes.empty();
}


bool rtnl_get_source(const RTNLRoutes& routes, std::string& source, const std::string& target, bool includeLoopBack)
{
  std::string gateway;
  for (RTNLRoutes::const_iterator iter = routes.begin(); iter != routes.end(); iter++)
  {
    if (!iter->source.empty())
    {
      if (!includeLoopBack && iter->device == "lo")
      {
        continue;
      }
      
      //
      // Filter out routes that don't have specific gateways
      //
      if (iter->gateway.empty() || iter->gateway == "0.0.0.0")
      {
        continue;
      }
      
      //
      // Check if the destination matches
      //
      if (OSS::socket_address_cidr_verify(target, iter->destination))
      {
        source = iter->source;
        return true;
      }
    }
    
    if (gateway.empty() && !iter->gateway.empty())
    {
      gateway = iter->gateway;
    }
  }
  
  //
  // We did not get the source using destinations.  Use the first item with a gateway
  //
  
  for (RTNLRoutes::const_iterator iter = routes.begin(); iter != routes.end(); iter++)
  {
    if (!iter->source.empty())
    {
      if (!includeLoopBack && iter->device == "lo")
      {
        continue;
      }
      //
      // Check if the gateway matches
      //
      if (OSS::socket_address_cidr_verify(gateway, iter->destination))
      {
        source = iter->source;
        return true;
      }
    }
  }
  
  return false;
}

} } // OSS::Net

#elif OSS_PLATFORM_MAC_OS_X

#pragma message "rtnetlink not available for Mac OSX"

#include <stdint.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <net/if.h>
#include <net/if_var.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/sysctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <err.h>

namespace OSS {
namespace Net {

/* alignment constraint for routing socket */
#define ROUNDUP(a) \
       ((a) > 0 ? (1 + (((a) - 1) | (sizeof(uint32_t) - 1))) : sizeof(uint32_t))
#define ADVANCE(x, n) (x += ROUNDUP((n)->sa_len))

/*
 * Definitions for showing gateway flags.
 */
struct bits {
	uint32_t	b_mask;
	char	b_val;
} bits[] = {
	{ RTF_UP,	'U' },
	{ RTF_GATEWAY,	'G' },
	{ RTF_HOST,	'H' },
	{ RTF_REJECT,	'R' },
	{ RTF_DYNAMIC,	'D' },
	{ RTF_MODIFIED,	'M' },
	{ RTF_MULTICAST,'m' },
	{ RTF_DONE,	'd' }, /* Completed -- for routing messages only */
	{ RTF_CLONING,	'C' },
	{ RTF_XRESOLVE,	'X' },
	{ RTF_LLINFO,	'L' },
	{ RTF_STATIC,	'S' },
	{ RTF_PROTO1,	'1' },
	{ RTF_PROTO2,	'2' },
	{ RTF_WASCLONED,'W' },
	{ RTF_PRCLONING,'c' },
	{ RTF_PROTO3,	'3' },
	{ RTF_BLACKHOLE,'B' },
	{ RTF_BROADCAST,'b' },
	{ RTF_IFSCOPE,	'I' },
	{ 0 }
};

typedef union {
	uint32_t dummy;		/* Helps align structure. */
	struct	sockaddr u_sa;
	u_short	u_data[128];
} sa_u;


static void
get_rtaddrs(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
        int i;
        
        for (i = 0; i < RTAX_MAX; i++) {
                if (addrs & (1 << i)) {
                        rti_info[i] = sa;
						sa = (struct sockaddr *)(ROUNDUP(sa->sa_len) + (char *)sa);
		} else {
                        rti_info[i] = NULL;
        }
}
}

static void
p_flags(int f, char *format)
{
	char name[33], *flags;
	struct bits *p = bits;

	for (flags = name; p->b_mask; p++)
		if (p->b_mask & f)
			*flags++ = p->b_val;
	*flags = '\0';
	printf(format, name);
}

char *
routename(uint32_t in)
{
	char *cp;
	static char line[MAXHOSTNAMELEN];

	cp = 0;

	if (cp) {
		strncpy(line, cp, sizeof(line) - 1);
		line[sizeof(line) - 1] = '\0';
	} else {
#define C(x)	((x) & 0xff)
		in = ntohl(in);
		sprintf(line, "%u.%u.%u.%u",
		    C(in >> 24), C(in >> 16), C(in >> 8), C(in));
	}
	return (line);
}

static uint32_t
forgemask(uint32_t a)
{
	uint32_t m;

	if (IN_CLASSA(a))
		m = IN_CLASSA_NET;
	else if (IN_CLASSB(a))
		m = IN_CLASSB_NET;
	else
		m = IN_CLASSC_NET;
	return (m);
}

static void
domask(char *dst, uint32_t addr, uint32_t mask)
{
	int b, i;

	if (!mask || (forgemask(addr) == mask)) {
		*dst = '\0';
		return;
	}
	i = 0;
	for (b = 0; b < 32; b++)
		if (mask & (1 << b)) {
			int bb;

			i = b;
			for (bb = b+1; bb < 32; bb++)
				if (!(mask & (1 << bb))) {
					i = -1;	/* noncontig */
					break;
				}
			break;
		}
	if (i == -1)
		sprintf(dst, "&0x%x", mask);
	else
		sprintf(dst, "/%d", 32-i);
}

/*
 * Return the name of the network whose address is given.
 * The address is assumed to be that of a net or subnet, not a host.
 */
char *
netname(uint32_t in, uint32_t mask)
{
	char *cp = 0;
	static char line[MAXHOSTNAMELEN];
	uint32_t omask, dmask;
	uint32_t i;

	i = ntohl(in);
	dmask = forgemask(i);
	omask = mask;

	if (cp)
		strncpy(line, cp, sizeof(line) - 1);
	else {
		switch (dmask) {
		case IN_CLASSA_NET:
			if ((i & IN_CLASSA_HOST) == 0) {
				sprintf(line, "%u", C(i >> 24));
				break;
			}
			/* FALLTHROUGH */
		case IN_CLASSB_NET:
			if ((i & IN_CLASSB_HOST) == 0) {
				sprintf(line, "%u.%u",
					C(i >> 24), C(i >> 16));
				break;
			}
			/* FALLTHROUGH */
		case IN_CLASSC_NET:
			if ((i & IN_CLASSC_HOST) == 0) {
				sprintf(line, "%u.%u.%u",
					C(i >> 24), C(i >> 16), C(i >> 8));
				break;
			}
			/* FALLTHROUGH */
		default:
			sprintf(line, "%u.%u.%u.%u",
				C(i >> 24), C(i >> 16), C(i >> 8), C(i));
			break;
		}
	}
	domask(line+strlen(line), i, omask);
	return (line);
}

#ifdef INET6
char *
netname6(struct sockaddr_in6 *sa6, struct sockaddr *sam)
{
	static char line[MAXHOSTNAMELEN];
	u_char *lim;
	int masklen, illegal = 0, flag = NI_WITHSCOPEID;
	struct in6_addr *mask = sam ? &((struct sockaddr_in6 *)sam)->sin6_addr : 0;

	if (sam && sam->sa_len == 0) {
		masklen = 0;
	} else if (mask) {
		u_char *p = (u_char *)mask;
		for (masklen = 0, lim = p + 16; p < lim; p++) {
			switch (*p) {
			 case 0xff:
				 masklen += 8;
				 break;
			 case 0xfe:
				 masklen += 7;
				 break;
			 case 0xfc:
				 masklen += 6;
				 break;
			 case 0xf8:
				 masklen += 5;
				 break;
			 case 0xf0:
				 masklen += 4;
				 break;
			 case 0xe0:
				 masklen += 3;
				 break;
			 case 0xc0:
				 masklen += 2;
				 break;
			 case 0x80:
				 masklen += 1;
				 break;
			 case 0x00:
				 break;
			 default:
				 illegal ++;
				 break;
			}
		}
		if (illegal)
			fprintf(stderr, "illegal prefixlen\n");
	} else {
		masklen = 128;
	}
	if (masklen == 0 && IN6_IS_ADDR_UNSPECIFIED(&sa6->sin6_addr))
		return("default");

	if (nflag)
		flag |= NI_NUMERICHOST;
	getnameinfo((struct sockaddr *)sa6, sa6->sin6_len, line, sizeof(line),
		    NULL, 0, flag);

	if (nflag)
		sprintf(&line[strlen(line)], "/%d", masklen);

	return line;
}

char *
routename6(struct sockaddr_in6 *sa6)
{
	static char line[MAXHOSTNAMELEN];
	int flag = NI_WITHSCOPEID;
	/* use local variable for safety */
	struct sockaddr_in6 sa6_local = {sizeof(sa6_local), AF_INET6, };

	sa6_local.sin6_addr = sa6->sin6_addr;
	sa6_local.sin6_scope_id = sa6->sin6_scope_id;

	if (nflag)
		flag |= NI_NUMERICHOST;

	getnameinfo((struct sockaddr *)&sa6_local, sa6_local.sin6_len,
		    line, sizeof(line), NULL, 0, flag);

	return line;
}
#endif /*INET6*/

static void
p_sockaddr(struct sockaddr *sa, struct sockaddr *mask, int flags, int width)
{
	char workbuf[128], *cplim;
	char *cp = workbuf;

	switch(sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;

		if ((sin->sin_addr.s_addr == INADDR_ANY) &&
			mask &&
		    (ntohl(((struct sockaddr_in *)mask)->sin_addr.s_addr) == 0L || mask->sa_len == 0))
				cp = (char*)"default" ;
		else if (flags & RTF_HOST)
			cp = routename(sin->sin_addr.s_addr);
		else if (mask)
			cp = netname(sin->sin_addr.s_addr,
			    ntohl(((struct sockaddr_in *)mask)->
			    sin_addr.s_addr));
		else
			cp = netname(sin->sin_addr.s_addr, 0L);
		break;
	    }

#ifdef INET6
	case AF_INET6: {
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)sa;
		struct in6_addr *in6 = &sa6->sin6_addr;

		/*
		 * XXX: This is a special workaround for KAME kernels.
		 * sin6_scope_id field of SA should be set in the future.
		 */
		if (IN6_IS_ADDR_LINKLOCAL(in6) ||
		    IN6_IS_ADDR_MC_LINKLOCAL(in6)) {
		    /* XXX: override is ok? */
		    sa6->sin6_scope_id = (u_int32_t)ntohs(*(u_short *)&in6->s6_addr[2]);
		    *(u_short *)&in6->s6_addr[2] = 0;
		}

		if (flags & RTF_HOST)
		    cp = routename6(sa6);
		else if (mask)
		    cp = netname6(sa6, mask);
		else
		    cp = netname6(sa6, NULL);
		break;
	    }
#endif /*INET6*/

	case AF_LINK: {
		struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;

		if (sdl->sdl_nlen == 0 && sdl->sdl_alen == 0 &&
		    sdl->sdl_slen == 0) {
			(void) sprintf(workbuf, "link#%d", sdl->sdl_index);
		} else {
			switch (sdl->sdl_type) {

			case IFT_ETHER: {
				int i;
				u_char *lla = (u_char *)sdl->sdl_data +
				    sdl->sdl_nlen;

				cplim = (char*)"";
				for (i = 0; i < sdl->sdl_alen; i++, lla++) {
					cp += sprintf(cp, "%s%x", cplim, *lla);
					cplim = (char*)":";
				}
				cp = workbuf;
				break;
			    }

			default:
				cp = link_ntoa(sdl);
				break;
			}
		}
		break;
	    }

	default: {
		u_char *s = (u_char *)sa->sa_data, *slim;

		slim =  sa->sa_len + (u_char *) sa;
		cplim = cp + sizeof(workbuf) - 6;
		cp += sprintf(cp, "(%d)", sa->sa_family);
		while (s < slim && cp < cplim) {
			cp += sprintf(cp, " %02x", *s++);
			if (s < slim)
			    cp += sprintf(cp, "%02x", *s++);
		}
		cp = workbuf;
	    }
	}
	if (width < 0 ) {
		printf("%s ", cp);
	} else {
			printf("%-*.*s ", width, width, cp);
	}
}

/*
 * Print address family header before a section of the routing table.
 */
void
pr_family(int af)
{
	char *afname;

	switch (af) {
	case AF_INET:
		afname = (char*)"Internet";
		break;
#ifdef INET6
	case AF_INET6:
		afname = (char*)"Internet6";
		break;
#endif /*INET6*/
	case AF_IPX:
		afname = (char*)"IPX";
		break;
	default:
		afname = NULL;
		break;
	}
	if (afname)
		printf("\n%s:\n", afname);
	else
		printf("\nProtocol Family %d:\n", af);
}

/* column widths; each followed by one space */
#ifndef INET6
#define	WID_DST(af) 	18	/* width of destination column */
#define	WID_GW(af)	18	/* width of gateway column */
#define	WID_IF(af)	7	/* width of netif column */
#else
#define	WID_DST(af) \
	((af) == AF_INET6 ? (lflag ? 39 : (nflag ? 39: 18)) : 18)
#define	WID_GW(af) \
	((af) == AF_INET6 ? (lflag ? 31 : (nflag ? 31 : 18)) : 18)
#define	WID_IF(af)	((af) == AF_INET6 ? 8 : 7)
#endif /*INET6*/

/*
 * Print header for routing table columns.
 */
void
pr_rthdr(int af)
{

	if (af == AF_INET)
			printf("%-*.*s %-*.*s %-10.10s %6.6s %8.8s %*.*s %6s\n",
				WID_DST(af), WID_DST(af), "Destination",
				WID_GW(af), WID_GW(af), "Gateway",
				"Flags", "Refs", "Use",
				WID_IF(af), WID_IF(af), "Netif", "Expire");
	else
		printf("%-*.*s %-*.*s %-10.10s %8.8s %6s\n",
			WID_DST(af), WID_DST(af), "Destination",
			WID_GW(af), WID_GW(af), "Gateway",
			"Flags", "Netif", "Expire");
}

static void
np_rtentry(struct rt_msghdr2 *rtm)
{
	struct sockaddr *sa = (struct sockaddr *)(rtm + 1);
	struct sockaddr *rti_info[RTAX_MAX];
	static int old_fam;
	int fam = 0;
	u_short lastindex = 0xffff;
	static char ifname[IFNAMSIZ + 1];
	sa_u addr, mask;

	/*
	 * Don't print protocol-cloned routes unless -a.
	 */
	if ((rtm->rtm_flags & RTF_WASCLONED) &&
	    (rtm->rtm_parentflags & RTF_PRCLONING)) {
			return;
	}

	fam = sa->sa_family;

	if (fam != old_fam) {
		pr_family(fam);
		pr_rthdr(fam);
		old_fam = fam;
	}
	get_rtaddrs(rtm->rtm_addrs, sa, rti_info);
	bzero(&addr, sizeof(addr));
	if ((rtm->rtm_addrs & RTA_DST))
		bcopy(rti_info[RTAX_DST], &addr, rti_info[RTAX_DST]->sa_len);
	bzero(&mask, sizeof(mask));
	if ((rtm->rtm_addrs & RTA_NETMASK))
		bcopy(rti_info[RTAX_NETMASK], &mask, rti_info[RTAX_NETMASK]->sa_len);
	p_sockaddr(&addr.u_sa, &mask.u_sa, rtm->rtm_flags,
	    WID_DST(addr.u_sa.sa_family));

	p_sockaddr(rti_info[RTAX_GATEWAY], NULL, RTF_HOST,
	    WID_GW(addr.u_sa.sa_family));
	
	p_flags(rtm->rtm_flags, (char*)"%-10.10s ");

	if (addr.u_sa.sa_family == AF_INET) {
		printf("%6u %8u ", rtm->rtm_refcnt, (unsigned int)rtm->rtm_use);
	}
	if (rtm->rtm_index != lastindex) {
		if_indextoname(rtm->rtm_index, ifname);
		lastindex = rtm->rtm_index;
	}
	printf("%*.*s", WID_IF(addr.u_sa.sa_family),
		WID_IF(addr.u_sa.sa_family), ifname);

	if (rtm->rtm_rmx.rmx_expire) {
		time_t expire_time;

		if ((expire_time =
			rtm->rtm_rmx.rmx_expire - time((time_t *)0)) > 0)
			printf(" %6d", (int)expire_time);
	}
	putchar('\n');
}


static void ntreestuff(void)
{
  size_t needed;
  int mib[6];
  char *buf, *next, *lim;
  struct rt_msghdr2 *rtm;

  mib[0] = CTL_NET;
  mib[1] = PF_ROUTE;
  mib[2] = 0;
  mib[3] = 0;
  mib[4] = NET_RT_DUMP2;
  mib[5] = 0;
  if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0) {
    err(1, "sysctl: net.route.0.0.dump estimate");
  }

  if ((buf = (char*)malloc(needed)) == 0) {
    err(2, "malloc(%lu)", (unsigned long)needed);
  }
  if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
    err(1, "sysctl: net.route.0.0.dump");
  }
  lim  = buf + needed;
  for (next = buf; next < lim; next += rtm->rtm_msglen) {
    rtm = (struct rt_msghdr2 *)next;
    np_rtentry(rtm);
  }
}

bool rtnl_get_route(RTNLRoutes& routes, const std::string& target, bool includeLoopBack)
{
  ntreestuff();
  return true;
}
bool rtnl_get_source(const RTNLRoutes& routes, std::string& source, const std::string& target, bool includeLoopBack)
{
  return true;
}

} } // OSS::NET

#endif // OSS_PLATFORM_LINUX OSS_PLATFORM_MAC_OS_X



