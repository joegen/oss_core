"use-strict"

const http = require("http");
const utils = require("utils");
const port = 3000;

const requestHandler = function(request, response) {
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

server.listen(port, function(err) {
  if (err) {
    return console.log('something bad happened', err)
  }
  console.log("server is listening on " + port)
});

http.get("localhost", port, "/", function(result) {
  console.log(result);
  exit(0);
});
