"use-strict"

const _proc = require("./_process.jso");
const async = require("async");
const SIGKILL = require("constants").SIGKILL;

var Process = function(name, startupCmd, shutdownCmd, pidFile) {
  this.name = name;
  this.startupCmd = startupCmd;
  this.shutdownCmd = shutdownCmd;
  this.pidFile = pidFile;
  this._timerId = 0;
  this._timerIinterval = 1000;
  this._hasStarted = false;
  this._process = null;
}

Process.prototype.start = function() {
  if (this._process !== null) {
    this._hasStarted = false;
    this._process = new _proc.Process(this.name, this.startupCmd, this.shutdownCmd, this.pidFile);
    this._timerId = async.setTimeout(this.onTimer, this._timerIinterval, [this]);
  }
}

Process.prototype.stop = function(signal) {
  if (typeof signal === "undefined") {
    signal = SIGKILL;
  }

  if (this._process !== null) {
    async.clearTimeout(this._timerId);
    this._process.stop(signal);
    this._process = null;
    this._hasStarted = false;
  }
}

Process.prototype.restart = function() {
  stop();
  start();
}

Process.prototype.isAlive = function() {
  if (this._process !== null) {
    return this._process.isAlive() && this.getPid() != -1;
  }
  return false;
}

Process.prototype.getPid = function() {
  if (this._process !== null) {
    return this._process.getPid();
  }
  return -1;
}

Process.prototype.onTimer = function(process) {
  var resetTimer = true;
  if (process._hasStarted && !process.isAlive()) {
    process.onStopped(process);
    process._hasStarted = false;
    resetTimer = false;
  } else if (!process._hasStarted) {
    process._hasStarted = true;
    process.onStarted(process);
  }

  if (resetTimer) {
    process._timerId = async.setTimeout(process.onTimer, process._timerIinterval, [process]);
  }
}

Process.prototype.onStarted = function(process) {}

Process.prototype.onStopped = function(process) {}

Process.prototype.on = function(name, method) {
  if (name === "started" && typeof method === "function") {
    this.onStarted = method;
  } else if (name === "stopped" && typeof method === "function") {
    this.onStopped = method;
  }
}