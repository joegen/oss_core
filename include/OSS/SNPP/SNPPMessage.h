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

#ifndef OSS_SNPPMESSAGE_H_INCLUDED
#define OSS_SNPPMESSAGE_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/SIP/SIPParser.h"

namespace OSS {
namespace SNPP {

  
static const char REQ_PAGE[]      = "PAGE";
static const char REQ_MESSAGE[]   = "MESS";
static const char REQ_SUBJECT[]   = "SUBJ";
static const char REQ_DATA[]      = "DATA";
static const char REQ_SEND[]      = "SEND";
static const char REQ_RESET[]     = "RESE";
static const char REQ_LOGIN[]     = "LOGI";
static const char REQ_QUIT[]      = "QUIT";
  
class SNPPMessage
{
public:
  enum RequestTypes
  {
    REQUEST_PAGE,
    REQUEST_MESSAGE,
    REQUEST_SUBJECT,
    REQUEST_DATA,
    REQUEST_SEND,
    REQUEST_RESET,
    REQUEST_LOGIN,
    REQUEST_QUIT,
    REQUEST_UNKNOWN
  };
  
  static const char* requestTypeToString(RequestTypes type)
  {
    char* ret = 0;
    if (type < REQUEST_UNKNOWN)
    {
      static const char* method_map[] = 
      {
        REQ_PAGE,
        REQ_MESSAGE,
        REQ_SUBJECT,
        REQ_DATA,
        REQ_SEND,
        REQ_RESET,
        REQ_LOGIN,
        REQ_QUIT
      };
      ret = (char*)method_map[type];
    }
    return ret;
  }
  
  SNPPMessage();
  SNPPMessage(const std::string& data);
  SNPPMessage(const SNPPMessage& data);
  ~SNPPMessage();
  
  std::string _method;
  std::string _message;
};

} } // OSS::SNPP

#endif // OSS_SNPPMESSAGE_H_INCLUDED

