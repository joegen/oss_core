// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPMessage.h"


namespace OSS {
namespace SIP {


SIPRoute::SIPRoute()
{
}

SIPRoute::SIPRoute(const std::string& route) :
  SIPContact(route)
{
}

SIPRoute::SIPRoute(const SIPRoute& route) :
  SIPContact(route)
{
}

SIPRoute::~SIPRoute()
{
}

static int msg_get_routes(SIPMessage* pMsg, std::list<std::string>& routes, const char* headerName)
{
  routes.clear();
  size_t count = pMsg->hdrGetSize(headerName);
  for(size_t i = 0; i < count; i++)
  {
    std::string route = pMsg->hdrGet(headerName, i);
    size_t size = SIPRoute::getSize(route);
    for( size_t j = 0; j < size; j++)
    {
      RouteURI routeURI;
      SIPRoute::getAt(route, routeURI, j);
      routes.push_back(routeURI.data());
    }
  }
  return routes.size();
}

int SIPRoute::msgGetRoutes(SIPMessage* pMsg, std::list<std::string>& routes)
{
  return msg_get_routes(pMsg, routes, "route");
}

int SIPRoute::msgGetRecordRoutes(SIPMessage* pMsg, std::list<std::string>& routes)
{
  return msg_get_routes(pMsg, routes, "record-route");
}

static bool msg_get_top_route(SIPMessage* pMsg, std::string& route, const char* headerName)
{
  std::string topRoute = pMsg->hdrGet(headerName);
  RouteURI routeURI;
  if (!SIPRoute::getAt(topRoute, routeURI, 0))
  {
    route = "";
    return false;
  }
  route = routeURI.data();
  return true;
}

bool SIPRoute::msgGetTopRoute(SIPMessage* pMsg, std::string& route)
{
  return msg_get_top_route(pMsg, route, "route");
}

bool SIPRoute::msgGetTopRecordRoute(SIPMessage* pMsg, std::string& route)
{
  return msg_get_top_route(pMsg, route, "record-route");
}

static bool msg_pop_top_route(SIPMessage* pMsg, std::string& route, const char* headerName)
{
  std::string topRoute = pMsg->hdrGet(headerName);
  RouteURI routeURI;

  bool ok = SIPRoute::popTopURI(topRoute, routeURI);
  if (!ok)
  {
    route = "";
    return false;
  }

  route = routeURI.data();

  OSS::string_trim(topRoute);

  if (topRoute.empty())
  {
    pMsg->hdrListPopFront(headerName);
  }
  return ok;
}

bool SIPRoute::msgPopTopRoute(SIPMessage* pMsg, std::string& route)
{
  return msg_pop_top_route(pMsg, route, "route");
}

bool SIPRoute::msgPopTopRecordRoute(SIPMessage* pMsg, std::string& route)
{
  return msg_pop_top_route(pMsg, route, "record-route");
}

bool SIPRoute::msgAddRoute(SIPMessage* pMsg, const std::string& route)
{
  return pMsg->hdrListPrepend("Route", route);
}

bool SIPRoute::msgAddRecordRoute(SIPMessage* pMsg, const std::string& route)
{
  return pMsg->hdrListPrepend("Record-Route", route);
}

bool SIPRoute::msgClearRoutes(SIPMessage* pMsg)
{
  return pMsg->hdrListRemove("Route");
}

bool SIPRoute::msgClearRecordRoutes(SIPMessage* pMsg)
{
  return pMsg->hdrListRemove("Record-Route");
}

} } // OSS::SIP



