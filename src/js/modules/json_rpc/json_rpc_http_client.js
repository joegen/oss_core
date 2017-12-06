"use-strict"

const http = require("http");
const HttpClient = http.HttpClient;
const HttpSession = http.HttpSession;
const HttpRequest = http.HttpRequest;
const HttpResponse = http.HttpResponse;
const utils = require("utils");


var JsonRpcHttpClient = function(host, port, secure)
{
  if (secure === undefined)
  {
    secure = false;
  }
  this._client = new HttpClient(secure);
  this._client.setHost(host);
  this._client.setPort(port);
  this._session = new HttpSession(this._client);
  this._readBuffer = [];
  
  var _this = this;
  this._session.on("response", function(response)
  {
    _this._readBuffer = [];
    _this._readContentLength = _this._session.getResponse().getContentLength();
   
    if (typeof _this._readContentLength === "number" && _this._readContentLength == -1)
    {
      _this._readContentLength = 0;
    }
    
    if (this._readContentLength)
    {
      _this._session.read(this._readContentLength);
    }
    else
    {
      _this._session.read(1024);
    }
  });
  
  this._session.on("read", function(data)
  {
    if (data.length > 0)
    {
      _this._readBuffer = _this._readBuffer.concat(data);
      if (!_this._readContentLength)
      {
        _this._session.read(1024);
      }
      else
      {
        _this.onReadCompleted(utils.bufferToString(_this._readBuffer));
      }
    }
    else
    {
      _this.onReadCompleted(utils.bufferToString(_this._readBuffer));
    }
  });
  
  this._session.on("error", function(err)
  {
      _this.onSendError(err);
  });
  
  this.onReadCompleted = function(){ assert(false); } // meant to be overridden
  
  this.onSendError = function(){ assert(false); } // meant to be overridden
  
  this.send = function(json, uri)
  {
    var request = new HttpRequest();
    request.setVersion(http.HTTP_1_1);
    request.setMethod(http.HTTP_POST);
    
    if (uri)
    {
      request.setUri(uri);
    }
    else
    {
      request.setUri("/");
    }
    
    request.setContentLength(json.length);
    request.setContentType("application/json-rpc");
    request.set("Accept", "application/json-rpc");
    if (!_this._session.send(request, json))
    {
      _this.onReadCompleted("");
    }
  }
}

exports.JsonRpcHttpClient = JsonRpcHttpClient;