"use-strict"

const utils = require("utils");
const http = require("http");
const HttpClient = http.HttpClient;
const HttpSession = http.HttpSession;
const HttpRequest = http.HttpRequest;
const HttpResponse = http.HttpResponse;

var test_client = function(session, uri, exitOnCompletion) {
  var buff = new Array();
  var contentLength;
  session.on("read", function(data) {
    if (data.length > 0) {
      buff = buff.concat(data);
      if (contentLength) {
        //
        // We are done.  We have a content length so we are sure we read the correct amount
        //
        console.log(utils.bufferToString(buff));
        if (exitOnCompletion) {
          system.exit(0);
        }
      } else {
        //
        // read some more since we dont know exactly how much data there is
        //
        session.read(1024);
      }
    } else {
      //
      // No more data left to read.  We are done.
      //
      console.log(utils.bufferToString(buff));
      if (exitOnCompletion) {
        system.exit(0);
      }
    }
  });

  session.on("error", function(message) {
    console.log(message);
    system.exit(0);
  });

  var request = new HttpRequest();
  request.setVersion(http.HTTP_1_1);
  request.setMethod(http.HTTP_GET);
  request.setUri(uri);
  request.setContentLength(5);
  request.setContentType("application/text");
  return session.send(request, function(request) {
    //
    // Write the body then trigger a receiveResponse
    //
    session.write("Hello");
    session.receiveResponse(function(response) {
      contentLength = response.getContentLength();
      if (typeof contentLength === "number" && contentLength == -1) {
        contentLength = 0;
      }
      session.read(contentLength ? contentLength : 1024);
    });
  });
}


var client = new HttpClient(false);
client.setHost("www.ossapp.com");
client.setPort(80);
var session = new HttpSession(client);
test_client(session, "/", false);


var cert = "/home/joegen/Devel/ProvidenceGW/examples/data/testCert.pem";
var caLoc = "/home/joegen/Devel/ProvidenceGW/examples/data/";

var verifyInvalidCertificate = function(cert) {
  console.log(JSON.stringify(cert));
  return true;
}

try {
  http.ssl.registerClientContext(
    cert,
    function() {
      console.log("Password Verification")
    },
    cert,
    verifyInvalidCertificate,
    caLoc
  );
} catch (e) {
  console.log(e);
  exit(-1);
}
var secureClient = new HttpClient(true);
secureClient.setHost("github.com");
secureClient.setPort(443);
var secureSession = new HttpSession(secureClient);
test_client(secureSession, "/joegen/oss_core/", false);

http.get("www.ossapp.com", 80, "/karoo/", function(result) {
  console.log(result);
  exit(0);
});
