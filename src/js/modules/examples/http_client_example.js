"use-strict"

const utils = require("utils");

const system = require("system");
const async = require("async");

const http = require("http");
const HttpClient = http.HttpClient;
const HttpSession = http.HttpSession;
const HttpRequest = http.HttpRequest;
const console = require("console");
const HttpResponse = http.HttpResponse;

var client = new HttpClient();
client.setHost("www.ossapp.com");
client.setPort(80);

var session = new HttpSession(client);

session.on("response", function()
{
  session.read(session._response.getContentLength());
});

session.on("read", function(size)
{
  if (size > 0)
  {
    var data = session.readBuffer(size);
    console.log(utils.bufferToString(data));
  }
  system.exit(0);
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

async.processEvents();
