"use-strict"

var _system = require("_system");
var pipe = require("pipe");
var fork = require("fork");

exports.read = _system.read;
exports.write = _system.write;
exports.close = _system.close;
exports.sleep = _system.sleep;

exports.pipe = pipe.pipe;
exports.fork = fork.fork;
exports.wait = fork.wait;

exports.exit = function(code)
{
  __cleanup_modules();
  _system.exit(code);
}

exports._exit = function(code)
{
  __cleanup_modules();
  _system._exit(code);
}

