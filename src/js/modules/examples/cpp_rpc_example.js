"use-strict";

var isolate = require("isolate");
var cpp_rpc_tester = require("examples/cpp_rpc_tester.jso");



isolate.on("ping", function(args, userData) {
  console.log("got ping message: " + args.message);
  console.log("got user data: " + cpp_rpc_tester.parse_user_data(userData));

  var result = new Object();
  result.result = "pong";
  return result;
});

cpp_rpc_tester.start_test();

async.setTimeout(system.exit, 100, [0]);