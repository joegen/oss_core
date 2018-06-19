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

#include "OSS/Net/ESLEvent.h"
#include "esl.h"


namespace OSS {
namespace Net {

ESLEvent::ESLEvent() :
 _event(0),
 _header(0)
{
}

ESLEvent::ESLEvent(const std::string& type, const std::string& subclass_name) :
  _event(0),
  _header(0)
{
  create(type, subclass_name);
}

ESLEvent::ESLEvent(void* ev) :
  _event(0),
  _header(0)
{
  esl_event_t* event = static_cast<esl_event_t*>(ev);
  esl_event_t* dup = 0;
  esl_event_dup(&dup, event);
  _event = reinterpret_cast<intptr_t>(dup); 
}
  
ESLEvent::ESLEvent(const ESLEvent& ev) :
  _event(0),
  _header(0)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(ev._event);
  esl_event_t* dup = 0;
  esl_event_dup(&dup, event);
  _event = reinterpret_cast<intptr_t>(dup); 
}

ESLEvent::~ESLEvent()
{
  cleanUp();
}

ESLEvent& ESLEvent::operator=(const ESLEvent& ev)
{
  cleanUp();
  esl_event_t* event = reinterpret_cast<esl_event_t*>(ev._event);
  esl_event_t* dup = 0;
  esl_event_dup(&dup, event);
  _event = reinterpret_cast<intptr_t>(dup); 
  return *this;
}

ESLEvent& ESLEvent::operator=(void* ev)
{
  cleanUp();
  esl_event_t* event = static_cast<esl_event_t*>(ev);
  esl_event_t* dup = 0;
  esl_event_dup(&dup, event);
  _event = reinterpret_cast<intptr_t>(dup); 
  return *this;
}

void ESLEvent::cleanUp()
{
  if (_event) 
  {
    esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
    esl_event_destroy(&event);
    _event = 0;
  }
  _data = std::string();
  _header = 0;
}

bool ESLEvent::create(const std::string& type, const std::string& subclass_name)
{
  cleanUp();
  esl_event_t* event = 0;
  esl_event_types_t event_id;
  if (!strcasecmp(type.c_str(), "json") && !subclass_name.empty()) 
  {
    if (esl_event_create_json(&event, subclass_name.c_str()) != ESL_SUCCESS) 
    {
      return false;
    }
    event_id = event->event_id;
  } 
  else 
  {
    if (esl_name_event(type.c_str(), &event_id) != ESL_SUCCESS) 
    {
      event_id = ESL_EVENT_MESSAGE;
    }
    if (!subclass_name.empty() && event_id != ESL_EVENT_CUSTOM) 
    {
      event_id = ESL_EVENT_CUSTOM;
    }
    if (esl_event_create_subclass(&event, event_id, subclass_name.c_str()) != ESL_SUCCESS) 
    {
      event = 0;
    }
  }
  
  if (event)
  {
    _event = reinterpret_cast<intptr_t>(event);
  }
  return !!_event;
}

const std::string& ESLEvent::data(MessageFormat format) const
{
  char* serialized = 0;
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  _data = std::string();
  if (!event)
  {
    if (format == FMT_JSON && esl_event_serialize_json(event, &serialized) == ESL_SUCCESS)
    {
      _data = serialized;
    } else if (format == FMT_TEXT && esl_event_serialize(event, &serialized, ESL_TRUE) == ESL_SUCCESS)
    {
      _data = serialized;
    }
  }
  return _data;
}

bool ESLEvent::getHeader(const std::string& name, std::string& value, std::size_t index) const
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (event)
  {
    char* header = 0;
    if ((header = esl_event_get_header_idx(event, name.c_str(), index)))
    {
      value = header;
    }
  }
  return !value.empty();
}

bool ESLEvent::getBody(std::string& value) const
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (event)
  {
    char* body = 0;
    if ((body = esl_event_get_body(event)))
    {
      value = body;
    }
  }
  return !value.empty();
}

bool ESLEvent::getEventName(std::string& value) const
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (event)
  {
    const char* name = 0;
    if ((name = esl_event_name(event->event_id)))
    {
      value = name;
    }
  }
  return !value.empty();
}

bool ESLEvent::setBody(const std::string& body)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (!event)
  {
    return false;
  }
  return esl_event_add_body(event, "%s", body.c_str()) == ESL_SUCCESS ? true : false;
}

bool ESLEvent::addHeader(const std::string& name, const std::string& value)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (!event)
  {
    return false;
  }
  return esl_event_add_header_string(event, ESL_STACK_BOTTOM, name.c_str(), value.c_str()) == ESL_SUCCESS ? true : false;
}

bool ESLEvent::pushHeader(const std::string& name, const std::string& value)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (!event)
  {
    return false;
  }
  return esl_event_add_header_string(event, ESL_STACK_PUSH, name.c_str(), value.c_str()) == ESL_SUCCESS ? true : false;
}

bool ESLEvent::unshiftHeader(const std::string& name, const std::string& value)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (!event)
  {
    return false;
  }
  return esl_event_add_header_string(event, ESL_STACK_UNSHIFT, name.c_str(), value.c_str()) == ESL_SUCCESS ? true : false;
}

bool ESLEvent::removeHeader(const std::string& name)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (!event)
  {
    return false;
  }
  return esl_event_del_header(event, name.c_str()) == ESL_SUCCESS ? true : false;
}

const char* ESLEvent::first()
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (event) 
  {
    _header = reinterpret_cast<intptr_t>(event->headers);
    return next();
  }
  return 0;
}

const char* ESLEvent::next()
{
  const char *name = 0;
  esl_event_header_t* hp = reinterpret_cast<esl_event_header_t*>(_header);
  if (hp) 
  {
    name = hp->name;
    _header = reinterpret_cast<intptr_t>(hp->next);
  }
  return name;
}

bool ESLEvent::setPriority(Priority priority)
{
  esl_event_t* event = reinterpret_cast<esl_event_t*>(_event);
  if (event) 
  {
    esl_priority_t prio = ESL_PRIORITY_NORMAL;
    if (priority == EVENT_PRIORITY_LOW)
    {
      prio = ESL_PRIORITY_LOW;
    }
    else if (priority == EVENT_PRIORITY_HIGH)
    {
      prio = ESL_PRIORITY_HIGH;
    }
    esl_event_set_priority(event, prio);
  }
  return !!event;
}


} } // OSS::Net


