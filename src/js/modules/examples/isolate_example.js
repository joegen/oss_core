"use-strict";
var isolate = require("isolate");

var script = utils.multiline(function(){/*
  "use-strict";
  var count = 0;
  var on_timer = function(msg, start)
  {
    var now = new Date();
    var elapsed = now.getTime() - start.getTime();
    console.log( msg + " elapsed: " + elapsed + " milliseconds");
    if (++count == 10)
    {
      exit(0);
    }
    async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
  }
  async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
*/});

var thread1 = isolate.create();
var thread2 = isolate.create();

thread1.runSource(script);
thread2.runSource(script);

thread1.join();
thread2.join();

