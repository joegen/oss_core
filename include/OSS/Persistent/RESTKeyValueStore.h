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

#ifndef OSS_RESTKEYVALUESTORE_H_INCLUDED
#define	OSS_RESTKEYVALUESTORE_H_INCLUDED


#include "OSS/Net/HTTPServer.h"
#include "OSS/Persistent/KeyValueStore.h"


namespace OSS {
namespace Persistent {


class RESTKeyValueStore : public OSS::Net::HTTPServer
{
public:
  typedef OSS::Net::HTTPServer::Request Request;
  typedef OSS::Net::HTTPServer::Response Response;
  
  RESTKeyValueStore();
  
  RESTKeyValueStore(int maxQueuedConnections, int maxThreads);

  ~RESTKeyValueStore();
  
  bool open(const std::string& dataFile);
  
  void setCredentials(const std::string& user, const std::string& password);
  
  void setCustomHandler(const Handler& handler);
protected:
  void onHandleRequest(Request& request, Response& response);
  
  bool isAuthorized(Request& request, Response& response);
  
private:
  KeyValueStore _data;
  std::string _user;
  std::string _password;
  Handler _customHandler;
};

//
// Inlines
//
inline bool RESTKeyValueStore::open(const std::string& dataFile)
{
  return _data.open(dataFile);
}

inline void RESTKeyValueStore::setCredentials(const std::string& user, const std::string& password)
{
  _user = user;
  _password = password;
}

inline void RESTKeyValueStore::setCustomHandler(const Handler& handler)
{
  _customHandler = handler;
}

} }

#endif	// OSS_RESTKEYVALUESTORE_H_INCLUDED

