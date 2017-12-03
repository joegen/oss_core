"use-strict";

const JsonRpcRequest = require("./json_rpc_request.js").JsonRpcRequest;
const JsonRpcResponse = require("./json_rpc_response.js").JsonRpcResponse;
const JsonRpcError = require("./json_rpc_error.js").JsonRpcError;

var JsonRpcClient = function(transport)
{
  this._transport = transport;
  this._id = 1;
  this._callback = undefined;
  var _this = this;
  
  this.setTransport = function(transport) { _this._transport = transport; };
  this.getTransport = function() { return _this._transport; };
    
  this._transport.onReadCompleted = function(response)
  {
    var rpcResponse = new JsonRpcResponse();
    rpcResponse.fromJSON(response)
    _this._callback(rpcResponse);
  }
  
  this.execute = function(method, params, callback)
  {
    _this._callback = callback;
    var request = new JsonRpcRequest(method, params);
    request.setId(_this._id++);
    _this._transport.send(request.toJSON());
  }
}

exports.JsonRpcClient = JsonRpcClient;
