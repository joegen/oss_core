

function AccessList()
{
  this._trustedNetwork = [];
  this._trustEveryone = false;
}

AccessList.prototype.verify = function(request)
{
  var sipMessage = new SIPMessage(request);

  if (this._trustEveryone)
  {
    sipMessage.setProperty("auth-action", "accept");
    return true;
  }

  var  sourceAddress = sipMessage.getSourceAddress();
  var i;
  for(i=0; i < this._trustedNetwork.length; i++)
  {
    var network = this._trustedNetwork[i];
    log_info(sipMessage.getLogId(), "Verifying " + sourceAddress + " against " + network )
    if (cidrVerify(sourceAddress, network))
    {
      sipMessage.setProperty("auth-action", "accept");
      return true;
    }
  }
  sipMessage.setProperty("auth-action", "reject");
  return false;
}
