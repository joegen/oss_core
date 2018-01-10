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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/JS/JSEventLoop.h"

static void test_ping(OSS::JS::JSIsolate::Ptr pIsolate)
{
  OSS::JSON::Object ping, arguments, pong;
  ping["method"] = OSS::JSON::String("ping");
  arguments["message"] = OSS::JSON::String("Hello Inter Isolate");
  ping["arguments"] = arguments;
  
  if (!pIsolate->eventLoop()->interIsolate().execute(ping, pong, 0, new std::string("Sample User Data")))
  {
    OSS_LOG_ERROR("json_execute_promise failed for method ping");
  }
  else
  {
    OSS::JSON::Object::iterator iter = pong.Find("result");
    if (iter == pong.End())
    {
      OSS_LOG_ERROR("json_execute_promise returned response with no result for method ping");
    }
    else
    {
      OSS::JSON::String result = iter->element;
      OSS_LOG_INFO("json_execute_promise returned result " << result.Value());
    }
  }
}

JS_METHOD_IMPL(start_test)
{
  js_enter_scope();
  new boost::thread(test_ping, OSS::JS::JSIsolateManager::instance().getIsolate());
  return JSUndefined();
}

JS_METHOD_IMPL(parse_user_data)
{
  js_enter_scope();
  std::string* userData = js_method_arg_unwrap_object(std::string, 0);
  return JSString(userData->c_str());
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_method("start_test", start_test);
  js_export_method("parse_user_data", parse_user_data);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSCPPRPCTester);

