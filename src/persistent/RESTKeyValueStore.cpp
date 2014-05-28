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


#include "OSS/Logger.h"
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"


using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Net::HTTPBasicCredentials;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPRequest; 
using Poco::StreamCopier;
using Poco::Net::HTTPMessage;
  

namespace OSS {
namespace Persistent {


//
// Used for sorting records
//
static bool compare_records (KeyValueStore::Record& first, KeyValueStore::Record& second)
{
  return first.key.compare(second.key) <= 0;
}
  
RESTKeyValueStore::RESTKeyValueStore() :
  _rootDocument(REST_DEFAULT_ROOT_DOCUMENT)
{
  setHandler(boost::bind(&RESTKeyValueStore::onHandleRequest, this, _1, _2));
}
  
RESTKeyValueStore::RESTKeyValueStore(int maxQueuedConnections, int maxThreads) :
  OSS::Net::HTTPServer(maxQueuedConnections, maxThreads),
  _rootDocument(REST_DEFAULT_ROOT_DOCUMENT)
{
  setHandler(boost::bind(&RESTKeyValueStore::onHandleRequest, this, _1, _2));
}

RESTKeyValueStore::~RESTKeyValueStore()
{
  for (KVStore::const_iterator iter = _kvStore.begin(); iter != _kvStore.end(); iter++)
    delete iter->second;
}

KeyValueStore* RESTKeyValueStore::getStore(const std::string& path)
{
  std::vector<std::string> tokens = OSS::string_tokenize(path, "/");
  if(tokens.size() < 3)
    return 0;
  
  OSS::mutex_lock lock(_kvStoreMutex);
  
  KeyValueStore* pStore = 0;
  std::string document = tokens[2];
  
  if (_kvStore.find(document) == _kvStore.end())
  {
    //
    // Create a new store
    //
    std::ostringstream strm;
    
    if (!_dataDirectory.empty())
      strm << _dataDirectory << "/" << document;
    else
      strm << document;
    
    
    pStore = new KeyValueStore();
    pStore->open(strm.str());
    if (pStore->isOpen())
    {
     _kvStore.insert(std::pair<std::string, KeyValueStore*>(document, pStore));
    }
    else
    {
      delete pStore;
      pStore = 0;
    }
  }
  else
  {
    pStore = _kvStore.at(document);
  }
  
  return pStore;
}

bool RESTKeyValueStore::isAuthorized(Request& request, Response& response)
{
  if (_user.empty())
    return true;
  
  if (!request.hasCredentials())
  {
    response.requireAuthentication(request.getURI());
    response.send();
    return false;
  }
  
  HTTPBasicCredentials cred(request);
  
  bool authorized = cred.getUsername() == _user && cred.getPassword() == _password;
  
  if (!authorized)
  {
    response.setStatus(HTTPResponse::HTTP_REASON_FORBIDDEN);
    response.send();
  }
  
  return authorized;
}

void RESTKeyValueStore::onHandleRequest(Request& request, Response& response)
{ 
  if (!isAuthorized(request, response))
  {
    return;
  }
  
  if (OSS::string_starts_with(request.getURI(), _rootDocument.c_str()))
  {
    onHandleRestRequest(request, response);
    return;
  }
  
  
  if (!_data.isOpen())
  {
    response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    response.send();
    return;
  }
  
  std::string value;
  
  HTMLForm form(request, request.stream());
  if (!form.empty() && form.has("key"))
  {
    std::string key = form.get("key");

    if (request.getURI() == "/get")
    {
      _data.get(key, value);
    }
    else if (request.getURI() == "/set" && form.has("value"))
    {
      int expires = 0;
      value = form.get("value");
      
      if (form.has("expires"))
      {
        expires = OSS::string_to_number<int>(form.get("expires").c_str());
        if (expires > 0)
        {
          _data.put(key, value, expires);
        }
        else
        {
          _data.put(key, value);
        }
      }
      else
      {
        _data.put(key, value);
      }
      
    }
    else if (request.getURI() == "/del")
    {
      _data.del(key);
    }
    else if (_customHandler)
    {
      _customHandler(request, response);
      return;
    }
  }
  else if (_customHandler)
  {
    _customHandler(request, response);
    return;
  }
  
  response.setChunkedTransferEncoding(true);
	response.setContentType("text/plain");

	std::ostream& ostr = response.send();
  
  if (!value.empty())
  {
    ostr << value;
  }
}

void RESTKeyValueStore::onHandleRestRequest(Request& request, Response& response)
{
  std::string path = request.getURI();
  if (OSS::string_ends_with(path, "/"))
  {
    path = OSS::string_left(request.getURI(), request.getURI().length() - 1);
  }
  
  KeyValueStore* pStore = getStore(path);
  
  if (!pStore)
  {
    response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    response.send();
    return;
  }
  
  std::vector<std::string> tokens = OSS::string_tokenize(path, "/");
  
  
  if (tokens.size() == 3)
  {
    path = "";
  }
  else if (tokens.size() == 4)
  {
    path = tokens[3];
  }
  else if (tokens.size() == 5)
  {
    path = tokens[3] + std::string("/") + tokens[4];
  }
  else
  {
    response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    response.send();
  }
  
  std::string filter = path + std::string("*");
  
  if (request.getMethod() == HTTPRequest::HTTP_GET)
  {
    KeyValueStore::Records records;
    pStore->getRecords(filter, records);
    
    if (records.empty())
    {
      response.setStatus(HTTPResponse::HTTP_OK);
      response.send();
      return;
    }
    
    sendRestRecordsAsJson(path, records, response);
    
    return;
  }
  else if (request.getMethod() == HTTPRequest::HTTP_DELETE)
  {
    pStore->delKeys(filter);
    response.setStatus(HTTPResponse::HTTP_OK);
    response.send();
    return;
  }
  else if (request.getMethod() == HTTPRequest::HTTP_PUT || request.getMethod() == HTTPRequest::HTTP_POST)
  {
    HTMLForm form(request, request.stream());
    if (!path.empty() && !form.empty() && form.has("value"))
    {
      std::string value = form.get("value");
      pStore->put(path, value);
      response.setStatus(HTTPResponse::HTTP_OK);
      response.send();
      return;
    }
  }
  
  //
  // Send a 404 if it ever gets here
  //
  response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
  response.send();
}

void RESTKeyValueStore::sendRestRecordsAsJson(const std::string& path, const KeyValueStore::Records& records, Response& response)
{
  //
  // sort the records
  //
  std::list<KeyValueStore::Record> sorted;
  std::copy( records.begin(), records.end(), std::back_inserter(sorted));
  sorted.sort(compare_records);
  
  
  response.setChunkedTransferEncoding(true);
  response.setContentType("text/plain");
  std::ostream& ostr = response.send();

  for (std::list<KeyValueStore::Record>::const_iterator iter = sorted.begin(); iter != sorted.end(); iter++)
  {
    ostr << iter->key << ": " << iter->value << "\r\n";
  }
}


void RESTKeyValueStore::sendRestRecordsAsValuePairs(const std::string& path, const KeyValueStore::Records& records, Response& response)
{
  //
  // sort the records
  //
  std::list<KeyValueStore::Record> sorted;
  std::copy( records.begin(), records.end(), std::back_inserter(sorted));
  sorted.sort(compare_records);
  
  
  response.setChunkedTransferEncoding(true);
  response.setContentType("text/plain");
  std::ostream& ostr = response.send();

  for (std::list<KeyValueStore::Record>::const_iterator iter = sorted.begin(); iter != sorted.end(); iter++)
  {
    ostr << iter->key << ": " << iter->value << "\r\n";
  }
}


RESTKeyValueStore::Client::Client(const std::string& host, unsigned short port) :
  _secure(false),
  _host(host),
  _port(port),
  _sessionHandle(0)
{
}

RESTKeyValueStore::Client::Client(const std::string& host, unsigned short port, bool secure) :
  _secure(secure),
  _host(host),
  _port(port),
  _sessionHandle(0)
{
}

RESTKeyValueStore::Client::~Client()
{
  delete (HTTPClientSession*)_sessionHandle;
}
    
bool RESTKeyValueStore::Client::kvSet(const std::string& key, const std::string& value)
{
  return kvSet(key, value, -1);
}

bool RESTKeyValueStore::Client::kvSet(const std::string& key, const std::string& value, int expires)
{
  Params params;
  params["key"] = key;
  params["value"] = value;
  
  if (expires > 0)
  {
    params["expire"] = OSS::string_from_number<int>(expires);
  }
  
  std::string result;
  int status = 0;
  return execute_POST("/set", params, result,  status);
}

bool RESTKeyValueStore::Client::kvGet(const std::string& key, std::string& value)
{
  Params params;
  params["key"] = key;
  int status = 0;
  return execute_POST("/get", params, value, status);
}

bool RESTKeyValueStore::Client::kvDelete(const std::string& key)
{
  Params params;
  params["key"] = key; 
  std::string value;
  int status = 0;
  return execute_POST("/del", params, value, status);
}

void RESTKeyValueStore::Client::setCredentials(const std::string& user, const std::string& password)
{
  _user = user;
  _password = password;
}

bool RESTKeyValueStore::Client::execute_POST(const std::string& path, const Params& params, std::string& result, int& status)
{
  return execute(HTTPRequest::HTTP_POST, path, params, result, status);
}

bool RESTKeyValueStore::Client::execute_GET(const std::string& path, const Params& params, std::string& result, int& status)
{
  return execute(HTTPRequest::HTTP_GET, path, params, result, status);
}

bool RESTKeyValueStore::Client::execute_PUT(const std::string& path, const Params& params, std::string& result, int& status)
{
  return execute(HTTPRequest::HTTP_PUT, path, params, result,status);
}

bool RESTKeyValueStore::Client::execute_DELETE(const std::string& path, const Params& params, std::string& result, int& status)
{
  return execute(HTTPRequest::HTTP_DELETE, path, params, result, status);
}

bool RESTKeyValueStore::Client::execute(const std::string& method, const std::string& path, const Params& params, std::string& result, int& status)
{
  status = 0;
    
  try
  {
    HTTPClientSession* pSession = 0;

    if (!_sessionHandle)
    {
      if (!_secure)
        pSession = new HTTPClientSession(_host, _port);
      else
        pSession = new HTTPSClientSession(_host, _port);
        
      _sessionHandle = (OSS_HANDLE)pSession;
    }
    else
    {
      pSession = (HTTPClientSession*)_sessionHandle;
    }

         
    HTTPRequest req(method, path, HTTPMessage::HTTP_1_1);
    
    if (!_user.empty())
    {
      HTTPBasicCredentials cred(_user, _password);
      cred.authenticate(req);
    }
    
    if (!params.empty())
    {
      HTMLForm form;
      for (Params::const_iterator iter = params.begin(); iter != params.end(); iter++)
      {
        form.add(iter->first, iter->second);
      }

      // Send the request.
      form.prepareSubmit(req);
      std::ostream& ostr = pSession->sendRequest(req);
      form.write(ostr);
    }
    else
    {
      pSession->sendRequest(req);
    }
    
    // Receive the response.
	  HTTPResponse res;
    
    std::istream& rs = pSession->receiveResponse(res);
    std::ostringstream strm;
    StreamCopier::copyStream(rs, strm);
    result = strm.str();

    status = res.getStatus();
    
    return (status == HTTPResponse::HTTP_OK);
  }
  catch(Poco::Exception e)
  {
    OSS_LOG_ERROR("RESTKeyValueStore::Client::execute Exception: " << e.message())
    delete (HTTPClientSession*)_sessionHandle;
    _sessionHandle = 0;
    return false;
  }
}

bool RESTKeyValueStore::Client::restPUT(const std::string& path, const std::string& value, int& status)
{
  Params params;
  params["value"] = value;
  std::string result;
  return execute_PUT(path, params, result, status);
}
    
bool RESTKeyValueStore::Client::restGET(const std::string& path, std::string& result, int& status)
{
  Params params;
  return execute_GET(path, params, result, status);
}

bool RESTKeyValueStore::Client::restDELETE(const std::string& path, int& status)
{
  Params params;
  std::string result;
  return execute_GET(path, params, result, status);
}
    
} }




