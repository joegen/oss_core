"use-strict";

var console = require("console");
var async = require("async");
var CPPRPC = require("cpp_rpc").CPPRPC;
var cpp_rpc_tester = require("cpp_rpc_tester");
var syste = require("system");


CPPRPC.on("ping", function()
{
  console.log("got ping");
  result = new Object();
  result.result = "pong";
  return result;
});

cpp_rpc_tester.start_test();

async.setTimeout(syste.exit, 100, [0]);

async.processEvents();


