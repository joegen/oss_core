function TransactionProfile()
{
  this._transactionParams = [];
}

TransactionProfile.prototype.setProperties = function(sipMessage)
{
    var i = 0;
    for (i = 0; i < this._transactionParams.length; i++)
    {
      var property = this._transactionParams[i];
      if (property._requestType == "*")
      {
        sipMessage.setTransactionProperty(property._name, property._value);
      }
      else if (sipMessage.isRequest(property._requestType))
      {
        sipMessage.setTransactionProperty(property._name, property._value);
      }
    }
    return true;
}
