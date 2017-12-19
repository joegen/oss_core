"use-strict"

var _isolate = require("./_isolate.jso");
const _Isolate = _isolate.Isolate;

var inter_isolate_handler = function(request, handler)
{
  var method = request.method;
  var arguments = request.arguments;
  if (!handler.hasOwnProperty(method))
  {
    var response = new Object();
    response.error = new Object();
    response.error.code = -32601;
    response.error.message = "Method not found";
    return response;
  }
  
  var result;
  try
  {
    result = handler[method](arguments);
  }
  catch(e)
  {
    e.printStackTrace();
    var response = new Object();
    response.error = new Object();
    response.error.code = -32603;
    response.error.message = e;
    return response;
  }
  return JSON.stringify(result);
}

exports.interIsolateHandler = function(request)
{
  return inter_isolate_handler(request, exports._handler);
}

exports._handler = {}
exports.on = function(name, func)
{
  exports._handler[name] = func;
}

exports.remove = function(name)
{
  exports._handler[name] = null;
  delete exports._handler[name];
}

if (_isolate.isRootIsolate())
{
  _isolate.setRootInterIsolateHandler(exports.interIsolateHandler);
}
else
{
  _isolate.setChildInterIsolateHandler(exports.interIsolateHandler);
}

var Isolate = function(threadId)
{
  var isolate = new _Isolate(threadId);
  var _this = this;
  var hadRegisteredHandler = false;
  _this.runSource = function(src)
  {
    return isolate.runSource(src);
  }
  
  _this.join = function()
  {
    return isolate.join();
  }
  
  _this.execute = function(method, args, timeout)
  {
    var request = new Object();
    request.method = method;
    request.arguments = args;
    if (!timeout)
    {
      timeout = 0;
    }
    var json = JSON.stringify(request);
    var result = isolate.execute(json, timeout);
    if (result)
    {
      return JSON.parse(result);
    }
    else
    {
      return undefined;
    }
  }
}

exports.Isolate = Isolate;

