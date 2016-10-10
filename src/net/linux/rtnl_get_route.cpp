

  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sstream>

#include "OSS/Net/Net.h"
#include "OSS/Net/Linux/rtnl_get_route.h"

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
  
  if( rt_attrs[RTA_GATEWAY] )
  {
    memset(&inaddr, 0, sizeof(inaddr));
    memcpy(&inaddr, RTA_DATA(rt_attrs[RTA_GATEWAY]), RTA_PAYLOAD(rt_attrs[RTA_GATEWAY]));
    route.gateway = inet_ntoa(inaddr);
    has_entry = true;
  }

  
  if( rt_attrs[RTA_DST] )
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
  
  if( rt_attrs[RTA_PREFSRC] )
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


bool rtnl_get_route(RTNLRoutes& routes, bool includeLoopBack)
{
  std::string target;
  return rtnl_get_route(routes, target, includeLoopBack);
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
    for( ; NLMSG_OK(nlh, recvlen); nlh=NLMSG_NEXT(nlh, recvlen) )
    {
      if( NLMSG_DONE==nlh->nlmsg_type )
      {
        done = 1;
        break;
      }
      append_route_entry(nlh, routes, target, includeLoopBack);
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
