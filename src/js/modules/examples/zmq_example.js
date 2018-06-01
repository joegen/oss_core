"use-strict";
const zmq = require("zmq");
const assert = require("assert");
const log = require("logger");
const async = require("async");
const system = require("system");

function req_rep_example() {
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

function rpc_server_example() {
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

function pub_sub_example() {
  var publisher = new zmq.ZMQSocket(zmq.PUB);
  var subscriber = new zmq.ZMQSocket(zmq.SUB);

  assert(publisher.bind("tcp://127.0.0.1:50001"));
  assert(subscriber.connect("tcp://127.0.0.1:50001"));
  assert(subscriber.subscribe("test-event"));
  
  subscriber.start(function() {
    var msg = new Buffer(1024);
    subscriber.receive(msg);
    log.log(log.INFO, msg.toString());
    subscriber.close();
    publisher.close();
    system.exit(0);
  });
  system.sleep(1);
  log.log(log.INFO, "Sending event");
  publisher.publish("test-event: Hello World!");
}
  
pub_sub_example();

