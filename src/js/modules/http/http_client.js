"use-strict"

const buffer = require("buffer");
const async = require("async");
const EventEmitter = async.EventEmitter;
const _client = require("./_http_client.jso");
const HttpResponse = require("./_http_response.jso").HttpResponse;
const HttpRequest = require("./_http_request.jso").HttpRequest;

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
    _this._on_event("response", _this._response);
  });
  
  this.on("read_ready", function(size)
  {
    if (size > 0)
    {
      var data = _this.readBuffer(size);
      _this._on_event("read", data);
    }
    else
    {
      _this._on_event("read", []);
    }
  });
}
HttpSession.prototype = Object.create(EventEmitter.prototype);

HttpSession.prototype.send = function(request, body)
{
  if (this._request !== null)
  {
    this._request.dispose();
    this._request = null;
  }
  this._request = request;
  
  if (this._http.sendRequest(request))
  {
    if (typeof body !== "undefined")
    {
      this.write(body);
    }
    if (this._response !== null)
    {
      this._response.dispose();
      this._response = null;
    }
    this._response = new HttpResponse();
    try
    {
      this._http.receiveResponse(this._response);
      return true;
    }
    catch(e)
    {
      this.emit("error", e);
      this._response.dispose();
      this._response = null;
      return false;
    }
  }
  return false;
}

HttpSession.prototype.write = function(data)
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
  return this._http._write(WRITE_BUFFER, size);
}

HttpSession.prototype.read = function(size)
{
  this._http._read(READ_BUFFER, size);
}

HttpSession.prototype.readBuffer = function(size)
{
  return READ_BUFFER.toArray(size);
}

__copy_exports(_client, exports);
exports.HttpSession = HttpSession;




