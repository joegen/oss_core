function SIPMessage(request)
  //
  // The SIPMessage class is a utility wrapper for the javascript API exposed by
  // Flexi-Route.
  //
  // Arguments:
  //  request: (Object) The request handle obtained from handle_request() procedure
  //
  // Return Type: Object
  //
  // Usage:
  //  function handle_request(request)
  //  {
  //    var msg = new SIPMessage(request);
  //    // Perform message operation here
  //  }
  //
{
  this._request = request;
}


SIPMessage.prototype.getLogId = function()
  //
  // This function will return the logger context-id to allow Flexi-Route to
  // set an identical context-id with the underlying dialog.  The context-id
  // is normally used as a filter to retrieve logs for a particular transaction
  // or dialog through command line tools such as the unix grep command.
  //
  // Parameters: None
  //
  // Return Type: string
  //
  // Usage:
  //  var logId = this._sipMessage.getLogId();
  //  log_info(logId, "this is a sample javascript log entry");
  //
{
  return msgGetTransactionProperty(this._request, "log-id");
}

SIPMessage.prototype.setProperty = function(propertyName, propertyValue)
  //
  // This function is used to set properties of a SIPMessage that may affect
  // transaction behavior in dealing with the SIPMessage.
  //
  // Parameters:
  //  propertyName: (String) The name of the property to set
  //  propertyValue: (String) The value of the property
  //
  // Return Type: void
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.setProperty("some-property", "some-value");
  //
{
  if (typeof propertyValue == "undefined")
    propertyValue = "?";
  log_debug(this.getLogId(), "Setting property " + propertyName + "=" + propertyValue);
  msgSetProperty(this._request, propertyName, propertyValue);
}

SIPMessage.prototype.getProperty = function(propertyName)
  //
  // This function returns the current value of a SIPMessage property.
  //
  // Parameters:
  //  propertyName: (String) The name of the property to get
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var someProp = msg.getProperty("some-property");
  //
{
  return msgGetProperty(this._request, propertyName);
}

SIPMessage.prototype.setTransactionProperty = function(propertyName, propertyValue)
  //
  // This function is used to set properties of a transaction that may affect
  // over-all transaction behavior.
  //
  // Parameters:
  //  propertyName: (String) The name of the property to set
  //  propertyValue: (String) The value of the property
  //
  // Return Type: void
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.setTransactionProperty("some-property", "some-value");
  //
{
  log_debug(this.getLogId(), "Setting transaction property " + propertyName + "=" + propertyValue);
  msgSetTransactionProperty(this._request, propertyName, propertyValue);
}

SIPMessage.prototype.getTransactionProperty = function(propertyName)
  //
  // This function returns the current value of a Transaction property.
  //
  // Parameters:
  //  propertyName: (String) The name of the property to get
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var someProp = msg.getTransactionProperty("some-property");
  //
{
  return msgGetTransactionProperty(this._request, propertyName);
}

SIPMessage.prototype.getSourceAddress = function()
  //
  // This function returns the source address of the packet.  Take note
  // that the via hostport and the source address are not always similar.
  // This function deals with the actual source address and not the via
  // hostport.  The reuturn value would only include the address and not the
  // port (See getSourcePort()).
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var src = msg.getSourceAddress();
  //
{
  return msgGetSourceAddress(this._request);
}

SIPMessage.prototype.getSourcePort = function()
  //
  // This function returns the source port of the packet.  Take note
  // that the via port and the source address are not always similar.
  // This function deals with the actual source port and not the via
  // port.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var addr = msg.getSourceAddress();
  //  var port = msg.getSourcePort();
  //
{
  return msgGetSourcePort(this._request);
}

SIPMessage.prototype.getSourceAddressAndPort = function()
{
	return this.getSourceAddress() + ":" + this.getSourcePort();
}

SIPMessage.prototype.getInterfaceAddress = function()
  //
  // This function returns the interface address that received the request.
  // Take note that this only returns the address and not the port used by the
  // interface (See getInterfacePort).
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var addr = msg.getInterfaceAddress();
  //  var port = msg.getInterfacePort();
  //
{
  return msgGetInterfaceAddress(this._request);
}

SIPMessage.prototype.getInterfacePort = function()
  //
  // This function returns the interface port that received the request.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var addr = msg.getInterfaceAddress();
  //  var port = msg.getInterfacePort();
  //
{
  return msgGetInterfacePort(this._request);
}

SIPMessage.prototype.getRequestUri = function()
  //
  // This function returns the request-uri of a SIP request message.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var uri = msg.getRequestUri();
  //
{
  return msgGetRequestUri(this._request);
}

SIPMessage.prototype.setRequestUri = function(requestUri)
  //
  // This function changes the request-uri of a SIP request message.  This
  // function returns true if successful.
  //
  // Parameters:
  //  requestUri: (String) The new SIP URI.  Must be in valid URI format.
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var uri = "sip:alice@atalanta.com";
  //  msg.setRequestUri(uri);
  //
{
  msgSetRequestUri(this._request, requestUri);
}

SIPMessage.prototype.getRequestUriUser = function()
  //
  // This function returns the user part of the request-uri.  For telephone
  // calls, this is normally the function used to retrieve the dialed number.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var user = msg.getRequestUriUser();
  //
{
  return msgGetRequestUriUser(this._request);
}

SIPMessage.prototype.setRequestUriUser = function(user)
  //
  // This function changes the user portion of the request-uri.  This function
  // returns true if successful.
  //
  // Parameters:
  //  user: (String) The new user value
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var user = "alice";
  //  msg.setRequestUriUser(user);
  //
{
  msgSetRequestUriUser(this._request, user);
}

SIPMessage.prototype.getRequestUriHostPort = function()
  //
  // This function returns the hostport portion of the request uri.
  // Hostport refers to host:port tuple where port is optional.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var hostPort = msg.getRequestUriHostPort();
  //
{
  return msgGetRequestUriHostPort(this._request);
}

SIPMessage.prototype.setRequestUriHostPort = function(hostPort)
  //
  // This function changes the hostport portion of the request uri.
  // Hostport refers to host:port tuple where port is optional.  This function
  // returns true if successful.
  //
  // Parameters:
  //  hostPort: (String) The new value for the request-uri hostport
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var hostPort = "192.168.0.10:5060";
  //  msg.setRequestUriHostPort(hostPort);
  //
{
  msgSetRequestUriHostPort(this._request, hostPort);
}

SIPMessage.prototype.getToUser = function()
  //
  // This function returns the user portion of the to-uri.  Take note that
  // the user portion of the to-uri is not similar to the current intended
  // target since proxies may change the target without touching the to-uri.
  // Please see getRequestUriUser() if you intend to retrieve the current
  // transaction target.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var user = msg.getToUser();
  //
{
 return msgGetToUser(this._request);
}

SIPMessage.prototype.setToUser = function(user)
  //
  // This function changes the user portion of the to-uri.  This function will
  // return true if successful.
  //
  // Parameters:
  //  user: (String) The new user
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var newUser = "alice";
  //  msg.setToUser(newUser);
  //
{
 msgSetToUser(this._request, user);
}

SIPMessage.prototype.getToHostPort = function()
  //
  // This function returns the hostport portion of the to-uri.
  // Hostport refers to host:port tuple where port is optional.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var hostPort = msg.getToHostPort();
  //
{
 return msgGetToHostPort(this._request);
}

SIPMessage.prototype.getToHost = function()
  //
  // This function returns the host portion of the to-uri.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var host = msg.getToHost();
  //
{
 return msgGetToHost(this._request);
}


SIPMessage.prototype.setToHostPort = function(hostPort)
  //
  // This function changes the hostport portion of the to-uri.
  // Hostport refers to host:port tuple where port is optional.
  // This function will return true if successful.
  //
  // Parameters:
  //  user: (String) The new user
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.setToHostPort("atlanta.com");
  //
{
  msgSetToHostPort(this._request, hostPort);
}

SIPMessage.prototype.getFromUser = function()
  //
  // This function returns the user portion of the from-uri.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var user = msg.getFromUser();
  //
{
  return msgGetFromUser(this._request);
}

SIPMessage.prototype.setFromUser = function(user)
  //
  // This function changes the user portion of the from-uri.  This function
  // returns true if successful.
  //
  // Parameters:
  //  user: (String) Replacement value
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var user = "alice";
  //  msg.setFromUser(user);
  //
{
  msgSetFromUser(this._request, user);
}

SIPMessage.prototype.setFromUriUser = function(user)
{
  msgSetFromUser(this._request, user);
}

SIPMessage.prototype.getFromHostPort = function()
  //
  // This function returns the hostport portion of the from-uri.
  // Hostport refers to host:port tuple where port is optional.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var hostPort = msg.getFromHostPort();
  //
{
  return msgGetFromHostPort(this._request);
}

SIPMessage.prototype.getFromHost = function()
  //
  // This function returns the host portion of the from-uri.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var host = msg.getFromHost();
  //
{
  return msgGetFromHost(this._request);
}

SIPMessage.prototype.setFromHostPort = function(hostPort)
  //
  // This function changes the hostport portion of the from-uri.
  // Hostport refers to host:port tuple where port is optional.
  // This function will return true if successful.
  //
  // Parameters:
  //  user: (String) The new user
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.setFromHostPort("proxy.atlanta.com:5060");
  //
{
 msgSetFromHostPort(this._request, hostPort);
}

SIPMessage.prototype.hdrPresent = function(headerName)
  //
  // This function will return true if a certain SIP header exists in the request.
  //
  // Parameter:
  //  headerName: (String) The name of the header to check.
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var hasExpires = msg.hdrPresent("expires");
  //
{
  return msgHdrPresent(this._request, headerName);
}

SIPMessage.prototype.hdrGetSize = function(headerName)
  //
  // This function will return the size of list headers such as via, route and
  // record-route headers
  //
  // Parameter:
  //  headerName: (String) The name of the header to check.
  //
  // Return Type: Integer
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var viaCount = msg.hdrGetSize("via");
  //
{
  return msgHdrGetSize(this._request, headerName);
}

SIPMessage.prototype.hdrGet = function(headerName)
  //
  // This function will return the raw string value of SIP headers present in a
  // SIPMessage
  //
  // Parameter:
  //  headerName: (String) The name of the header to retrieve.
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var from = msg.hdrGet("from");
  //
{
  return msgHdrGet(this._request, headerName);
}

SIPMessage.prototype.hdrSet = function(headerName, hdrValue)
  //
  // This function will change the value of a particular SIP header or add
  // a new header if it exists.  If successful, this function will return true.
  //
  // Parameters:
  //  hdrName: (String) The name of the SIP header to change or add
  //  hdrValue: (String) Replacement value for the SIP header
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var srcAddr = msg.getSourceAddress();
  //  msg.hdrSet("RemoteIP", srcAddr);
  //
{
  msgHdrSet(this._request, headerName, hdrValue);
}

SIPMessage.prototype.hdrRemove = function(headerName)
  //
  // This function will remove an existing SIP header. This fucntion must be
  // used carefully since altering SIP headers may directly affect proper
  // transaction operation.  This function will return true if successful.
  //
  // Parameters:
  //  headerName: (String) The name of the header to delete
  //
  // Return Type: Boolean
  //
  // Usage:
  // var msg = new SIPMessage(request);
  //  msg.hdrRemove("user-agent");
  //  msg.hdrSet("User-Agent", "My own cool user agent");
  //
{
  msgHdrRemove(this._request, headerName);
}

SIPMessage.prototype.hdrListAppend = function(headerName, headerValue)
  //
  // This function will append a new item in a set of existing list headers.
  // Example of list headers are routes and record-routes.  Take note that
  // appending means the header will be inserted at the bottom of the list.
  // This function will return true if successful.
  //
  // Parameters:
  //  headerName: (String) The name of the list header to append.
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.hdrListAppend("Record-Route", "<sip:myhost:5060;lr>");
  //
{
  msgHdrListAppend(this._request, headerName, headerValue);
}

SIPMessage.prototype.hdrListPrepend = function(headerName, headerValue)
  //
  // This function will prepend a new item in a set of existing list headers.
  // Example of list headers are routes and record-routes.  Take note that
  // prepending means the header will be inserted at the top of the list.
  // This function will return true if successful.
  //
  // Parameters:
  //  headerName: (String) The name of the list header to prepend.
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.hdrListAppend("Record-Route", "<sip:myhost:5060;lr>");
  //
{
  msgHdrListPrepend(this._request, headerName, headerValue);
}

SIPMessage.prototype.hdrListPopFront = function(headerName)
  //
  // This function pops the top most header from a set of existing list headers.
  // Example of list headers are routes and record-routes.  Popping a list header
  // deletes the header from the list and would return the raw string value
  // of the popped header or type-undefined if unsuccessful.
  //
  // Parameters:
  //  headerName: (String) The name of the list header to pop.
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  var topRR = msg.hdrListPopFront("Record-Route");
  //
{
  return msgHdrListPopFront(this._request, headerName);
}

SIPMessage.prototype.hdrListRemove = function(headerName)
  //
  // This function will remove an entire set (multiple headers) of list headers
  // and returns true if sucessful.
  //
  // Parameters:
  //  headerName: (String) The name of the list header set to remove.
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.hdrListRemove("Record-Route");
  //
{
  return msgHdrListRemove(this._request, headerName);
}

SIPMessage.prototype.isRequest = function(method)
  //
  // This function returns true if the SIP message is a request, false if it's
  // a response and type-undefined if there was an error. An optional method
  // parameter can be set to check against a specific method.
  //
  // Parameters:
  //  method: (String)(Optional) The specific method name to check
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.isRequest("INVITE"))
  //  {
  //    //Do your INVITE specific stuffs here
  //  }
  //
{
  return msgIsRequest(this._request, method);
}

SIPMessage.prototype.isResponse = function()
  //
  // This function returns true if the SIP message is a response, false if it's
  // a request and type-undefined if there was an error.
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.isResponse())
  //  {
  //    // Do your response specific stuffs here
  //  }
  //
{
  return msgIsResponse(this._request);
}

SIPMessage.prototype.is1xx = function()
  //
  // This function returns true if the SIP message is a response with code 1xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is1xx())
  //  {
  //    // Do your 1xx specific stuffs here
  //  }
  //
{
  return msgIs1xx(this._request);
}

SIPMessage.prototype.is2xx = function()
  //
  // This function returns true if the SIP message is a response with code 2xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is2xx())
  //  {
  //    // Do your 2xx specific stuffs here
  //  }
  //
{
  return msgIs2xx(this._request);
}

SIPMessage.prototype.is3xx = function()
  //
  // This function returns true if the SIP message is a response with code 3xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is3xx())
  //  {
  //    // Do your 3xx specific stuffs here
  //  }
  //
{
  return msgIs3xx(this._request);
}

SIPMessage.prototype.is4xx = function()
  //
  // This function returns true if the SIP message is a response with code 4xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is4xx())
  //  {
  //    // Do your 4xx specific stuffs here
  //  }
  //
{
  return msgIs4xx(this._request);
}

SIPMessage.prototype.is5xx = function()
  //
  // This function returns true if the SIP message is a response with code 5xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is5xx())
  //  {
  //    // Do your 5xx specific stuffs here
  //  }
  //
{
  return msgIs5xx(this._request);
}

SIPMessage.prototype.is6xx = function()
  //
  // This function returns true if the SIP message is a response with code 6xx
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.is6xx())
  //  {
  //    // Do your 6xx specific stuffs here
  //  }
  //
{
  return msgIs6xx(this._request);
}


SIPMessage.prototype.isErrorResponse = function()
  //
  // This function returns true if the SIP message is an error response
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.isErrorResponse())
  //  {
  //    // Do your error specific stuffs here
  //  }
  //
{
  return msgIsErrorResponse(this._request);
}

SIPMessage.prototype.isMidDialog = function()
  //
  // This function returns true if the SIP message is mid-dialog
  //
  // Parameters: void
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  if (msg.isMidDialog())
  //  {
  //    // Do your dialog specific stuffs here
  //  }
  //
{
  return msgIsMidDialog(this._request);
}

SIPMessage.prototype.getBody = function()
  //
  // This function returns the body of a SIPMessage if it exists or type-undefined
  // if it doesn't.
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  var contentType = msg.hdrGet("content-type");
  //  var sdp = "";
  //  if (contentType == "application/sdp")
  //    sdp = msg.getBody();
  //
{
  return msgGetBody(this._request);
}

SIPMessage.prototype.setBody = function(body)
  //
  // This function sets the value of the SIP message body.  It will return true
  // if successful.
  //
  // Parameters:
  //  body: (String) The value of the new body
  //
  // Return Type: Boolean
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  msg.hdrSet("content-type", "text/plain");
  //  msg.setBody("This is a simple plain text body");
  //
{
  msgSetBody(this._request, body);
}

SIPMessage.prototype.getStartLine = function()
  //
  // This function returns the raw start-line of the SIPMessage
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  var startLine = msg.getStartLine();
  //
{
  return msgGetStartLine(this._request);
}

SIPMessage.prototype.setStartLine = function(sline)
  //
  // This function returns the raw start-line of the SIPMessage
  //
  // Parameters:
  //  sline: (String) The new startline value
  //
  // Return Type: void
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  msg.setStartLine("INVITE sip:1234@192.168.0.10:5060 SIP/2.0");
  //
{
  msgSetStartLine(this._request, sline);
}

SIPMessage.prototype.getContactUri = function()
  //
  // This function returns the contact-uri of the SIPMessage
  //
  // Parameters: void
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  var contactUri = msg.getContactUri();
  //
{
    return msgGetContactUri(this._request);
}

SIPMessage.prototype.getContactParameter = function(parameter)
  //
  // This function returns the contact-uri-parameter of the SIPMessage
  // if one exists in the parameter list
  //
  // Parameters:
  //  parameter: (String) The name of the parameter
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  var expires = msg.getContactParameter("expires");
  //
{
    return msgGetContactParameter(this._request, parameter);
}

SIPMessage.prototype.getAuthenticator = function(realm)
  //
  // This function returns the raw Authorization or Proxy-Authorization header
  // for a particular realm if it exists in the SIPMessage.  The realm is
  // used to identify the exact header during cases where there are multiple
  // authorization headers.  Using * in the header means simply return the first
  // authorization header if it exists.  This is normally used by the script
  // during cases where the script explicitly channlenged a particular request
  // for authority.
  //
  // Parameters:
  //  parameter: (String) The realm of the authentiator
  //
  // Return Type: String
  //
  // Usage:
  //  var msg = SIPMessage(request);
  //  var expires = msg.getContactParameter("expires");
  //
{
    return msgGetAuthenticator(this._request, realm);
}

SIPMessage.prototype.routeByAOR = function(userComparisonOnly)
  //
  // This function will attempt to route the request using the registration table
  //
  // Parameters:
  //  userComparisonOnly: (Bool) flag to indicate whether to match the entire AOR or to just use the user
  //
  // Return Type: String
{
    return msgRouteByAOR(this._request, userComparisonOnly);
}

SIPMessage.prototype.resetMaxForwards = function(maxForwards)
  //
  // This function allows
  // rewriting of max-forwards header which might be too low a value to properly
  // allow SIP Messages to spiral in systems like sipXecs.  This function will
  // return false if the request already contains a via to points back to Karoo
  //
  // Parameters:
  //   maxForwards: (String) new value to be used for max-forwards header
  //
  // Return Type: Bool
  //
  // Usage:
  //  var msg = new SIPMessage(request);
  //  msg.resetMaxForwards("20");
  //
{
    return msgResetMaxForwards(this._request, maxForwards);
}
