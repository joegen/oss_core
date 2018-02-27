"use-strict"

var _isolate = require("./_isolate.jso");
const _Isolate = _isolate.Isolate;
const EventEmitter = async.EventEmitter;

var inter_isolate_handler = function(request, userData, handler, defaultHandler) {
  var method = request.method;
  var arguments = request.arguments;
  if (!handler.hasOwnProperty(method)) {
    if (!defaultHandler) {
      var response = new Object();
      response.error = new Object();
      response.error.code = -32601;
      response.error.message = "Method not found";
      return JSON.stringify(response);
    } else {
      result = defaultHandler(request, userData);
    }
  }
  var result;
  try {
    result = handler[method](arguments, userData);
  } catch (e) {
    e.printStackTrace();
    var response = new Object();
    response.error = new Object();
    response.error.code = -32603;
    response.error.message = e;
    return JSON.stringify(response);
  }
  return JSON.stringify(result);
}

exports.interIsolateHandler = function(request, userData) {
  return inter_isolate_handler(request, userData, exports._handler, exports._global_handler);
}

exports._handler = {}
exports._global_handler = undefined;
exports.on = function(name, func) {
  if (name !== "*") {
    exports._handler[name] = func;
  } else {
    exports._global_handler = func;
  }
}

exports.remove = function(name) {
  exports._handler[name] = null;
  delete exports._handler[name];
}

var IsolateEventEmitter = function() {
  EventEmitter.call(this);
  this.onAnyEvent(function() {
    var request = new Object();
    request.method = arguments[0];
    request.argument = Array.prototype.slice.call(arguments, 1);
    inter_isolate_handler(request, exports._handler);
  });
}
IsolateEventEmitter.prototype = Object.create(EventEmitter.prototype);
exports._eventEmitter = new IsolateEventEmitter();

if (_isolate.isRootIsolate()) {
  _isolate.setRootInterIsolateHandler(exports.interIsolateHandler, exports._eventEmitter._fd);
} else {
  _isolate.setChildInterIsolateHandler(exports.interIsolateHandler, exports._eventEmitter._fd);
}

var Isolate = function(threadId) {
  var isolate = new _Isolate(threadId);
  var _this = this;
  var hadRegisteredHandler = false;
  _this.runSource = function(src) {
    return isolate.runSource(src);
  }

  _this.join = function() {
    return isolate.join();
  }

  _this.execute = function(method, args, timeout) {
    var request = new Object();
    request.method = method;
    request.arguments = args;
    if (!timeout) {
      timeout = 0;
    }
    var json = JSON.stringify(request);
    var result = isolate.execute(json, timeout);
    if (result) {
      return JSON.parse(result);
    } else {
      return undefined;
    }
  }

  _this.notify = function(method, args) {
    var request = new Object();
    request.method = method;
    request.arguments = args;
    var json = JSON.stringify(request);
    isolate.notify(json);
  }
}

exports.notifyParentIsolate = function(method, args) {
  var request = new Object();
  request.method = method;
  request.arguments = args;
  var json = JSON.stringify(request);
  _isolate.notifyParentIsolate(json);
}

exports.Isolate = Isolate;
