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


#ifndef RTNL_GET_ROUTE_H_INCLUDED
#define RTNL_GET_ROUTE_H_INCLUDED

#include <string>
#include <vector>

namespace OSS {
namespace Net {

struct RTNLRouteEntry 
{
  std::string device;
  std::string destination;
  std::string source;
  std::string gateway;
};

typedef std::vector<RTNLRouteEntry> RTNLRoutes;

bool rtnl_get_route(RTNLRoutes& routes, bool includeLoopBack);
bool rtnl_get_route(RTNLRoutes& routes, const std::string& target, bool includeLoopBack);
bool rtnl_get_source(const RTNLRoutes& routes, std::string& source, const std::string& target, bool includeLoopBack);


} } // OSS::Net 

#endif // RTNL_GET_ROUTE_H_INCLUDED

