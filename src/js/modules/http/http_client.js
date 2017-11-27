"use-strict"

const buffer = require("buffer");
const _client = require("./_http_client.jso");
const HttpResponse = require("./_http_response.jso").HttpResponse;
const HttpRequest = require("./_http_request.jso").HttpRequest;

const async = require("async");

const READ_BUFFER_SIZE = 1024 * 16;
var READ_BUFFER = new Buffer(READ_BUFFER_SIZE);

const WRITE_BUFFER_SIZE = 1024 * 16;
var WRITE_BUFFER = new Buffer(WRITE_BUFFER_SIZE);

_client.HttpClient.prototype._on_response = function(request, response) {}
_client.HttpClient.prototype._on_error = function(request, e) {}
_client.HttpClient.prototype._has_registered_fd = false;

_client.HttpClient.prototype.send = function(request)
{
  this._request = request;
  var ret = this.sendRequest(request);
  if (ret && !this._has_registered_fd)
  {
    this._has_registered_fd = true;
    this._fd = this.getSocketFd();
    var _this = this;
    async.monitorFd(this._fd, function(fd, revents) 
    {
      var response = new HttpResponse();
      try
      {
        _this.receiveResponse(response);
        _this._on_response(_this._request, response);
      }
      catch(e)
      {
        _this._on_error(_this._request, e);
        response.dispose();
      }
    });
  }
  return ret;
}

_client.HttpClient.prototype.write = function(data)
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
  return this._write(WRITE_BUFFER, size);
}

_client.HttpClient.prototype.read = function(size)
{
  var ret = this._read(size);
  if (ret > 0)
  {
    READ_BUFFER.toArray(ret);
  }
  return [];
}

_client.HttpClient.prototype.on = function(name, func)
{
  if (name === "response") this._on_response = func;
  if (name === "error") this._on_error = func;
}

__copy_exports(_client, exports);




