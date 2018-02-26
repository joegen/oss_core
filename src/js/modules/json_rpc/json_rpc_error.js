"use-strict";

var JsonRpcError = function(code, message, data) {
  this._code = code;
  this._message = message;
  this._data = data;

  this.setCode = function(code) {
    this._code = code;
  };
  this.getCode = function() {
    return this._code;
  };

  this.setMessage = function(message) {
    this._message = message;
  };
  this.getMessage = function() {
    return this._message;
  };

  this.setData = function(data) {
    this._data = data;
  };
  this.getData = function() {
    return this._data;
  };
}

exports.JsonRpcError = JsonRpcError;