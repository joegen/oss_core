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


#ifndef OSS_JSONRPCCLIENT_H_INCLUDED
#define OSS_JSONRPCCLIENT_H_INCLUDED


#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/JSON/Json.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/ClientTransport.h"


namespace OSS {
namespace JSON {

class JsonRpcTransaction
{
public:
  typedef boost::shared_ptr<JsonRpcTransaction> Ptr;
  JsonRpcTransaction(int id) :
    _id(id)
  {
    _pResponse = 0;
  }
  
  ~JsonRpcTransaction()
  {
    delete _pResponse;
  }

  int id() const
  {
    return _id;
  }

  bool read(std::string& response, long timeout)
  {
    return _responseQueue.try_dequeue(response, timeout);
  }

  void queueResponse(json::Object* json, const std::string& response)
  {
    delete _pResponse;
    _pResponse = json;
    _responseQueue.enqueue(response);
  }
  
  json::Object* getJSON()
  {
    return _pResponse;
  }
private:
  int _id;
  OSS::BlockingQueue<std::string> _responseQueue;
  json::Object* _pResponse;
};

typedef std::map<int, JsonRpcTransaction::Ptr> TransactionMap;  

template <typename Connection>
class JsonRpcClient
{
private:
  
  
  JsonRpcTransaction::Ptr createTransaction()
  {
    JsonRpcTransaction::Ptr pTransaction = JsonRpcTransaction::Ptr(new JsonRpcTransaction(generateId()));
    OSS::mutex_critic_sec_lock lock(_transactionsMutex);
    _transactions[pTransaction->id()] = pTransaction;
    return pTransaction;
  }
  
  void destroyTransaction(int id)
  {
    OSS::mutex_critic_sec_lock lock(_transactionsMutex);
    TransactionMap::iterator iter = _transactions.find(id);
    if (iter != _transactions.end())
    {
      _transactions.erase(iter);
    }
  }
  
  JsonRpcTransaction::Ptr findTransaction(int id)
  {
    OSS::mutex_critic_sec_lock lock(_transactionsMutex);
    TransactionMap::iterator iter = _transactions.find(id);
    if (iter != _transactions.end())
    {
      return iter->second;
    }
    return JsonRpcTransaction::Ptr();
  }

public:
  typedef boost::function<void (const std::string&)> CloseHandler;
  typedef boost::function<void (const std::string&)> FailHandler;
  typedef OSS::Net::ClientTransport Transport;
  
  FailHandler failHandler;
  CloseHandler closeHandler;
  std::string identifier;
  
  const std::string& getUrl() const
  {
    return _url;
  }
  
  bool isConnected()
  {
    return _isConnected;
  }
protected:
  int _connectTransactionId;
  std::string _url;
  bool _isConnected;
  bool _isTerminated;
 
public:
  JsonRpcClient()
  {
    _currentId = 0;
    _pEventThread = 0;
    _connectTransactionId = -1;
    _isTerminated = false;
    _isConnected = false;
  }
  
  ~JsonRpcClient()
  {
    close();
  }
  
  void close()
  {
    if (_isTerminated || !_pEventThread)
    {
      return;
    }
    _connection.close();
    _isTerminated = true;
    _pEventThread->join();
    delete _pEventThread;
    _pEventThread = 0;

  }
  
  bool connect(const std::string& url, long timeout = 5000)
  {
    _url = url;
    close();
    if (_pEventThread || _connection.isOpen() || !_connection.connect(url))
    {
      OSS_LOG_ERROR("JsonRpcClient unable to connect to " << url);
      return false;
    }
    JsonRpcTransaction::Ptr pTransaction = createTransaction();
    _connectTransactionId = pTransaction->id();
    _pEventThread = new boost::thread(boost::bind(&JsonRpcClient::processEvents, this));
    
    bool opened = false;
    std::string answer;
    if (pTransaction->read(answer, timeout))
    {
      opened = (answer == "on_open");
      OSS_LOG_DEBUG("JsonRpcClient got signal " << answer);
    }
    else
    {
      OSS_LOG_DEBUG("JsonRpcClient NO signal from server ");
    }
    _connectTransactionId = -1;
    destroyTransaction(pTransaction->id());
    return opened;
  }
  
  void processEvents()
  {
    while (!_isTerminated)
    {
      Transport::EventData event;
      if (!_connection.receive(event, 10))
      {
        continue;
      }
      
      if (event.event == Transport::EventClose) 
      {
        _isConnected = false;
        if (closeHandler)
        {
          closeHandler(identifier);
        }
        if (_connectTransactionId != -1)
        {
          JsonRpcTransaction::Ptr pTransaction = findTransaction(_connectTransactionId);
          if (pTransaction)
          {
            pTransaction->queueResponse(0, "on_close");
          }
        }
        break;
      }
      else if (event.event == Transport::EventFail) 
      {
        _isConnected = false;
        if (failHandler)
        {
          failHandler(identifier);
        }
        break;
      }
      else if (event.event == Transport::EventOpen) 
      {
        _isConnected = true;
        if (_connectTransactionId != -1)
        {
          JsonRpcTransaction::Ptr pTransaction = findTransaction(_connectTransactionId);
          if (pTransaction)
          {
            pTransaction->queueResponse(0, "on_open");
          }
        }
      }
      else if (event.event == Transport::EventMessage)
      {
        _isConnected = true;
        json::Object* pResponse = new json::Object();
        try
        {
          std::stringstream strm;
          strm << event.data;
          json::Reader::Read(*pResponse, strm);
          json::Object::const_iterator iter = pResponse->Find("id");
          if (iter != pResponse->End())
          {
            json::Number id = iter->element;
            JsonRpcTransaction::Ptr pTransaction = findTransaction(id.Value());
            if (pTransaction)
            {
              pTransaction->queueResponse(pResponse, strm.str());
            }
            else
            {
              OSS_LOG_DEBUG("JsonRpcClient got a response for a destroyed transaction.  Message: " << event.data);
              delete pResponse;
            }
          }
        }
        catch(json::Exception& e)
        {
          delete pResponse;
        }
        catch(...)
        {
          delete pResponse;
        }
      }
      else
      {
        // ignore for now
      }
    }
    OSS_LOG_DEBUG("JsonRpcClient thread TERMINATED");
  }
  
  bool call(const std::string& method, const json::Object& params, json::Object& response, long timeout)
  {
    if (!_isConnected)
    {
      OSS_LOG_DEBUG("JsonRpcClient::call method: " << method << " called while client is closed");
      return false;
    }
    
    if (_connection.implementsSendAndReceive())
    {
      return callSendAndReceive(method, params, response, timeout);
    }
    
    JsonRpcTransaction::Ptr pTransaction = createTransaction();

    try
    {
      std::ostringstream requestStrm;
      json::Object request;
      request["jsonrpc"] = json::String("2.0");
      request["method"] = json::String(method);
      request["id"] = json::Number(pTransaction->id());
      request["params"] = params;
      json::Writer::Write(request, requestStrm);
      if (!_connection.send(requestStrm.str()))
      {
        OSS_LOG_DEBUG("Unable to send JSON-RPC request " << method);
        destroyTransaction(pTransaction->id());
        return false;
      }
    }
    catch(json::Exception& e)
    {
      OSS_LOG_DEBUG("Unable to send JSON-RPC request: " << e.what());
      destroyTransaction(pTransaction->id());
      return false;
    }
    catch(std::exception& e)
    {
      OSS_LOG_DEBUG("Unable to send JSON-RPC request: " << e.what());
      destroyTransaction(pTransaction->id());
      return false;
    }
    
    std::string answer;
    bool ret = pTransaction->read(answer, timeout);
    if (ret && !answer.empty() && pTransaction)
    {
      response = *(pTransaction->getJSON());
    }
    else if (!ret)
    {
      OSS_LOG_DEBUG("Unable to receive JSON-RPC response for method " << method);
    }
    destroyTransaction(pTransaction->id());
    return ret;
  }
  
  void notify(const std::string& method, const json::Object& params)
  {
    if (!_isConnected)
    {
      return false;
    }
    
    try
    {
      std::ostringstream requestStrm;
      json::Object request;
      request["jsonrpc"] = json::String("2.0");
      request["method"] = json::String(method);
      request["params"] = params;
      json::Writer::Write(request, requestStrm);
      _connection.send(requestStrm.str());
    }
    catch(json::Exception& e)
    {
      return false;
    }
    catch(std::exception& e)
    {
      return false;
    }
    return true;
  }
  
  int generateId()
  {
    OSS::mutex_critic_sec_lock lock(_currentIdMutex);
    if (_currentId >= 0xFFFF)
    {
      _currentId = 0;
    }
    return ++_currentId;
  }
  
  Connection& connection()
  {
    return _connection;
  }
  
protected:
  bool callSendAndReceive(const std::string& method, const json::Object& params, json::Object& response, long timeout)
  {
    try
    {
      std::ostringstream requestStrm;
      json::Object request;
      request["jsonrpc"] = json::String("2.0");
      request["method"] = json::String(method);
      request["id"] = json::Number(generateId());
      request["params"] = params;
      json::Writer::Write(request, requestStrm);
      std::string result;
      if (!_connection.sendAndReceive(requestStrm.str(), result, timeout))
      {
        OSS_LOG_DEBUG("Unable to send JSON-RPC request " << method);
        return false;
      }
      return OSS::JSON::json_parse_string(result, response);
    }
    catch(json::Exception& e)
    {
      OSS_LOG_DEBUG("Unable to send JSON-RPC request: " << e.what());
      return false;
    }
    catch(std::exception& e)
    {
      OSS_LOG_DEBUG("Unable to send JSON-RPC request: " << e.what());
      return false;
    }
  }
  
private:
  Connection _connection;
  TransactionMap _transactions;
  OSS::mutex_critic_sec _transactionsMutex;
  int _currentId;
  OSS::mutex_critic_sec _currentIdMutex;
  boost::thread* _pEventThread;
};

} }


#endif //OSS_JSONRPCCLIENT_H_INCLUDED

