"use-strict";

const TcpSocketServer = require("socket").TcpSocketServer;
const TcpSocketClient = require("socket").TcpSocketClient;
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

var server = new TcpSocketServer("127.0.0.1", "50002");
server.on("accept", function(connection) {
  connection.on("read", function(socket) {
    var result = socket.read(BUF_SIZE);
    var peer = socket.getRemoteAddress();
    console.log("Server Received " + result.size + " bytes " + bufferToString(result.data) + " from " + peer.host + ":" + peer.port);
    assert(socket.write(result.data) > 0);
  });
  return true;
});

var client = new TcpSocketClient("127.0.0.1", "50002");
client.on("read", function(socket) {
  var result = socket.read(BUF_SIZE);
  if (result.size > 0) {
    var peer = socket.getRemoteAddress();
    console.log("Client Received " + result.size + " bytes " + bufferToString(result.data) + " from " + peer.host + ":" + peer.port);
    client.close();
    server.close();
    system.exit(0);
  }
});

client.write("Hello!");