"use-strict"

var _system = require("_system");
var pipe = require("pipe");
var fork = require("fork");

exports.read = _system.read;
exports.write = _system.write;
exports.close = _system.close;
exports.exit = _system.exit;
exports._exit = _system._exit;
exports.sleep = _system.sleep;

exports.pipe = pipe.pipe;
exports.fork = fork.fork;
exports.wait = fork.wait;

