
"use-strict";
const zmq = require("zmq");
const assert = require("assert");
const log = require("logger");
const async = require("async");
const system = require("system");

function example1()
{
  zmq.zmq_rpc_server(
    "tcp://127.0.0.1:50001",
    {
      "ping" : function(args)
      {
        return "pong";
      },
      "hello" : function(args)
      {
        return "Hi there!";
      },
      "bye" : function(args)
      {
        return "See you!";
      }
    }
  );
}

example1();

async.processEvents();



