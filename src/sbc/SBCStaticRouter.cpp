
// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCStaticRouter.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include <OSS/Persistent/ClassType.h>
#include <OSS/Persistent/DataType.h>
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {


using namespace OSS::Persistent;

SBCStaticRouter::SBCStaticRouter(SBCManager* pManager) :
  _pManager(pManager)
{
  _pManager->transactionManager().setPostRouteCallback(boost::bind(
      &SBCStaticRouter::onPostRouteTransaction, this, _1, _2, _3, _4, _5
    ));
}

SBCStaticRouter::~SBCStaticRouter()
{

}

bool SBCStaticRouter::loadStaticRoutes()
{
  
  SBCRedisManager::WorkSpace pRedis = _pManager->redis().getSystemDb();

  json::Object staticRoutes;
  json::Array routes;
  if (pRedis->get("static-routes", staticRoutes))
  {
    routes = staticRoutes["routes"];
  }


  {
    OSS::mutex_critic_sec_lock lock(_staticRoutesMutex);
    _staticRoutes.clear();
    for (json::Array::iterator iter = routes.Begin(); iter < routes.End(); iter++)
    {
      try
      {
        json::Object& entry = *iter;
        json::String id = entry["id"]; // identifier for this new route
        json::String method = entry["method"];
        json::String userlenmatch = entry["userlenmatch"]; // length match for the user part of the request uri
        json::String usermatch = entry["usermatch"]; // match rule for the user part of the request uri
        json::String hostmatch = entry["hostmatch"]; // match rule for the host part of the request uri;
        json::String fusermatch = entry["fusermatch"]; // match rule for the user part of the from uri;
        json::String fuserlenmatch = entry["fuserlenmatch"]; // length match for the user part of the from uri
        json::String fhostmatch = entry["fhostmatch"]; // match rule for the host part of the from uri;
        json::String ifacematch = entry["ifacematch"]; // match rule for the interface address where the request was receved

        json::String userstrip = entry["userstrip"]; // strip some digits from the user part of the request-uri;
        json::String userprepend = entry["userprepend"]; // prepend some digits to the user part of the request-uri.
        json::String tuserstrip = entry["tuserstrip"]; // strip some digits from the user part of the to-uri;
        json::String tuserprepend = entry["tuserprepend"]; // prepend some digits to the user part of the to-uri.
        json::String targethost = entry["targethost"]; // rewrite the host part of the request uri;
        json::String ttargethost = entry["ttargethost"]; // rewrite the host part of the to uri;
        json::String targetaddr = entry["targetaddr"]; // target address (host:port) for the outbound request
        json::String transport = entry["transport"]; // type of transport to be used (udp, tcp)
        json::String ifaceaddr = entry["ifaceaddr"]; // host:port where the request will be sent out

        StaticRoute sroute;
        sroute.id = id.Value();
        sroute.method = method.Value();

        std::vector<std::string> userlenmatch_tokens = OSS::string_tokenize(userlenmatch.Value(), "-");
        if (userlenmatch_tokens.size() == 2)
        {
          sroute.minuserlenmatch = OSS::string_to_number<int>(userlenmatch_tokens[0].c_str());
          sroute.maxuserlenmatch = OSS::string_to_number<int>(userlenmatch_tokens[1].c_str());
        }
        sroute.usermatch = usermatch.Value();
        sroute.hostmatch = hostmatch.Value();
        sroute.fusermatch = fusermatch.Value();

        std::vector<std::string> fuserlenmatch_tokens = OSS::string_tokenize(fuserlenmatch.Value(), "-");
        if (fuserlenmatch_tokens.size() == 2)
        {
          sroute.minfuserlenmatch = OSS::string_to_number<int>(fuserlenmatch_tokens[0].c_str());
          sroute.maxfuserlenmatch = OSS::string_to_number<int>(fuserlenmatch_tokens[1].c_str());
        }
        sroute.fhostmatch = fhostmatch.Value();
        sroute.ifacematch = ifacematch.Value();

        sroute.userstrip = OSS::string_to_number<int>(userstrip.Value().c_str());
        sroute.userprepend = userprepend.Value();
        sroute.tuserstrip = OSS::string_to_number<int>(tuserstrip.Value().c_str());
        sroute.tuserprepend = tuserprepend.Value();
        sroute.targethost = targethost.Value();
        sroute.ttargethost = ttargethost.Value();
        sroute.targetaddr = targetaddr.Value();
        sroute.transport = transport.Value();
        sroute.ifaceaddr = ifaceaddr.Value();
        _staticRoutes.push_back(sroute);
      }
      catch(std::exception& e)
      {
        OSS_LOG_ERROR("SBCStaticRouter::loadStaticRoutes Error: " << e.what());
      }
      catch(...)
      {
        OSS_LOG_ERROR("SBCStaticRouter::loadStaticRoutes Error: Unknown exception.");
      }
    }
  }
  return true;
}

SIPMessage::Ptr SBCStaticRouter::onPostRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target
)
{
  return pResponse;
}



} } } // OSS::SIP::SBC




