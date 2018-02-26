"use-strict";

const _file = require("_file");
const buffer = require("buffer");

var File = function(fn, mode) {
  this._file = new _file.File();
  this._memBuffer = null;
  if (typeof fn === "string") {
    this.open(fn, mode);
  }
}

File.prototype.open = function(fn, mode) {
  if (typeof mode !== "string") {
    mode = "a+";
  }
  return this._file.fopen(fn, mode);
}

File.prototype.close = function(fn) {
  this._file.fflush();
  return this._file.fclose(fn);
}

File.prototype.write = function(data) {
  return this._file.fwrite(data);
}

File.prototype.writeLine = function(data) {
  var written = this._file.fwrite(data);
  if (written > 0) {
    written += this._file.fputc(10);
  }
  return written;
}

File.prototype.read = function(len) {
  return this._file.fread(len);
}

File.prototype.readLine = function() {
  var buf = this._file.fgets(_file.LINE_MAX_LEN);
  if (buffer.isBuffer(buf)) {
    return buf.toString();
  }
}

File.prototype.getPos = function() {
  return this._file.ftell();
}

File.prototype.setPos = function(pos, whence) {
  if (typeof whence !== "number") {
    whence = _file.SEEK_SET;
  }
  return this._file.fseek(pos, whence);
}

File.prototype.eof = function() {
  return this._file.feof();
}

File.prototype.flush = function() {
  return this._file.fflush();
}

File.prototype.lock = function() {
  return this._file.flock(_file.LOCK_EX | _file.LOCK_NB) == 0;
}

File.prototype.unlock = function() {
  return this._file.flock(_file.LOCK_UN | _file.LOCK_NB) == 0;
}

File.prototype.getFd = function() {
  return this._file.fileno();
}

exports.File = File;