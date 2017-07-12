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
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Logger.h"


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

  void queueReponse(json::Object* json, const std::string& response)
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

template <typename Connection, typename EventData>
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
  
  FailHandler failHandler;
  CloseHandler closeHandler;
  std::string identifier;
  
  JsonRpcClient()
  {
    _currentId = 0;
    _pEventThread = 0;
  }
  
  ~JsonRpcClient()
  {
    close();
  }
  
  void close()
  {
    _connection.close();
    if (_pEventThread)
    {
      _pEventThread->join();
      delete _pEventThread;
      _pEventThread = 0;
    }
  }
  
  bool connect(const std::string& url)
  {
    close();
    if (_pEventThread || _connection.isOpen() || !_connection.connect(url))
    {
      return false;
    }
    _pEventThread = new boost::thread(boost::bind(&JsonRpcClient::processEvents, this));
    return true;
  }
  
  void processEvents()
  {
    while (true)
    {
      EventData event;
      _connection.receive(event);
      
      if (event.event == "on_close") 
      {
        if (closeHandler)
        {
          closeHandler(identifier);
        }
        break;
      }
      else if (event.event == "on_fail") 
      {
        if (failHandler)
        {
          failHandler(identifier);
        }
        break;
      }
      else if (event.event == "on_message" )
      {
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
              pTransaction->queueReponse(pResponse, strm.str());
            }
            else
            {
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
  }
  
  bool call(const std::string& method, const json::Object& params, json::Object& response, long timeout)
  {
    JsonRpcTransaction::Ptr pTransaction = createTransaction();
    if (!pTransaction)
    {
      return false;
    }

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
      OSS_LOG_DEBUG("Unable to receive JSON-RPC response");
    }
    destroyTransaction(pTransaction->id());
    return ret;
  }
  
  void notify(const std::string& method, const json::Object& params)
  {
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

