"use-strict"

var _system = require("./_system.jso");
var pipe = require("pipe");
var fork = require("fork");
var consts = require("constants");
var async = require("async");

__copy_exports(_system, exports);
__copy_exports(pipe, exports);
__copy_exports(fork, exports);
__copy_exports(consts, exports);

exports.exit = function(code) {
  async.__stop_event_loop();
  __cleanup_modules();
  _system.exit(code);
}

exports._exit = function(code) {
  async.__stop_event_loop();
  __cleanup_modules();
  _system._exit(code);
}

exports.daemonize = function() {
  var pid = 0;
  if (_system.getppid() === 1) {
    return;
  }
  pid = fork.fork();
  if (pid < 0) {
    // Fork error
    _system.exit(1);
  } else if (pid > 0) {
    // Exit the parent process
    _system.exit(0);
  }

  // Obtain a new process group
  _system.setsid();

  //for (var descriptor = _system.getdtablesize(); descriptor >= 0; --descriptor)
  //{
  // close all descriptors we have inheritted from parent
  //  _system.close(descriptor);
  //}

  // Close I/O descriptors
  _system.close(consts.STDIN_FILENO);
  _system.close(consts.STDOUT_FILENO);
  _system.close(consts.STDERR_FILENO);
}
