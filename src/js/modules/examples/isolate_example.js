"use-strict";
var isolate = require("isolate");

var script = utils.multiline(function(){ /*
  "use-strict";
  var count = 0;
  var on_timer = function(msg, start)
  {
    var now = new Date();
    var elapsed = now.getTime() - start.getTime();
    console.log( msg + " elapsed: " + elapsed + " milliseconds");
    if (++count == 10)
    {
      console.log("Stopping event loop");
      async.__stop_event_loop();
      return;
    }
    async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
  }
  async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
*/ });

var delegate =  utils.multiline(function(){ /*
  "use-strict";
  var isolate = require("isolate");
  isolate.on("ping", function(args)
  {
    var result = new Object();
    result.result = "pong";
    result.args = args;
    return result;
  });
*/ });

var thread0 = isolate.create();
var thread1 = isolate.create();
var thread2 = isolate.create();

thread0.runSource(delegate);
console.log(JSON.stringify(thread0.execute("ping", "testing delegates")));

thread1.runSource(script);
thread2.runSource(script);


console.log("Joining thread 1");
thread1.join();
console.log("Joining thread 2");
thread2.join();

exit(0);



