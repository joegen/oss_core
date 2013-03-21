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


#ifndef SIP_SIPRoute_INCLUDED
#define SIP_SIPRoute_INCLUDED


#include "OSS/SIP/SIPContact.h"


namespace OSS {
namespace SIP {

class SIPMessage;

typedef SIPFrom RouteURI;

class OSS_API SIPRoute: public SIPContact
  /// CSeq header lazy parser.
{
public:
  SIPRoute();
    /// Create a new Route header

  SIPRoute(const std::string& route);
    /// Create a Route header from a string.
    ///
    /// This constructor expects that the contact header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the contact variable parameter
    /// is the body of the contact header (not including Route:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("route");
    ///   SIPRoute route(hdr);

  SIPRoute(const SIPRoute& route);
    /// Create a new SIPRoute from another SIPRoute object

  ~SIPRoute();
    /// Destroy the SIPRoute object

  static int msgGetRoutes(SIPMessage* pMsg, std::list<std::string>& routes);
    /// Get all the route headers from a SIPMessage and store it to a list.
    /// Returns the number of routes found

  static int msgGetRecordRoutes(SIPMessage* pMsg, std::list<std::string>& routes);
    /// Get all the record-route headers from a SIPMessage and store it to a list.
    /// Returns the number of record-routes found

  static bool msgGetTopRoute(SIPMessage* pMsg, std::string& route);
    /// Get the value of the top Route in a SIPMessage

  static bool msgGetTopRecordRoute(SIPMessage* pMsg, std::string& route);
    /// Get the value of the top Record-Route in a SIPMessage

  static bool msgPopTopRoute(SIPMessage* pMsg, std::string& route);
    /// Pop the top-most route header from a SIPMessage

  static bool msgPopTopRecordRoute(SIPMessage* pMsg, std::string& route);
    /// Pop the top-most record-route header from a SIPMessage

  static bool msgAddRoute(SIPMessage* pMsg, const std::string& route);
    /// Add a new route header to a SIPMessage.
    /// Take note that route headers are inserted at the beginning of
    /// the route header list

  static bool msgAddRecordRoute(SIPMessage* pMsg, const std::string& route);
    /// Add a new record-route header to a SIPMessage.
    /// Take note that record-route headers are inserted at the beginning of
    /// the record-route header list

  static bool msgClearRoutes(SIPMessage* pMsg);
    /// Clear all route headers in the SIPMessage

  static bool msgClearRecordRoutes(SIPMessage* pMsg);
    /// Clear all record-route headers in the SIPMessage
};


typedef SIPRoute SIPRecordRoute;
typedef RouteURI SIPRecordRouteURI;

} } // OSS::SIP
#endif // SIP_SIPRoute_INCLUDED


