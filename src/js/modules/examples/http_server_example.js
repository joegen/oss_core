"use-strict"

const http = require("http");
const utils = require("utils");
const port = 3000;

const requestHandler = function(request, response)
{
  console.log(request.uri);
  response.setContentType("text/plain");
  var message = 'Hello oss_js Server!';
  response.setContentLength(message.length);
  
  // demonstrates multiple call to send
  response.send("Hello ");
  response.send("oss_js ");
  response.send("Server!");
}

const server = http.createServer(requestHandler)

server.listen(port, function (err)
{
  if (err) 
  {
    return console.log('something bad happened', err)
  }
  console.log("server is listening on " + port)
});

const HttpClient = http.HttpClient;
const HttpSession = http.HttpSession;
const HttpRequest = http.HttpRequest;

const HttpResponse = http.HttpResponse;

var client = new HttpClient();
client.setHost("localhost");
client.setPort(port);

var session = new HttpSession(client);
var contentLength = undefined;
session.on("response", function(response)
{
  contentLength = response.getContentLength();
  session.read(contentLength ? contentLength : 1024);
});

var buff = new Array();
session.on("read", function(data)
{
  if (data.length > 0)
  {
    buff = buff.concat(data);
    if (contentLength)
    {
      //
      // We are done.  We have a content length so we are sure we read the correct amount
      //
      try
      {
        console.log(utils.bufferToString(buff));
        system.exit(0);
      }
      catch(e)
      {
        e.printStackTrace();
      }
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
