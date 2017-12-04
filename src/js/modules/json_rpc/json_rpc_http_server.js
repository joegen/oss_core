"use-strict";

const http = require("http");
const JsonRpcServer = require("./json_rpc_server.js").JsonRpcServer;
const utils = require("utils");

var JsonRpcHttpServer = function()
{
  var _this = this;
  
  this.nonRpcHandler = function(request, response)
  {
    //
    // Send out an empty response
    //
    response.send();
    return;
  }
  
  this.requestHandler = function(request, response)
  {
    _this._request = request;
    _this._response = response;
    
    var contentType = request.getContentType();
    if (!contentType)
    {
      _this.nonRpcHandler(request, response);
      return;
    }
    else
    {
      contentType = contentType.toLowerCase();
      if (contentType !== "application/json-rpc" && contentType !== "application/json" && contentType !== "application/jsonrequest")
      {
        _this.nonRpcHandler(request, response);
        return;
      }
    }
    
    var body = request.getBody();
    
    if (body.length > 0)
    {
      _this.onNewRequest(utils.bufferToString(body));
    }
    else
    {
      //
      // Send out an empty response
      //
      _this._response.send();
      return;
    }
  }
  
  // This is meant to be overriden
  var onNewRequest = function() { assert(false); }
  
  this._server = http.createServer(this.requestHandler);
  this.listen = function()
  {
    if (arguments.length == 1)
    {
      _this._server.listen(arguments[0]);
    }
    else if(arguments.length == 2)
    {
      _this._server.listen(arguments[0], arguments[1]);
    }
    else if(arguments.length == 3)
    {
      _this._server.listen(arguments[0], arguments[1], arguments[2]);
    }
  }
  
  this.send = function(body)
  {
    _this._response.setContentType("application/json-rpc");
    _this._response.setContentLength(body.length);
    _this._response.send(body);
  }
}

exports.JsonRpcHttpServer = JsonRpcHttpServer