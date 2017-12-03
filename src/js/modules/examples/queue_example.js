"use-strict";

const async = require("async");
const system = require("system");
const log = require("logger");

var on_event = function(args1, args2)
{
  if (args1 == -1)
  {
    system.exit(0);
  }
  log.log(log.INFO, args2 + " -> " + args1);
}

var queue = new async.Queue(on_event);
for (var i = 0; i < 10; i++)
{
  queue.enqueue([i,"args"]);
}
queue.enqueue([-1]);
