"use-strict";
const _zmq = require("_zmq");
const async = require("async");
const poll = require("poll");
const log = require("logger");
const system = require("system");
const buffer = require("buffer");
const assert = require("assert");

__copy_exports(_zmq, exports);

exports.ZMQSocket.prototype.start = function(callback)
{
  async.monitorFd(this.getFd(), callback);
}

exports.ZMQSocket.prototype.close = function()
{
  async.unmonitorFd(this.getFd());
  this._close();
}
