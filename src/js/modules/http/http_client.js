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


var HttpSession = function(client) {
  EventEmitter.call(this);
  this._http = client;
  this._request = null;
  this._response = null;
  this._http.setEventFd(this._fd);

  this.getRequest = function() {
    return this._request;
  };
  this.getResponse = function() {
    return this._response;
  };

  var _this = this;

  this.on("request_sent", function() {
    if (_this._request.onRequestSent) {
      _this._request.onRequestSent(_this._request);
    }
  });

  this.on("response_ready", function() {
    if (_this._response.onReceivedResponse) {
      _this._response.onReceivedResponse(_this._response);
    }
  });

  this.on("read_ready", function(size) {
    if (size > 0) {
      var data = _this.readBuffer(size);
      _this.emit("read", data);
    } else {
      _this.emit("read", []);
    }
  });

  this.on("error", function(err) {
    _this.emit("error", err);
  });

  this.send = function(request, requestCb) {
    if (_this._request !== null) {
      _this._request.dispose();
      _this._request = null;
    }

    _this._request = request;
    _this._request.onRequestSent = requestCb;
    _this._http.sendRequest(_this._request);
  }

  this.receiveResponse = function(responseCb) {
    _this._response = new HttpResponse();
    _this._response.onReceivedResponse = responseCb;
    try {
      _this._http.receiveResponse(_this._response);
    } catch (e) {
      _this.emit("error", e);
      _this._response.dispose();
      _this._response = null;
    }
  }

  this.write = function(data) {
    var size = 0;
    if (buffer.isBuffer(data)) {
      WRITE_BUFFER.fromBuffer(data, false);
      size = data.size();
    } else if (typeof data === "string") {
      WRITE_BUFFER.fromString(data, false);
      size = data.length;
    } else if (typeof data === "array" || typeof data === "object") {
      WRITE_BUFFER.fromArray(data, false);
      size = data.length;
    } else {
      throw new Error("Invalid data argument.  Must be buffer, array or string");
    }
    return _this._http._write(WRITE_BUFFER, size);
  }

  this.read = function(size) {
    _this._http._read(READ_BUFFER, size);
  }

  this.readBuffer = function(size) {
    return READ_BUFFER.toArray(size);
  }

  this.dispose = function() {
    _this._http.dispose();
  }
}
HttpSession.prototype = Object.create(EventEmitter.prototype);



var createSession = function(host, port, uri, resultHandler, autoDispose) {
  var client = new HttpClient(false);
  client.setHost(host);
  client.setPort(port);

  var session = new HttpSession(client);

  var buff = new Array();
  session.on("read", function(data) {
    if (data.length > 0) {
      buff = buff.concat(data);
      if (contentLength) {
        //
        // We are done.  We have a content length so we are sure we read the correct amount
        //
        resultHandler(utils.bufferToString(buff));
        if (autoDispose != undefined && autoDispose) {
          session.dispose();
        }
      } else {
        //
        // read some more since we dont know exactly how much data there is
        //
        session.read(1024);
      }
    } else {
      //
      // No more data left to read.  We are done.
      //
      resultHandler(utils.bufferToString(buff));
      if (autoDispose != undefined && autoDispose) {
        session.dispose();
      }
    }
  });

  session.on("error", function(message) {
    resultHandler(undefined, message);
    if (autoDispose != undefined && autoDispose) {
      session.dispose();
    }
  });

  return session;
}

var get = function(host, port, uri, resultHandler) {
  var session = createSession(host, port, uri, resultHandler, true);
  var request = new HttpRequest();
  request.setVersion(_request.HTTP_1_1);
  request.setMethod(_request.HTTP_GET);
  request.setUri(uri);
  session.send(request, function(request) {
    session.receiveResponse(function(response) {
      contentLength = response.getContentLength();
      if (typeof contentLength === "number" && contentLength == -1) {
        contentLength = 0;
      }
      session.read(contentLength ? contentLength : 1024);
    });
  });
}

var post = function(host, port, uri, contentType, content, resultHandler) {
  var session = createSession(host, port, uri, resultHandler, true);
  var request = new HttpRequest();
  request.setVersion(_request.HTTP_1_1);
  request.setMethod(_request.HTTP_POST);
  request.setUri(uri);
  request.setContentType(contentType);
  request.setContentLength(content.length);
  return session.send(request, function(request) {
    //
    // Write the body then trigger a receiveResponse
    //
    session.write(content);
    session.receiveResponse(function(response) {
      var contentLength = response.getContentLength();
      if (typeof contentLength === "number" && contentLength == -1) {
        contentLength = 0;
      }
      session.read(contentLength ? contentLength : 1024);
    });
  });
}

__copy_exports(_client, exports);
exports.HttpSession = HttpSession;
exports.get = get;
exports.post = post;
exports.createSession = createSession;