
"use-strict";
const zmq = require("zmq");
const assert = require("assert");
const async = require("async");
const system = require("system");
const console = require("console");
const Server = zmq.ZmqRpcServer;
const Client = zmq.ZmqRpcClient;

var server = new Server("tcp://127.0.0.1:50001");
var client = new Client("tcp://127.0.0.1:50001");


server.on("ping",
  function()
  {
    return "pong";
  }
);

server.on("multiply",
  function(params)
  {
    return params.var1 * params.var2
  }
);

server.on("exit",
  function(params)
  {
    return "bye";
  }
);

client.on("ping", 
  function(result) 
  {
    console.log("ping result -> " + result);
  }
);

client.on("multiply", 
  function(result) 
  {
    console.log("multiply result -> " + result);
  }
);

client.on("error", 
  function(method, error) 
  {
    console.log(method + " error result -> " + error.message);
  }
);

client.on("exit", 
  function(result) 
  {
    console.log("exit result -> " + result);
    system.exit(0);
  }
);

client.execute("ping", {});
client.execute("multiply", { var1 : 10, var2 : 20 });
client.execute("bogus", {});
client.execute("exit", {});




