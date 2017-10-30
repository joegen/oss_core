"use-strict";
const zmq = require("zmq");
const assert = require("assert");
const log = require("logger");
const async = require("async");
const system = require("system");

var requester = new zmq.ZMQSocket(zmq.REQ);
var responder = new zmq.ZMQSocket(zmq.REP);

assert(responder.bind("tcp://127.0.0.1:50000"));
assert(requester.connect("tcp://127.0.0.1:50000"));
responder.start(function()
{
  var msg = responder.receive();
  log.log(log.INFO, msg.toString());
  var response = new Buffer("Bye ZeroMQ!");
  assert(responder.send(response));
});

requester.start(function()
{
  var msg = requester.receive();
  log.log(log.INFO, msg.toString());
  responder.close();
  requester.close();
  system.exit(0);
});

var request = new Buffer("Hello ZeroMQ!");
assert(requester.send(request));

async.processEvents();



