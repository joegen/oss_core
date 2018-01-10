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

#include "OSS/JS/JSInterIsolateCallManager.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/JS/JSEventLoop.h"


namespace OSS {
namespace JS {



JSInterIsolateCallManager::JSInterIsolateCallManager(JSEventLoop* pEventLoop) :
  JSEventLoopComponent(pEventLoop)
{
}

JSInterIsolateCallManager::~JSInterIsolateCallManager()
{
}

void JSInterIsolateCallManager::enqueue(const JSInterIsolateCall::Ptr& pCall)
{
  OSS::mutex_critic_sec_lock lock(_queueMutex);
  _queue.push(pCall);
}

JSInterIsolateCall::Ptr JSInterIsolateCallManager::dequeue()
{
  OSS::mutex_critic_sec_lock lock(_queueMutex);
  JSInterIsolateCall::Ptr pCall;
  if (_queue.empty())
  {
    return JSInterIsolateCall::Ptr();
  }
  pCall = _queue.front();
  _queue.pop();
  return pCall;
}

bool JSInterIsolateCallManager::doOneWork()
{
  JSInterIsolateCall::Ptr pCall = dequeue();
  if (!pCall)
  {
    return false;
  }
  
  if (_handler.empty())
  {
    //
    // return true here so that the event loop knows we processed something
    //
    pCall->setValue("{error: 'No handler set'}");
    return true;
  }
  
  js_enter_scope();
  JSLocalObjectHandle pUserData = getIsolate()->wrapExternalPointer(pCall->getUserData());
  JSValueHandle request = getIsolate()->parseJSON(pCall->json());
  JSArgumentVector jsonArg;
  jsonArg.push_back(request);
  jsonArg.push_back(pUserData);

  JSValueHandle result =  _handler.value()->Call(getGlobal(), jsonArg.size(), jsonArg.data());

  std::string value;
  if (!result.IsEmpty() && result->IsString())
  {
    value = js_handle_as_std_string(result);
  }
  pCall->setValue(value);
  return true;
}

bool JSInterIsolateCallManager::execute(const Request& request, Result& result, uint32_t timeout, void* userData)
{
  bool delegateToSelf = getIsolate()->isThreadSelf();
  JSInterIsolateCall::Ptr pCall(new JSInterIsolateCall(request, timeout, userData));
  enqueue(pCall);

  if (delegateToSelf)
  {
    doOneWork();
  }
  else
  {
    getEventLoop()->wakeup();
  }
  
  if (!pCall->waitForResult())
  {
    return false;
  }
  result = pCall->getResult();
  return true;
}

bool JSInterIsolateCallManager::execute(const std::string& requestStr, std::string& resultStr, uint32_t timeout, void* userData)
{
  Request request; Result result;
  if (!OSS::JSON::json_parse_string(requestStr, request) || !execute(request, result, timeout, userData))
  {
    return false;
  }
  return OSS::JSON::json_object_to_string(result, resultStr);
}

void JSInterIsolateCallManager::notify(const std::string& requestStr, void* userData)
{
  Request request;
  if (OSS::JSON::json_parse_string(requestStr, request))
  {
    notify(request, userData);
  }
}

void JSInterIsolateCallManager::notify(const Request& request, void* userData)
{
  bool delegateToSelf = getIsolate()->isThreadSelf();
  JSInterIsolateCall::Ptr pCall(new JSInterIsolateCall(request, 0, userData));
  enqueue(pCall);

  if (delegateToSelf)
  {
    doOneWork();
  }
  else
  {
    getEventLoop()->wakeup();
  }
}

} }


