"use-strict";

const async = require("async");

var CPPRPC = function(request)
{
  
  var method = request.method;
  var arguments = request.arguments;
  
  if (!CPPRPC.hasOwnProperty(method))
  {
    var response = new Object();
    response.error = new Object();
    response.error.code = -32601;
    response.error.message = "Method not found";
    return response;
  }
  
  var result = CPPRPC[method](arguments);
  return JSON.stringify(result);
}

CPPRPC.on = function(name, func)
{
  CPPRPC[name] = func;
}

async.__set_promise_callback(CPPRPC);

exports.CPPRPC = CPPRPC;
