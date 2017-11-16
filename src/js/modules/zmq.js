"use-strict";
const _zmq = require("_zmq");
const async = require("async");
const poll = require("poll");
const log = require("logger");
const system = require("system");
const buffer = require("buffer");
const assert = require("assert");
const Fifo = require("fifo").Fifo;

__copy_exports(_zmq, exports);

const READ_BUFFER_SIZE = 1024 * 16;
var READ_BUFFER = new Buffer(READ_BUFFER_SIZE);

exports.ZMQSocket.prototype.start = function(callback)
{
  if (typeof callback === "function")
  {
    this._callback = callback;
    async.monitorFd(this.getFd(), callback);
  }
}

exports.ZMQSocket.prototype.close = function()
{
  if (typeof this._callback === "function")
  {
    async.unmonitorFd(this.getFd());
    this._close();
  }
}


/////////////////////////// RPC SERVER /////////////////////////////////

var ZmqRpcError = function(code, message, data)
{
  this.code = code;
  this.message = message;
  this.data = data;
}

var ZmqRpcRequest = function(method, params)
{
  this.jsonrpc = "2.0";
  this.id = ++ZmqRpcRequest._currentId;
  this.method = method;
  this.params = params;
  if (ZmqRpcRequest._currentId == Number.MAX_VALUE)
  {
    ZmqRpcRequest._currentId = 0;
  }
}
ZmqRpcRequest._currentId = 0;

var ZmqRpcServer = function(address)
{
  this._socket = new zmq.ZMQSocket(zmq.REP);
  
  if (!this._socket.bind(address))
  {
    throw new Error("Unable to bind to address " + address);
  }
  
  var _this = this;
  this.onNewRequest = function()
  {
    var request = _this._socket.receive(READ_BUFFER);
    if (!buffer.isBuffer(request))
    {
      return _this._socket.send(JSON.stringify(new ZmqRpcError(-32603, "Internal JSON-RPC error")));
    }
    
    log_info("--->" + request.payloadSize)
    log_info("------->" + request.toString(request.payloadSize));
    
    var rpc = JSON.parse(request.toString(request.payloadSize));
    var method = rpc.method;
    var params = rpc.params;

    var response = new Object();
    response.jsonrpc = "2.0";
    response.id = rpc.id;

    var procedure;
    if (typeof method === "string" && _this.hasMethod(method))
    {
      procedure = _this.getMethod(method);
    }

    if (typeof procedure === "function")
    {
      var procedure = _this.getMethod(method);
      response.result = procedure(params);
      return _this._socket.send(JSON.stringify(response));
    }
    else
    {
      return _this._socket.send(JSON.stringify(new ZmqRpcError(-32601, "Method not found")));
    }
  }
  
  this._socket.start(this.onNewRequest);
}



ZmqRpcServer.prototype.on = function(name, method)
{
  this[name] = method;
}

ZmqRpcServer.prototype.hasMethod = function(name)
{
  var method = this.getMethod(name);
  return typeof method === "function";
}

ZmqRpcServer.prototype.getMethod = function(name)
{
  if (this.hasOwnProperty(name))
  {
    var method = this[name];
   if (typeof method === "function")
   {
     return method;
   }
  }
}

/////////////////////////// RPC CLIENT /////////////////////////////////

var ZmqRpcClient = function(address)
{
  this._socket = new zmq.ZMQSocket(zmq.REQ);
  this._activeMthod = "";
  this._expectingResponse = false;
  this._fifo = new Fifo();
  
  var _this = this;
  
  if (!this._socket.connect(address))
  {
    throw new Error("Unable to connect to address " + address);
  }
  
  this.onNewResponse = function()
  {
    _this._expectingResponse = false;
    var response = _this._socket.receive(READ_BUFFER);
    if (!buffer.isBuffer(response))
    {
      return _this._socket.send(JSON.stringify(new ZmqRpcError(-32603, "Internal JSON-RPC error")));
    }
    
    var rpc = JSON.parse(response.toString(response.payloadSize));
    var method = _this._activeMethod;

    // Call pending call to excute prior to calling callbacks
    // so that new sends would line up at the back of the stack
    _this.callPending();

    if (typeof rpc !== "object")
    {
      _this.onError(method, new ZmqRpcError(-32700, "Parse Error"));
    }
    else if (typeof rpc.error === "object")
    {
      _this.onError(method, rpc.error);
    }
    else if (typeof rpc.result !== "undefined")
    {
      var proc = _this.getMethod(method);
      if (typeof proc === "function")
      {
        proc(rpc.result);
      }
    }
    else
    {
      _this.onError(method, new ZmqRpcError(-32603, "Internal Error"));
    }
  }
  
  this._socket.start(this.onNewResponse);
}

ZmqRpcClient.prototype.onError = function(method, error)
{
  // meant to be implemented by application
}

ZmqRpcClient.prototype.on = function(name, method)
{
  if (name === "error")
  {
    this.onError = method;
  }
  else
  {
    this[name] = method;
  }
}

ZmqRpcClient.prototype.hasMethod = function(name)
{
  var method = this.getMethod(name);
  return typeof method === "function";
}

ZmqRpcClient.prototype.getMethod = function(name)
{
  if (this.hasOwnProperty(name))
  {
    var method = this[name];
    if (typeof method === "function")
    {
      return method;
    }
  }
}

ZmqRpcClient.prototype.execute = function(proc, params)
{
  if (!this.hasMethod(proc))
  {
    this.onError(proc, new ZmqRpcError(-32600, "Invalid Procedure"));
    return;
  }
  
  var request = new ZmqRpcRequest(proc, params);
  if (this._expectingResponse)
  {
    this._fifo.push(request);
    return;
  }
  this._activeMethod = proc;
  this._expectingResponse = true;
  this._socket.send(JSON.stringify(request));
}

ZmqRpcClient.prototype.callPending = function()
{
  if (this._expectingResponse || this._fifo.empty())
  {
    return;
  }
  var request = this._fifo.pop();
  this._activeMethod = request.method;
  this._expectingResponse = true;
  this._socket.send(JSON.stringify(request));
}

exports.ZmqRpcServer = ZmqRpcServer;
exports.ZmqRpcClient = ZmqRpcClient;





