
"use-strict";

const JsonRpcError = require("./json_rpc_error.js").JsonRpcError;

var JsonRpcResponse = function(result, error)
{
  this._version = "2.0";
  this._id = undefined; // automatically set by the server
  this._result = result;
  this._error = error;
  var _this = this;
  
  this.setVersion = function(version) { _this._version = version; };
  this.getVersion = function() { return _this._version; };
  
  this.setResult = function(result) { _this._result = result; };
  this.getResult = function() { return _this._result; };
  
  this.setId = function(id) { _this._id = id; };
  this.getId = function() { return _this._id; };
  
  this.setError = function(error) { _this._error = error; };
  this.getError = function() { return _this._error; };
  

  this.fromJSON = function(json)
  {
    var response;
    try
    {
      response = JSON.parse(json);
    }
    catch(e)
    {
      _this._result = undefined;
      _this._error = new JsonRpcError(-1, "Local parse error occured");
      return false;
    }
          
    if (!response.id || !response.jsonrpc)
    {
      _this._result = undefined;
      _this._error = new JsonRpcError(-1, "Missing ID or Version element");
      return false;
    }
    
    if (!response.result && !response.error)
    {
      _this._result = undefined;
      _this._error = new JsonRpcError(-1, "The other side is a teapot.  Giving up");
      return false;
    }
    
    _this._version = response.jsonrpc;
    _this._id = response.id;
    if (response.result)
    {
      _this._result = response.result;
    }
    
    if (response.error)
    {
      _this._error = response.error;
    }
    return true;
  }
  
  this.toJSON = function()
  {
    if (!_this._version || !_this._id)
    {
      return undefined;
    }
    
    if (!_this._result && !_this._error)
    {
      return undefined;
    }
    
    var response = new Object();
    response.jsonrpc = _this._version;
    response.id = _this._id;

    if (_this._result)
    {
      response.result = _this._result;
    }
    
    if (_this._error)
    {
      var error = new Object();
      if (_this._error instanceof JsonRpcError)
      {
        error.code = _this._error.getCode();
        error.message = _this._error.getMessage();
        
        if (_this._error.getData() != undefined)
        {
          error.data = _this._error.getData();
        }
      }
      else
      {
        error.code = _this._error.code;
        error.message = _this._error.message;
        error.data = _this._error.data;
      }
      response.error = error;
    }
    return JSON.stringify(response);
  }
}

exports.JsonRpcResponse = JsonRpcResponse;
