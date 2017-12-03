"use-strict";

const http = require("http");
const JsonRpcServer = require("./json_rpc_server.js").JsonRpcServer;
const utils = require("utils");

var JsonRpcHttpServer = function()
{
  var _this = this;
  
  this.requestHandler = function(request, response)
  {
    _this._request = request;
    _this._response = response;
    
    var contentType = request.getContentType();
    if (!contentType || contentType.toLowerCase() !== "application/json-rpc")
    {
      //
      // Send out an empty response
      //
      _this._response.send();
      return;
    }
    
    var contentLength = request.getContentLength();
    var body = [];
    if (contentLength)
    {
      body = request.read(contentLength);
    }
    else
    {
      while (true)
      {
        var buf = request.read(256);
        if (buf && buf.length > 0)
        {
          body.concat(buf);
        }
        else
        {
          break;
        }
      }
    }
    
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