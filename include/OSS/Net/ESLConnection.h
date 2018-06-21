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


#ifndef OSS_ESLCONNECTION_H_INCLUDED
#define OSS_ESLCONNECTION_H_INCLUDED


#include <boost/noncopyable.hpp>
#include "OSS/Net/ESLEvent.h"

namespace OSS {
namespace Net {

class ESLConnection : boost::noncopyable
{
public:
  ESLConnection();
  ~ESLConnection();
  
  bool connect(const std::string& host, unsigned short port, const std::string& password);
  bool connect(const std::string& host, unsigned short port, const std::string& user, const std::string& password);
  
  ESLEvent::Ptr getInfo();
  bool send(const std::string& cmd);
  ESLEvent::Ptr sendRecv(const std::string& cmd);
  ESLEvent::Ptr api(const std::string& cmd, const std::string& arg = "");
  ESLEvent::Ptr bgapi(const std::string& cmd, const std::string& arg = "", const std::string& job_uuid = "");
  ESLEvent::Ptr sendEvent(const ESLEvent::Ptr& send_me);
  bool sendMsg(const ESLEvent::Ptr& send_me, const std::string& uuid = "");
  ESLEvent::Ptr recvEvent();
  ESLEvent::Ptr recvEventTimed(int ms);
  ESLEvent::Ptr filter(const std::string& header, const std::string& value);
  bool events(const std::string& etype, const std::string& value);
  ESLEvent::Ptr execute(const std::string& app, const std::string& arg = "", const std::string& uuid = "");
  ESLEvent::Ptr executeAsync(const std::string& app, const std::string& arg = "", const std::string& uuid = "");
  void setAsyncExecute(bool async);
  bool getAsyncExecute() const;
  void setEventLock(bool lock);
  bool getEventLock() const;
  bool disconnect(void);

  int socketDescriptor();
  bool connected();
  
private:
  intptr_t _handle;
};

//
// Inlines
//

inline bool ESLConnection::connect(const std::string& host, unsigned short port, const std::string& password)
{
  return connect(host, port, "", password); 
}

} } // OSS::Net
#endif // OSS_ESLCONNECTION_H_INCLUDED

