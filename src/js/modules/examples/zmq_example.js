"use-strict";
const zmq = require("zmq");
const assert = require("assert");
const log = require("logger");
const async = require("async");
const system = require("system");

function example1() {
  var requester = new zmq.ZMQSocket(zmq.REQ);
  var responder = new zmq.ZMQSocket(zmq.REP);

  assert(responder.bind("tcp://127.0.0.1:50000"));
  assert(requester.connect("tcp://127.0.0.1:50000"));
  responder.start(function() {
    var msg = new Buffer(1024);
    responder.receive(msg);
    log.log(log.INFO, msg.toString());
    var response = new Buffer("Bye ZeroMQ!");
    assert(responder.send(response));
  });

  requester.start(function() {
    var msg = new Buffer(1024);
    requester.receive(msg);
    log.log(log.INFO, msg.toString());
    responder.close();
    requester.close();
    system.exit(0);
  });

  var request = new Buffer("Hello ZeroMQ!");
  assert(requester.send(request));
}

function example2() {
  zmq.zmq_rpc_server(
    "tcp://127.0.0.1:50001", {
      "ping": function(args) {
        return "pong";
      },
      "hello": function(args) {
        return "Hi there!";
      },
      "bye": function(args) {
        return "See you!";
      }
    }
  );
}

example1();