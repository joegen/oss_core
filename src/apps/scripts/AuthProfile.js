function AuthProfile()
{
  this._accessList = new AccessList();
  this._realm = "karoo";
}

AuthProfile.prototype = new TransactionProfile();

AuthProfile.prototype.verify = function(request)
{
  var sipMessage = new SIPMessage(request);
  if (this._accessList.verify(request))
  {
    return this.setProperties(sipMessage);
  }
  return false;
}

