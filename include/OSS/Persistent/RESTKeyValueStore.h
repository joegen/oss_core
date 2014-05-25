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
  
  class Client
  {
  public:
    typedef std::map<std::string, std::string> Params;
    
    Client(const std::string& host, unsigned short port);
    
    Client(const std::string& host, unsigned short port, bool secure);
    
    ~Client();
    
    bool kvSet(const std::string& key, const std::string& value);
    
    bool kvSet(const std::string& key, const std::string& value, int expires);
    
    bool kvGet(const std::string& key, std::string& value);
    
    bool kvDelete(const std::string& key);
    
    bool restPUT(const std::string& path, const std::string& value, int& status);
    
    bool restGET(const std::string& path, std::string& result, int& status);
    
    bool restDELETE(const std::string& path, int& status);
    
    void setCredentials(const std::string& user, const std::string& password);
    
    bool execute_POST(const std::string& path, const Params& params, std::string& result, int& status);
    
    bool execute_GET(const std::string& path, const Params& params, std::string& result, int& status);
    
    bool execute_PUT(const std::string& path, const Params& params, std::string& result, int& status);
    
    bool execute_DELETE(const std::string& path, const Params& params, std::string& result, int& status);
    
    bool execute(const std::string& method, const std::string& path, const Params& params, std::string& result, int& status);
    
  private:
    bool _secure;
    std::string _host;
    unsigned short _port;
    std::string _user;
    std::string _password;
    OSS_HANDLE _sessionHandle;
  };
  
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
  _data.open(dataFile); return _data.isOpen();
}

inline void RESTKeyValueStore::setCredentials(const std::string& user, const std::string& password)
{
  _user = user; _password = password;
}

inline void RESTKeyValueStore::setCustomHandler(const Handler& handler)
{
  _customHandler = handler;
}

} }

#endif	// OSS_RESTKEYVALUESTORE_H_INCLUDED
