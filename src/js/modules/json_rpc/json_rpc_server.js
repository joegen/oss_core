"use-strict";

var JsonRpcRequest = require("./json_rpc_request.js").JsonRpcRequest;
var JsonRpcResponse = require("./json_rpc_response.js").JsonRpcResponse;
var JsonRpcError = require("./json_rpc_error.js").JsonRpcError;

var JsonRpcServer = function(transport, handlers) {
  this._transport = transport;
  this._handlers = handlers;
  this._currentId = 0;
  this._canSendResponse = false;
  this.setTransport = function(transport) {
    this._transport = transport;
  };
  this.getTransport = function() {
    return this._transport;
  };

  var _this = this;
  this._transport.onNewRequest = function(request) {
    var jsonRpcRequest = new JsonRpcRequest();
    _this._canSendResponse = true;
    if (jsonRpcRequest.fromJSON(request)) {
      _this._currentId = jsonRpcRequest.getId();
      _this._currentMethod = jsonRpcRequest.getMethod();

      if (_this._handlers.hasOwnProperty(_this._currentMethod)) {

        var response = undefined;
        try {
          var response = _this._handlers[_this._currentMethod](jsonRpcRequest.getParams());
          response.setId(_this._currentId);
          if (response instanceof JsonRpcResponse) {
            _this.send(response);
          } else {
            _this.sendError(-32603, "I'm a teapot");
          }
        } catch (e) {
          console.printStackTrace(e);
          _this.sendError(-32603, "Exception occured: " + e);
        }
      } else {
        _this.sendError(-32601, "Method not found", undefined);
      }
    } else {
      //
      // This will send the wrong ID but we need to respond anyway so just let it be
      //
      _this.sendError(-32700, "Parser error");
    }
  }

  this.createResponse = function() {
    var response = new JsonRpcResponse();
    response.setId(this._currentId);
    return response;
  }

  this.send = function(response) {
    if (!this._canSendResponse || response.getId() !== this._currentId) {
      if (!this._canSendResponse) {
        throw new Error("Invalid State");
      } else {
        throw new Error("Invalid JSON-RPC ID");
      }
    }
    if (!response.getError() && !response.getResult()) {
      this._canSendResponse = false;
      sendError(-32000, "We are unable to generate a response", undefined);
      return false;
    }


    var json = response.toJSON();
    if (!json) {
      return false;
    }

    this._canSendResponse = false;
    return this._transport.send(response.toJSON());
  }

  this.sendError = function(code, message, data) {
    var error = new JsonRpcError(code, message, data);
    var response = new JsonRpcResponse(undefined, error);
    response.setId(this._currentId);

    var json = response.toJSON();
    if (!json) {
      return false;
    }

    this._canSendResponse = false;
    return this._transport.send(response.toJSON());
  }
}

exports.JsonRpcServer = JsonRpcServer;
