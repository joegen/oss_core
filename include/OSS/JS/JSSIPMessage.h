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


#ifndef JSSIPMESSAGE_H_INCLUDED
#define JSSIPMESSAGE_H_INCLUDED


#include "OSS/JS/JSBase.h"

#include "OSS/SIP/SIPMessage.h"


namespace OSS {
namespace JS {


OSS_CREATE_INLINE_EXCEPTION(JSSIPMessageException, OSS::JS::JSBaseException, "Javascript SIPMessage Processor Exception");

class OSS_API JSSIPMessage : public JSBase
{
public:
  JSSIPMessage(const std::string& contextName);
    /// Create a new JSSIPMessage

  virtual ~JSSIPMessage();
    /// Destroy the JSSIPMessage

  bool processRequest(const OSS::SIP::SIPMessage::Ptr& request);
    /// Send the SIPRequest to the java script engine for processing

protected:
  virtual void initGlobalFuncs(OSS_HANDLE objectTemplate);
    /// Initialize global functions that will be exposed to the java script engine
};

//
// Inlines
//
inline JSSIPMessage::JSSIPMessage(const std::string& contextName) : JSBase(contextName)
{
}

inline JSSIPMessage::~JSSIPMessage()
{
}

inline bool JSSIPMessage::processRequest(const OSS::SIP::SIPMessage::Ptr& request)
{
  return JSBase::processRequest((OSS_HANDLE)request.get());
}

} } //const JSSIPMessage& msg OSS::JS


#endif // JSSIPMESSAGE_H_INCLUDED


