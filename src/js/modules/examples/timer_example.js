"use-strict";
var async = require("async");
var console = require("console");
var system = require("system");
var count = 0;
var on_timer = function(msg, start) {
  var now = new Date();
  var elapsed = now.getTime() - start.getTime();
  console.log(msg + " elapsed: " + elapsed + " milliseconds");
  if (++count == 10) {
    system.exit(0);
  }
  async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
}

async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);