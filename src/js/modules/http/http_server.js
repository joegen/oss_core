"use-strict"

const console = require("console");
const async = require("async");
const isolate = require("isolate");
const system = require("system");
const HttpServerBase = require("./_http_server.jso").HttpServer;  
const buffer = require("buffer");

const READ_BUFFER_SIZE = 1024 * 16;
var READ_BUFFER = new Buffer(READ_BUFFER_SIZE);

const WRITE_BUFFER_SIZE = 1024 * 16;
var WRITE_BUFFER = new Buffer(WRITE_BUFFER_SIZE);

var RequestHandler = function(rpc)
{  
  if (rpc )
  {
    var params = rpc;
    var request = params.request;
    var rpcId = params.rpcId;
    var inputStreamId = params.inputStreamId;
    var outputStreamId = params.outputStreamId;
    request.serverAddress = params.serverAddress;
    request.clientAddress = params.clientAddress;
    
    if (rpcId && request && inputStreamId && outputStreamId)
    {
      var server = RequestHandler.find(rpcId);
      if (server)
      {
        return server.handleRequest(request, inputStreamId, outputStreamId);
      }
      else
      {
        return RequestHandler.error(-32603, "Server Gone");
      }
    }
    else
    {
      return RequestHandler.error(-32602, "Invalid Request");
    }
  }
  else
  {
    
  }
}

RequestHandler.servers = new Object();
RequestHandler.idCounter = 0;

RequestHandler.error = function(code, message)
{
  var err = new Object();
  err.code = code;
  err.message = message;
  return err;
}

RequestHandler.register = function(server)
{
  var serverId = "srv-" + RequestHandler.idCounter++;
  server._rpcId = serverId;
  server._base._setRpcId(serverId);
  RequestHandler.servers[serverId] = server;
}

RequestHandler.find = function(serverId)
{
  if (RequestHandler.servers.hasOwnProperty(serverId))
  {
    return RequestHandler.servers[serverId];
  }
  return undefined;
}

isolate.on("HttpServerObject__handleRequest", RequestHandler);

var IncomingMessage = function(headers, inputStreamId, server)
{
  this.uri = headers.uri;
  this._headers = headers;
  this._inputStreamId = inputStreamId;
  this._server = server;
  this.serverAddress = headers.serverAddress;
  this.clientAddress = headers.clientAddress;
  
  var _this = this;
  
  this.read = function(size)
  {
    return _this._server.read(this._inputStreamId, size);
  }
  
  this.getHeader = function(name)
  {
    if (typeof name !== "string")
    {
      return undefined;
    }
    var headerName = name.toLowerCase();
    for (var key in this._headers) 
    {
      if (key.toLowerCase() === headerName)
      {
        return _this._headers[key];
      }
    }
  }
  
  
  this.getContentLength = function()
  {
    return parseInt(_this.getHeader("Content-Length"));
  }
  
  this.getContentType = function()
  {
    return _this.getHeader("Content-Type");
  }
  
  this.getBody = function()
  {
    var contentLength = _this.getContentLength();
    var body = [];
    if (contentLength)
    {
      body = _this.read(contentLength);
    }
    else
    {
      while (true)
      {
        var buf = _this.read(256);
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
    return body;
  }
}

var HttpServerResponse = function(outputStreamId, server)
{
  this._outputStreamId = outputStreamId;
  this._server = server;
  this._status = 200;
  this._reason = "Ok";
  this._contentType = null;
  this._contentLength = null;
  this._transferEncoding = null;
  this._chunkedTransferEncoding = null;
  this._keepAlive = null;
  this._hasSentHeaders = false;
  
  this.setStatus = function(value) { this._status = value; }
  this.getStatus = function() { return this._status; }
  
  this.setReason = function(value) { this._reason = value; }
  this.getReason = function() { return this._reason; }
  
  this.setContentType = function(value) { this._contentType = value; }
  this.getContentType = function() { return this._contentType; }
  
  this.setContentLength = function(value) { this._contentLength = value; }
  this.getContentLength = function() { return this._contentLength; }
  
  this.setTransferEncoding = function(value) { this._transferEncoding = value; }
  this.getTransferEncoding = function() { return this._transferEncoding; }
  
  this.setChunkedTransferEncoding = function(value) { this._chunkedTransferEncoding = value; }
  this.getChunkedTransferEncoding = function() { return this._chunkedTransferEncoding; }
  
  this.setKeepAlive = function(value) { this._keepAlive = value; }
  this.getKeepAlive = function() { return this._keepAlive; }
  
  this.send = function(data)
  {
    if (data != undefined)
    {
      return this.write(data);
    }
    
    if (this._hasSentHeaders)
    {
      return;
    }
    
    this._hasSentHeaders = true;
    var response = new Object();
    response.status = this._status;
    response.reason = this._reason;
    response.contentType = this._contentType;
    response.contentLength = this._contentLength;
    response.transferEncoding = this._transferEncoding;
    response.chunkedTransferEncoding = this._chunkedTransferEncoding;
    response.keepAlive = this._keepAlive;

    return this._server.sendResponse(response, this._outputStreamId);
  }
  
  this.write = function(data)
  {
    this.send(); // Send the headers if need
    return this._server.write(this._outputStreamId, data);
  }
  
  this.reject = function(status, reason)
  {
    this.setStatus(status);
    this.setReason(reason);
    this.send();
  }
}

var HttpServer = function(requestHandler)
{
  this._handler = requestHandler;
  this._base = new HttpServerBase();
  
  this.handleRequest = function(request, inputStreamId, outputStreamId)
  {
    var request = new IncomingMessage(request, inputStreamId, this);
    var response = new HttpServerResponse(outputStreamId, this);
    try
    {
      this._handler(request, response);
      var result = new Object();
    }
    catch(e)
    {
      console.printStackTrace(e);
    }
    result.result = "Ok";
    return result;
  }
  
  this.listen = function()
  {
    if (arguments.length == 1)
    {
      this._base._listen("*", arguments[0], function(){});
    }
    else if (arguments.length == 2)
    {
      this._base._listen("*", arguments[0], arguments[1]);
    }
    else if (arguments.length == 3)
    {
      this._base._listen(arguments[0], arguments[1], arguments[2]);
    }
    else
    {
      return false;
    }
    return true;
  }
  
  this.read = function(inputStreamId, size)
  {
    var len = this._base._read(inputStreamId, READ_BUFFER, size);
    if (!len)
    {
      return [];
    }
    return READ_BUFFER.toArray(len);
  }
  
  this.write = function(outputStreamId, data)
  {
    var size = 0;
    if (buffer.isBuffer(data))
    {
      WRITE_BUFFER.fromBuffer(data, false);
      size = data.size();
    }
    else if (typeof data === "string")
    {
      WRITE_BUFFER.fromString(data, false);
      size = data.length;
    }
    else if (typeof data === "array" || typeof data === "object")
    {
      WRITE_BUFFER.fromArray(data, false);
      size = data.length;
    }
    else
    {
      throw new Error("Invalid data argument.  Must be buffer, array or string");
    }
    return this._base._write(outputStreamId, WRITE_BUFFER, size);
  }
  
  this.sendResponse = function(response, outputStreamId)
  {
    return this._base._sendResponse(outputStreamId, response);
  }
}

var createServer = function(requestHandler)
{
  var server = new HttpServer(requestHandler);
  RequestHandler.register(server);
  return server;
}

exports.createServer = createServer;