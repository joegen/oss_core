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


#ifndef SBCSTATICROUTER_H
#define	SBCSTATICROUTER_H

#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include <vector>
#include <OSS/UTL/Thread.h>


namespace OSS {
namespace SIP {
namespace SBC {

  
using namespace OSS::SIP::B2BUA;

class SBCManager;

class SBCStaticRouter
{
public:
  struct StaticRoute
  {
    std::string id; // identifier for this new route
    std::string method; // method to which this rule applies
    int minuserlenmatch; // length match for the user part of the request uri
    int maxuserlenmatch; // length match for the user part of the request uri
    std::string usermatch; // match rule for the user part of the request uri
    std::string hostmatch; // match rule for the host part of the request uri;
    std::string fusermatch; // match rule for the user part of the from uri;
    int minfuserlenmatch; // length match for the user part of the from uri
    int maxfuserlenmatch; // length match for the user part of the from uri
    std::string fhostmatch; // match rule for the host part of the from uri;
    std::string ifacematch; // match rule for the interface address where the request was receved

    int userstrip; // strip some digits from the user part of the request-uri;
    std::string userprepend; // prepend some digits to the user part of the request-uri.
    int tuserstrip; // strip some digits from the user part of the to-uri;
    std::string tuserprepend; // prepend some digits to the user part of the to-uri.
    std::string targethost; // rewrite the host part of the request uri;
    std::string ttargethost; // rewrite the host part of the to uri;
    std::string targetaddr; // target address (host:port) for the outbound request
    std::string transport; // type of transport to be used (udp, tcp)
    std::string ifaceaddr; // host:port where the request will be sent out
  };

  typedef std::vector<StaticRoute> Routes;

  SBCStaticRouter(SBCManager* pManager);
  ~SBCStaticRouter();

  bool loadStaticRoutes();

  SIPMessage::Ptr onPostRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target
  );

private:
  SBCManager* _pManager;
  OSS::mutex_critic_sec _staticRoutesMutex;
  Routes _staticRoutes;
};


} } } // OSS::SIP::SBC


#endif	// SBCSTATICROUTER_H

