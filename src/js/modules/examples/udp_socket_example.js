"use-strict";

const UdpSocketServer = require("socket").UdpSocketServer;
const UdpSocketClient = require("socket").UdpSocketClient;
const async = require("async");
const system = require("system");
const console = require("console");
const sock = require("socket/_inet_socket");
const assert = require("assert");

function bufferToString(buf) {
  return buf.map(function(i) {
    return String.fromCharCode(i)
  }).join("")
}

const BUF_SIZE = 100;

var server = new UdpSocketServer("127.0.0.1", "50001");
server.on("read", function(socket) {
  var result = socket.receiveFrom(BUF_SIZE);
  console.log("Received " + result.size + " bytes from " + result.host + ":" + result.port + " " + bufferToString(result.data));
  assert(socket.sendTo(result.host, result.port, result.data) > 0);
});

var client = new UdpSocketClient(sock.SOCKET_IPV4);
client.on("read", function(socket) {
  var result = socket.receiveFrom(BUF_SIZE);
  console.log("Received " + result.size + " bytes from " + result.host + ":" + result.port + " " + bufferToString(result.data));
  client.close();
  server.close();
  system.exit(0);
});

client.connect("127.0.0.1", "50001");
client.write("Hello!");
