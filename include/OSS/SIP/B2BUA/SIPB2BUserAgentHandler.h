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

#ifndef SIPB2BUSERAGENTHANDLER_H_INCLUDED
#define	SIPB2BUSERAGENTHANDLER_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <Poco/ClassLoader.h>
#include <Poco/ClassLibrary.h>
#include "OSS/SIP/OSSSIP.h"


#define BEGIN_MANIFEST POCO_BEGIN_MANIFEST
#define END_MANIFIEST POCO_END_MANIFEST
#define EXPORT_CLASS POCO_EXPORT_CLASS


namespace OSS {
namespace SIP {
namespace B2BUA {


class SIPB2BTransactionManager;

class SIPB2BUserAgentHandler : boost::noncopyable
{
public:
  enum Action
  {
    Handled,
    Continue,
    Deny,
    MaxAction
  };

  SIPB2BUserAgentHandler();

  ~SIPB2BUserAgentHandler();

  virtual Action handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction) = 0;

  SIPB2BTransactionManager* getUserAgent();

  void setUserAgent(SIPB2BTransactionManager* pB2BUA);

  virtual std::size_t priority() const = 0;
  virtual std::string name() const = 0;

protected:
  SIPB2BTransactionManager* _pB2BUA;
  std::size_t _priority;
};

//
// Inlines
//

inline SIPB2BUserAgentHandler::SIPB2BUserAgentHandler() :
  _pB2BUA(0),
  _priority(0)
{
}

inline SIPB2BUserAgentHandler::~SIPB2BUserAgentHandler()
{
}

inline SIPB2BTransactionManager* SIPB2BUserAgentHandler::getUserAgent()
{
  return _pB2BUA;
}

inline void SIPB2BUserAgentHandler::setUserAgent(SIPB2BTransactionManager* pB2BUA)
{
  _pB2BUA = pB2BUA;
}


typedef Poco::ClassLoader<SIPB2BUserAgentHandler> SIPB2BUserAgentHandlerLoader;
typedef Poco::Manifest<SIPB2BUserAgentHandler> SIPB2BUserAgentHandlerManifest;


} } } // OSS::SIP::B2BUA


#endif	/// SIPB2BUSERAGENTHANDLER_H_INCLUDED

