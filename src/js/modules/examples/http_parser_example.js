"use-strict"

const console = require("console");
const HttpParser = require("http").HttpParser;
const assert = require("assert");

var request = '';
request += 'POST /cgi-bin/process.cgi HTTP/1.1\r\n'
request += 'User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n'
request += 'Host: www.ossapp.com\r\n'
request += 'Content-Type: text/xml; charset=utf-8\r\n'
request += 'Content-Length: 95\r\n'
request += 'Accept-Language: en-us\r\n'
request += 'Accept-Encoding: gzip, deflate\r\n'
request += 'Connection: Keep-Alive\r\n'
request += '\r\n'
request += '<?xml version="1.0" encoding="utf-8"?>\r\n'
request += '<string xmlns="http://clearforest.com/">string</string>'

var parser = new HttpParser();

var buf = new Buffer(request);
var parsedLen = parser.parse(buf);
assert(parsedLen == buf.size());
assert(parser.isHeadersComplete());
assert(parser.isMessageComplete());
assert(parser.isRequest());
assert(parser.getHeaderCount() == 7);

console.log(parser.getMethod() + " " + parser.getUrl() + " HTTP/" + parser.getMajorVersion() + "." + parser.getMinorVersion());
console.log("User-Agent: " + parser.getHeader("User-Agent"));
console.log("Host: " + parser.getHeader("Host"));
console.log("Content-Type: " + parser.getHeader("Content-Type"));
console.log("Content-Length: " + parser.getHeader("Content-Length"));
console.log("Accept-Language: " + parser.getHeader("Accept-Language"));
console.log("Accept-Encoding: " + parser.getHeader("Accept-Encoding"));
console.log("Connection: " + parser.getHeader("Connection"));
console.log("");
console.log(parser.getBody());

for (var i = 0; i < parser.getHeaderCount(); i++) {
  var header = parser.getHeaderAt(i);
  console.log(i + "-> " + header.header + ": " + header.value);
}

//
// Explicitly exit to termiante the event loop
//
exit(0);