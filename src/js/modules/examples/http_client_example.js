"use-strict"

const utils = require("utils");

const system = require("system");
const async = require("async");

const http = require("http");
const HttpClient = http.HttpClient;
const HttpRequest = http.HttpRequest;
const console = require("console");
const HttpResponse = http.HttpResponse;




var client = new HttpClient();
client.setHost("www.ossapp.com");
client.setPort(80);

client.on("response", function(request, response)
{
  if (response.hasContentLength())
  {
    var ret = client.read(response.getContentLength());
    if (ret.length > 0)
    {
      console.log(utils.bufferToString(ret));
    }
    //
    // Not really needed but this will free up the memory sooner
    //
    request.dispose();
    response.dispose();
  }
  system.exit(0);
});

client.on("error", function(request, e)
{
  console.log(e);
  request.dispose();
  system.exit(0);
});

var request = new HttpRequest();
request.setVersion(http.HTTP_1_1);
request.setMethod(http.HTTP_GET);
request.setUri("/");
request.setContentLength(5);
request.setContentType("application/text");


client.send(request);
client.write("hello");

log_info("Sent request");

async.processEvents();
