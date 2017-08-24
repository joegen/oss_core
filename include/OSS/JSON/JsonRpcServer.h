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


#ifndef OSS_JSONRPCSERVER_H_INCLUDED
#define OSS_JSONRPCSERVER_H_INCLUDED


#include "OSS/JSON/Json.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/ServerTransport.h"

namespace OSS {
namespace JSON {
  
class JsonRpcServerHandler
{
public:
  JsonRpcServerHandler() {};
  virtual ~JsonRpcServerHandler() {};
  virtual void onOpen(int connectionId) = 0;
  virtual void onClose(int connectionId) = 0;
  virtual void onError(int connectionId, const std::string& error) = 0;
  virtual void onPing(int connectionId) = 0;
  virtual void onPong(int connectionId) = 0;
  virtual void onPongTimeout(int connectionId) = 0;
};
  
typedef boost::function<bool(const OSS::JSON::Object& /*params*/, OSS::JSON::Object& /*result*/, int& /*error code*/, std::string& /*error message*/)> JsonRpcProcedure;

template<typename Transport>
class JsonRpcServer
{
protected:
  
  class ServerTransport : public Transport
  {
  public:
    ServerTransport(JsonRpcServer<Transport>& rpc) :
      _rpc(rpc)
    {
    }
    virtual ~ServerTransport()
    {
    }
    
    virtual void onOpen(int connectionId)
    {
      _rpc.onOpen(connectionId);
    }
  
    virtual void onClose(int connectionId)
    {
      _rpc.onClose(connectionId);
    }

    virtual void onError(int connectionId, const std::string& error)
    {
      _rpc.onError(connectionId, error);
    }

    virtual void onMessage(int connectionId, const std::string& message)
    {
      _rpc.onMessage(connectionId, message);
    }

    virtual void onPing(int connectionId)
    {
      _rpc.onPing(connectionId);
    }

    virtual void onPong(int connectionId)
    {
      _rpc.onPong(connectionId);
    }

    virtual void onPongTimeout(int connectionId)
    {
      _rpc.onPongTimeout(connectionId);
    }
    
  private:
    JsonRpcServer<Transport>& _rpc;
  };

public:
  
  typedef json::Array Array;
  typedef json::Boolean Boolean;
  typedef json::Number Number;
  typedef json::Object Object;
  typedef json::String String;
  typedef std::map<std::string, JsonRpcProcedure> Procedures;
  
  JsonRpcServer() :
    _transport(*this),
    _pHandler(0)
  {
  }
  
  ~JsonRpcServer()
  {
    delete _pHandler;
  }
  
  bool listen(const std::string& bindAddress)
  {
    return _transport.listen(bindAddress);
  }
  
  void close()
  {
    _transport.close();
  }
  
  void setHandler(JsonRpcServerHandler* pHandler)
  {
    _pHandler = pHandler;
  }
  void registerProcedure(const std::string& method, const JsonRpcProcedure& procedure)
  {
    _procedures[method] = procedure;
  }
  
  void createErrorObject(Object& result, int id, int code, const std::string& message)
  {
    Object error;
    result["id"] = Number(id);
    result["jsonrpc"] = String("2.0");
    error["code"] = Number(code);
    error["message"] = String(message);
    result["error"] = error;
  }
  
  void createResponseObject(const Object& result, int id, Object& response)
  {
    response["id"] = Number(id);
    response["jsonrpc"] = String("2.0");
    response["result"] = result;
  }
  
  enum ServerErrorCodes
  {
    ParseError = -32700, 
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603,
    ServerError = -32000
  };
  
  void tryBatchMessages(int connectionId, const std::string& message)
  {
    //
    // TODO:  To fully support 2.0 specs, we need to implement batch processing
    //
  }
  
  void onMessage(int connectionId, const std::string& message)
  {
    OSS_LOG_DEBUG("JsonRpcServer processing message from Connection " << connectionId);
    Object object;
    Object result;
    int errorCode = 0;
    int rpcId = -1;
    std::string errorMessage;
    
    if (!OSS::JSON::json_parse_string(message, object))
    {
      //
      // If parsing the message as an Object fails,
      // there is a chance that it is an Array.
      // We failover to batch processing
      //
      OSS_LOG_DEBUG("JsonRpcServer unable to parse message as Object from Connection " << connectionId);
      tryBatchMessages(connectionId, message);
      return;
    }
    
 
    try
    {
      String method = object["method"];
      Number id = object["id"];
      Object params = object["params"];
      
      rpcId = id.Value();
      //
      // Check if method is registered
      //
      OSS_LOG_DEBUG("JsonRpcServer finding method " << method.Value() << " Connection " << connectionId);
      Procedures::iterator proc = _procedures.find(method.Value());
      if (proc == _procedures.end())
      {
        errorCode = MethodNotFound;
        createErrorObject(result, rpcId, errorCode, "Invalid procedure name");
        OSS_LOG_DEBUG("JsonRpcServer unable to find method" << method.Value() << " Connection " << connectionId);
      }
      else
      {
        JsonRpcProcedure func = proc->second;
        if (func)
        {
           OSS_LOG_DEBUG("JsonRpcServer invoking method " << method.Value() << " Connection " << connectionId);
          if (!func(params, result, errorCode, errorMessage) || errorCode != 0)
          {
            if (errorCode == 0)
            {
              errorCode = ServerError; 
              errorMessage = "Unknown Server Exception";
            }
            OSS_LOG_DEBUG("JsonRpcServer method " << method.Value() << " Connection " << connectionId << " returned false.  Error=" << errorCode);
            createErrorObject(result, rpcId, errorCode, errorMessage);
          }
          else
          {
            errorCode = 0;
          }
        }
      }
    }
    catch(std::exception& e)
    {
      OSS_LOG_DEBUG("Unable to parse JSON payload - " << e.what());
      return;
    }
    
    if (errorCode != 0)
    {
      std::string  replyStr;
      OSS::JSON::json_object_to_string(result, replyStr);
      OSS_LOG_DEBUG("JsonRpcServer sending ERROR response to Connection " << connectionId << " Response: " << replyStr);
      _transport.sendMessage(connectionId, replyStr);
    }
    else
    {
      std::string  replyStr;
      Object response;
      createResponseObject(result, rpcId, response);
      OSS::JSON::json_object_to_string(response, replyStr);
      OSS_LOG_DEBUG("JsonRpcServer sending SUCCESS response to Connection " << connectionId << " Response: " << replyStr);
      _transport.sendMessage(connectionId, replyStr);
    }
  }
  
  void onOpen(int connectionId)
  {
    if (_pHandler)
    {
      _pHandler->onOpen(connectionId);
    }
  }

  void onClose(int connectionId)
  {
    if (_pHandler)
    {
      _pHandler->onClose(connectionId);
    }
  }

  void onError(int connectionId, const std::string& error)
  {
    if (_pHandler)
    {
      _pHandler->onError(connectionId, error);
    }
  }

  void onPing(int connectionId)
  {
    if (_pHandler)
    {
      _pHandler->onPing(connectionId);
    }
  }

  void onPong(int connectionId)
  {
    if (_pHandler)
    {
      _pHandler->onPong(connectionId);
    }
  }

  void onPongTimeout(int connectionId)
  {
    if (_pHandler)
    {
      _pHandler->onPongTimeout(connectionId);
    }
  }
  
protected:
  ServerTransport _transport;
  Procedures _procedures;
  JsonRpcServerHandler* _pHandler;
};

} } // OSS::JSON

#endif // OSS_JSONRPCSERVER_H_INCLUDED

