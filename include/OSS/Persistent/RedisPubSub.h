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

#ifndef OSS_REDISPUBSUB_H_INCLUDED
#define	OSS_REDISPUBSUB_H_INCLUDED

#include "OSS/UTL/Logger.h"
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
#include "OSS/UTL/Thread.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/BlockingQueue.h"
#include <map>


namespace OSS {
namespace Persistent {


class RedisPubSub
{
public:
  typedef std::vector<std::string> Event;
  typedef BlockingQueue<Event> EventQueue;

  RedisPubSub();
  
  ~RedisPubSub();
  
  bool connect(const std::string& host, int port, const std::string& password);
  
  void disconnect();
  
  bool subscribe(const std::string& channelName);
  
  bool publish(const std::string& channelName, const std::string& event);
  
  void receive(Event& event);

  void post(const Event& event);
protected:
  void eventLoop();  
private:
  mutable OSS::mutex_critic_sec _mutex;
  redisAsyncContext* _context;
  struct event_base* _pEventBase;
  boost::thread* _pEventThread;
  EventQueue _eventQueue;
};

} } // OSS::Presistent  
  
#endif	// OSS_REDISPUBSUB_H_INCLUDED

