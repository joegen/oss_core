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

#ifndef OSS_ESLEVENT_H_INCLUDED
#define OSS_ESLEVENT_H_INCLUDED

#include <string>
#include <boost/shared_ptr.hpp>

namespace OSS {
namespace Net {


class ESLEvent 
{
public:
  enum MessageFormat
  {
    FMT_JSON,
    FMT_TEXT
  };
  
  enum Priority
  {
    EVENT_PRIORITY_NORMAL,
    EVENT_PRIORITY_LOW,
    EVENT_PRIORITY_HIGH
  };
  
  typedef boost::shared_ptr<ESLEvent> Ptr;
  
  ESLEvent();
  ESLEvent(const std::string& type, const std::string& subclass_name = std::string());
  explicit ESLEvent(void* event);
  explicit ESLEvent(const ESLEvent& event);
  ~ESLEvent();
  
  ESLEvent& operator=(const ESLEvent& event);
  ESLEvent& operator=(void* event);
  
  bool create(const std::string& type, const std::string& subclass_name);
  const std::string& data(MessageFormat format = FMT_TEXT) const; 
  bool getHeader(const std::string& name, std::string& value, std::size_t index = 0) const;
  bool getBody(std::string& body) const;
  bool getEventName(std::string& name) const;
  bool setBody(const std::string& body);
  bool addHeader(const std::string& name, const std::string& value);
  bool pushHeader(const std::string& name, const std::string& value);
  bool unshiftHeader(const std::string& name, const std::string& value);
  bool removeHeader(const std::string& name);
  const char *first();
  const char *next();
  bool setPriority(Priority priority);
  bool isValid() const;
  intptr_t getEvent() const;
  
private:
  void cleanUp();
  intptr_t _event;
  intptr_t _header;
  mutable std::string _data;
};

//
// Inlines
//

inline bool ESLEvent::isValid() const
{
  return !!_event;
}

inline intptr_t ESLEvent::getEvent() const
{
  return _event;
}


} } // OSS::Net

#endif // OSS_ESLEVENT_H_INCLUDED

