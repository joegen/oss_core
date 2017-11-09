"use-strict";
const _zmq = require("_zmq");
const async = require("async");
const poll = require("poll");
const log = require("logger");
const system = require("system");
const buffer = require("buffer");
const assert = require("assert");

__copy_exports(_zmq, exports);

exports.ZMQSocket.prototype.start = function(callback)
{
  async.monitorFd(this.getFd(), callback);
}

exports.ZMQSocket.prototype.close = function()
{
  async.unmonitorFd(this.getFd());
  this._close();
}

//
// RPC Server Functions
//
function handle_rpc_request(request)
{
  var rpc = JSON.parse(request);
  var method = rpc.method;
  var params = rpc.params;
  if (typeof this.rpc_handlers[method] === "function")
  {
    return JSON.stringify(this.rpc_handlers[method](params));
  }
}

exports.zmq_rpc_server = function(address, handlers)
{
  handle_rpc_request.rpc_handlers = handlers;
  var rpc_server = new zmq.ZMQSocket(zmq.REP);
  if (!rpc_server.bind(address))
  {
    rpc_server = null;
    return;
  }
  rpc_server.start(function()
  {
    var result = handle_rpc_request(rpc_server.receive());
    var response = new Buffer(result);
    rpc_server.send(response);
  });
  return rpc_server;
}

//
// RPC Client function
//
exports.zmq_rpc_client = function(address)
{
  var rpc_client = new zmq.ZMQSocket(zmq.REQ);
  if (!rpc_client.connect(address))
  {
    rpc_client = null;
    return;
  }
  rpc_client.start(function()
  {
    var msg = requester.rpc_client();
    var rpc = JSON.parse(msg);
    rpc_client.callback(rpc);
  });
  
  var id = 0;
  
  rpc_client.call = function(method, params, callback)
  {
    var rpc;
    rpc.jsonrpc = "2.0";
    rpc.method = method;
    rpc.params = params;
    rpc.id = ++id;
    rpc_client.send(JSON.stringify(rpc));
    rpc_client.callback = callback;
  }
  
  return rpc_client;
}
