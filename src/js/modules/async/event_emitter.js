"use-strict"

_async = require("./_async.jso");

var EventEmitter = function()
{
  this._handlers = new Object();
  var _this = this;
  this._on_event = function()
  {
    var event = arguments[0];
    if (!_this._handlers.hasOwnProperty(event))
    {
      return; // no handler, don't consume this event
    }
    var args = Array.prototype.slice.call(arguments, 1);
    try
    {
      _this._handlers[event].apply(this, args);
    }
    catch(e)
    {
      e.printStackTrace();
    }
  }
  
  this.on = function(event, func)
  {
    _this._handlers[event] = func;
  }
  this._queue = new _async.Queue(this._on_event);
  this._fd = this._queue.getFd();
  
  this.emit = function()
  {
    if (arguments.length < 1)
    {
      throw new Error("Must provide a valid event name");
    }
    var event = arguments[0];
    if (!_this._handlers.hasOwnProperty(event))
    {
      return; // no handler, don't consume this event
    }
    var args = [];
    for( var i = 0; i < arguments.length; i++) 
    {
      args[i] = arguments[i];
    }
    _this._queue.enqueue(args);
  }
}

exports.EventEmitter = EventEmitter;