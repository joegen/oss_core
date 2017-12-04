"use-strict"

const buffer = require("buffer");
const async = require("async");
const EventEmitter = async.EventEmitter;
const _client = require("./_http_client.jso");
const _request = require("./_http_request.jso")
const HttpClient = _client.HttpClient;
const HttpResponse = require("./_http_response.jso").HttpResponse;
const HttpRequest = _request.HttpRequest;
const utils = require("utils");


const READ_BUFFER_SIZE = 1024 * 16;
var READ_BUFFER = new Buffer(READ_BUFFER_SIZE);

const WRITE_BUFFER_SIZE = 1024 * 16;
var WRITE_BUFFER = new Buffer(WRITE_BUFFER_SIZE);


var HttpSession = function(client)
{
  EventEmitter.call(this);
  this._http = client;
  this._request = null;
  this._response = null;
  this._http.setEventFd(this._fd);
  
  this.getRequest = function() { return this._request; };
  this.getResponse = function() { return this._response; };
  
  var _this = this;
  this.on("response_ready", function()
  {
    _this.emit("response", _this._response);
  });
  
  this.on("read_ready", function(size)
  {
    if (size > 0)
    {
      var data = _this.readBuffer(size);
      _this.emit("read", data);
    }
    else
    {
      _this.emit("read", []);
    }
  });
  
  this.on("error", function(err)
  {
    _this.emit("error", err);
  });
  
  this.send = function(request, body)
  {
    if (_this._request !== null)
    {
      _this._request.dispose();
      _this._request = null;
    }
    _this._request = request;

    if (_this._http.sendRequest(request))
    {
      if (typeof body !== "undefined")
      {
        _this.write(body);
      }
      if (_this._response !== null)
      {
        _this._response.dispose();
        _this._response = null;
      }
      _this._response = new HttpResponse();
      try
      {
        _this._http.receiveResponse(_this._response);
        return true;
      }
      catch(e)
      {
        _this.emit("error", e);
        _this._response.dispose();
        _this._response = null;
        return false;
      }
    }
    return false;
  }

  this.write = function(data)
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
    return _this._http._write(WRITE_BUFFER, size);
  }

  this.read = function(size)
  {
    _this._http._read(READ_BUFFER, size);
  }

  this.readBuffer = function(size)
  {
    return READ_BUFFER.toArray(size);
  }
}
HttpSession.prototype = Object.create(EventEmitter.prototype);



var createSession = function(host, port, uri, result)
{
  var client = new HttpClient();
  client.setHost(host);
  client.setPort(port);
  
  var session = new HttpSession(client);
  var contentLength = undefined;
  session.on("response", function(response)
  {
    contentLength = response.getContentLength();
    if (typeof contentLength === "number" && contentLength == -1)
    {
      contentLength = 0;
    }
    session.read(contentLength ? contentLength : 1024);
  });

  var buff = new Array();
  session.on("read", function(data)
  {
    if (data.length > 0)
    {
      buff = buff.concat(data);
      if (contentLength)
      {
        //
        // We are done.  We have a content length so we are sure we read the correct amount
        //
        result(utils.bufferToString(buff));
      }
      else
      {
        //
        // read some more since we dont know exactly how much data there is
        //
        session.read(1024); 
      }
    }
    else
    {
      //
      // No more data left to read.  We are done.
      //
      result(utils.bufferToString(buff));
    }
  });

  session.on("error", function(message)
  {
    result(undefined, message);
  });
  
  return session;
}

var get = function(host, port, uri, result)
{
  var session = createSession(host, port, uri, result);
  var request = new HttpRequest();
  request.setVersion(_request.HTTP_1_1);
  request.setMethod(_request.HTTP_GET);
  request.setUri(uri);
  return session.send(request);
}

var post = function(host, port, uri, contentType, content, result)
{
  var session = createSession(host, port, uri, result);
  var request = new HttpRequest();
  request.setVersion(_request.HTTP_1_1);
  request.setMethod(_request.HTTP_POST);
  request.setUri(uri);
  request.setContentType(contentType);
  request.setContentLength(content.length);
  return session.send(request, content);
}

__copy_exports(_client, exports);
exports.HttpSession = HttpSession;
exports.get = get;
exports.post = post;
exports.createSession = createSession;




