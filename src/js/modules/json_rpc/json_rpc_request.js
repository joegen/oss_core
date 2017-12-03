"use-strict";

var JsonRpcRequest = function(method, params)
{
  this._method = method;
  this._params = params;
  this._version = "2.0";
  this._id = undefined;
  
  this.getMethod = function() { return this._method; };
  this.setMethod = function(method) { this._method = method };
  
  this.setVersion = function(version) { this._version = version; };
  this.getVersion = function() { return this._version; };
  
  this.setParams = function(params) { this._params = params; };
  this.getParams = function() { return this._params; };
  
  this.setId = function(id) { this._id = id; };
  this.getId = function() { return this._id; };
  
  this.fromJSON = function(json)
  {
    var request = undefined;
    try 
    {
      request = JSON.parse(json);
    }
    catch(e)
    {
      return false;
    }
    
    if (!request.method || !request.jsonrpc)
    {
      return false;
    }
    this._version = request.jsonrpc;
    this._method = request.method;
    this._id = request.id;
    this._params = request.params;
    return true;
  }
  
  this.toJSON = function()
  {
    if (!this._version || !this._method)
    {
      return undefined;
    }
    var request = new Object();
    request.jsonrpc = this._version;
    request.method = this._method;
    if (this._id)
    {
      request.id = this._id;
    }
    if (this._params)
    {
      request.params = this._params;
    }
    return JSON.stringify(request);
  }
}

exports.JsonRpcRequest = JsonRpcRequest;
