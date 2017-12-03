"use-strict"

const utils = require("utils");
const http = require("http");
const HttpClient = http.HttpClient;
const HttpSession = http.HttpSession;
const HttpRequest = http.HttpRequest;

const HttpResponse = http.HttpResponse;

var client = new HttpClient();
client.setHost("www.ossapp.com");
client.setPort(80);

var session = new HttpSession(client);
var _this = this;
this.contentLength = undefined;
session.on("response", function(response)
{
  _this.contentLength = response.getContentLength();
  session.read(_this.contentLength ? _this.contentLength : 1024);
});

this.buff = new Array();
session.on("read", function(data)
{
  if (data.length > 0)
  {
    _this.buff = _this.buff.concat(data);
    if (_this.contentLength)
    {
      //
      // We are done.  We have a content length so we are sure we read the correct amount
      //
      console.log(utils.bufferToString(_this.buff));
      system.exit(0);
    }
    else
    {
      //
      // read some more since we dont know exactly how much data there is
      //
      session.read(1024); 
    }
  }
  else
  {
    //
    // No more data left to read.  We are done.
    //
    console.log(utils.bufferToString(_this.buff));
    system.exit(0);
  }
});

session.on("error", function(message)
{
  console.log(message);
  system.exit(0);
});

var request = new HttpRequest();
request.setVersion(http.HTTP_1_1);
request.setMethod(http.HTTP_GET);
request.setUri("/");
request.setContentLength(5);
request.setContentType("application/text");
session.send(request, "hello");
