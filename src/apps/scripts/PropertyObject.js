

function PropertyObject(pname, value, requestType)
{
  this._name = pname;
  this._value = value;
  this._requestType = requestType;
}

PropertyObject.prototype.set = function(pname, value, requestType)
{
  this._name = pname;
  this._value = value;
  this._requestType = requestType;
}


