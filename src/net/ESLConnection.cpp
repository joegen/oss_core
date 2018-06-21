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


#include "OSS/Net/ESLConnection.h"
#include <sstream>
#include "esl.h"


namespace OSS {
namespace Net {


ESLConnection::ESLConnection() :
  _handle(0)
{
  esl_handle_t* handle = static_cast<esl_handle_t*>(malloc(sizeof(esl_handle_t)));
  memset(handle, 0, sizeof(*handle));
  _handle = reinterpret_cast<intptr_t>(handle);
}

ESLConnection::~ESLConnection()
{
  disconnect();
  free(reinterpret_cast<esl_handle_t*>(_handle));
}

bool ESLConnection::connect(const std::string& host, unsigned short port, const std::string& user, const std::string& password)
{
  if (!connected())
  {
    esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
    esl_connect(handle, host.c_str(), port, (user.empty() ? 0 : user.c_str()), password.c_str());
  }
  return connected();
}

ESLEvent::Ptr ESLConnection::getInfo()
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (handle->connected && handle->info_event) 
  {
    return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(handle->info_event)));
  }
  return ESLEvent::Ptr();
}

bool ESLConnection::send(const std::string& cmd)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  return esl_send(handle, cmd.c_str()) == ESL_SUCCESS;
}

ESLEvent::Ptr ESLConnection::sendRecv(const std::string& cmd)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (esl_send_recv(handle, cmd.c_str()) == ESL_SUCCESS && handle->last_sr_event) {
		return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(handle->last_sr_event)));
	}
  return ESLEvent::Ptr();
}

ESLEvent::Ptr ESLConnection::api(const std::string& cmd, const std::string& arg)
{
  if (cmd.empty()) 
  {
    return ESLEvent::Ptr();;
  }
  std::ostringstream strm;
  strm << "api " << cmd << " " << arg;
  return sendRecv(strm.str());
}

ESLEvent::Ptr ESLConnection::bgapi(const std::string& cmd, const std::string& arg, const std::string& job_uuid)
{
  if (cmd.empty())
  {
    return ESLEvent::Ptr();
  }
  std::ostringstream strm;
  if (!job_uuid.empty()) 
  {
    if (arg.empty())
    {
      strm << "bgapi " << cmd << "\nJob-UUID: " << job_uuid;
    }
    else
    {
      strm << "bgapi " << cmd << " " << arg << "\nJob-UUID: " << job_uuid;
    }
  } 
  else 
  {
    if (arg.empty())
    {
      strm << "bgapi " << cmd;
    }
    else
    {
      strm << "bgapi " << cmd << " " << arg;
    }
  }
  return sendRecv(strm.str());
}

ESLEvent::Ptr ESLConnection::sendEvent(const ESLEvent::Ptr& send_me)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (esl_sendevent(handle, reinterpret_cast<esl_event_t*>(send_me->getEvent())) == ESL_SUCCESS) 
  {
    esl_event_t* e = handle->last_ievent ? handle->last_ievent : handle->last_event;
    if (e) 
    {
      return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(e)));
    }
  }
  return ESLEvent::Ptr();
}

bool ESLConnection::sendMsg(const ESLEvent::Ptr& send_me, const std::string& uuid)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  return esl_sendmsg(handle, reinterpret_cast<esl_event_t*>(send_me->getEvent()), uuid.c_str()) == ESL_SUCCESS;
}

ESLEvent::Ptr ESLConnection::recvEvent()
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (esl_recv_event(handle, 1, NULL) == ESL_SUCCESS) 
  {
    esl_event_t* e = handle->last_ievent ? handle->last_ievent : handle->last_event;
    if (e) 
    {
      return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(e)));
    }
  }
  return ESLEvent::Ptr();
}

ESLEvent::Ptr ESLConnection::recvEventTimed(int ms)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (esl_recv_event_timed(handle, ms, 1, NULL) == ESL_SUCCESS) 
  {
    esl_event_t* e = handle->last_ievent ? handle->last_ievent : handle->last_event;
    if (e) 
    {
      return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(e)));
    }
  }
  return ESLEvent::Ptr();
}

ESLEvent::Ptr ESLConnection::filter(const std::string& header, const std::string& value)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  esl_status_t status = esl_filter(handle, header.c_str(), value.c_str());

  if (status == ESL_SUCCESS && handle->last_sr_event) 
  {
    return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(handle->last_sr_event)));
  }

  return ESLEvent::Ptr();
}

bool ESLConnection::events(const std::string& etype, const std::string& value)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  esl_event_type_t type_id = ESL_EVENT_TYPE_PLAIN;
  if (!strcmp(etype.c_str(), "xml")) 
  {
    type_id = ESL_EVENT_TYPE_XML;
  } 
  else if (!strcmp(etype.c_str(), "json")) 
  {
    type_id = ESL_EVENT_TYPE_JSON;
  }
  return esl_events(handle, type_id, value.c_str()) == ESL_SUCCESS;
}

ESLEvent::Ptr ESLConnection::execute(const std::string& app, const std::string& arg, const std::string& uuid)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (esl_execute(handle, app.c_str(), arg.c_str(), uuid.c_str()) == ESL_SUCCESS && handle->last_sr_event) 
  {
		return ESLEvent::Ptr(new ESLEvent(static_cast<void*>(handle->last_sr_event)));
	}
  return ESLEvent::Ptr();
}

ESLEvent::Ptr ESLConnection::executeAsync(const std::string& app, const std::string& arg, const std::string& uuid)
{
  ESLEvent::Ptr event;
  bool async = getAsyncExecute();
  setAsyncExecute(true);
  event = execute(app, arg, uuid);
  setAsyncExecute(async);
  return event;
}

void ESLConnection::setAsyncExecute(bool val)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  handle->async_execute = val;
}

bool ESLConnection::getAsyncExecute() const
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  return !!handle->async_execute;
}

void ESLConnection::setEventLock(bool val)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  handle->event_lock = val;
}

bool ESLConnection::getEventLock() const
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  return !!handle->event_lock;
}

bool ESLConnection::disconnect(void)
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (!handle->destroyed) 
  {
    return esl_disconnect(handle) == ESL_SUCCESS;
  }
  return false;
}

int ESLConnection::socketDescriptor()
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  if (handle->connected) 
  {
    return (int) handle->sock;
  }
  return -1;
}

bool ESLConnection::connected()
{
  esl_handle_t* handle = reinterpret_cast<esl_handle_t*>(_handle);
  return !!(handle->connected);
}


} } // OSS::Net


