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
  
  var result;
  try
  {
    result = CPPRPC[method](arguments);
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

CPPRPC.on = function(name, func)
{
  CPPRPC[name] = func;
}

CPPRPC.remove = function(name)
{
  delete CPPRPC[name];
}

async.__set_promise_callback(CPPRPC);

exports.CPPRPC = CPPRPC;
