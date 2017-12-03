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

#ifndef OSS_JS_HTTPSERVEROBJECT_H_INCLUDED
#define OSS_JS_HTTPSERVEROBJECT_H_INCLUDED

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/UTL/Thread.h"

class HttpServerObject : public OSS::JS::ObjectWrap
{
public:
  typedef Poco::Net::MessageException MessageException;
  typedef Poco::NoThreadAvailableException NoThreadAvailableException;
  typedef Poco::Net::ServerSocket ServerSocket;
  typedef Poco::Net::SocketAddress SocketAddress;
  typedef Poco::Net::IPAddress IPAddress;
  typedef Poco::Net::HTTPRequestHandler HTTPRequestHandler;
  typedef Poco::Net::HTTPRequestHandlerFactory HTTPRequestHandlerFactory;
  typedef Poco::Net::HTTPServer HTTPServer;
  typedef Poco::Net::HTTPServerRequest HTTPServerRequest;
  typedef Poco::Net::HTTPServerResponse HTTPServerResponse;
  typedef Poco::Net::HTTPServerParams HTTPServerParams;
  typedef Poco::Net::HTTPResponse::HTTPStatus HTTPStatus;
  typedef std::map<int, std::istream*> InputStreamMap;
  typedef std::map<int, std::ostream*> OutputStreamMap;
  typedef std::map<int, HTTPServerResponse*> ResponseMap;
  
  JS_CONSTRUCTOR_DECLARE();

  //
  // Methods
  //
  JS_METHOD_DECLARE(setMaxQueued);
  JS_METHOD_DECLARE(setMaxThreads);
  JS_METHOD_DECLARE(_listen);
  JS_METHOD_DECLARE(_read);
  JS_METHOD_DECLARE(_write);
  JS_METHOD_DECLARE(_setRpcId);
  JS_METHOD_DECLARE(_sendResponse);

  void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
  void processError(OSS::JSON::Object& result, HTTPServerResponse& response);
  int storeInputStream(std::istream* strm);
  std::istream* findInputStream(int id);
  int storeOutputStream(int id, std::ostream* strm);
  std::ostream* findOutputStream(int id);
  void removeInputStream(int id);
  void removeOutputStream(int id);
  uint32_t write(int streamId, const char* data, uint32_t len);
  uint32_t read(int streamID, char* data, uint32_t len);
  int storeResponse(HTTPServerResponse* pResponse);
  HTTPServerResponse* findResponse(int id);
  void removeResponse(int id);
private:
  HttpServerObject();
  virtual ~HttpServerObject();
  HTTPServerParams* _pParams;
  ServerSocket* _pSocket;
  HTTPServer* _pHttpServer;
  std::string _rpcId;
  JSPersistentFunctionHandle _requestHandler;
  InputStreamMap _inputStreams;
  OutputStreamMap _outputStreams;
  int _inputStreamId;
  OSS::mutex_critic_sec _inputStreamsMutex;
  OSS::mutex_critic_sec _outputStreamsMutex;
  ResponseMap _responses;
  OSS::mutex_critic_sec _responsesMutex;
  int _responseId;
};

#endif // OSS_JS_HTTPSERVEROBJECT_H_INCLUDED

