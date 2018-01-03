"use-strict";
var isolate = require("isolate");

var script = utils.multiline(function(){ /*
  "use-strict";
  var isolate = require("isolate");
  var count = 0;
  
  var threadName;
  isolate.on("setThreadName", function(args)
  {
    threadName = args;
  });
  
  isolate.on("terminate", function(args)
  {
    console.log("Stopping event loop on terminate event", threadName);
    async.__stop_event_loop();
  });
  
  var on_timer = function(msg, start)
  {
    var now = new Date();
    var elapsed = now.getTime() - start.getTime();
    isolate.notifyParentIsolate("timerFire", msg + " elapsed: " + elapsed + " milliseconds" + "name: " + threadName);
    if (++count == 10)
    {
      isolate.notifyParentIsolate("isolateTerminated", threadName);
      return;
    }
    async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
  }
  async.setTimeout(on_timer, 100, ["Hello Timers!", new Date()]);
*/ });


isolate.on("timerFire", function(args)
{
  console.log(args);
});

isolate.on("isolateTerminated", function(args){
  console.log(args, "terminated");
  if (args === "thread1")
  {
    thread1.notify("terminate");
    thread1.join();
    thread1 = null;
  }
  else
  {
    thread2.notify("terminate");
    thread2.join();
    thread2 = null;
  }
  
  if (!thread1 && !thread2)
  {
    exit(0);
  }
});


var thread1 = isolate.create();
var thread2 = isolate.create();

thread1.runSource(script);
thread1.execute("setThreadName", "thread1");

thread2.runSource(script);
thread2.execute("setThreadName", "thread2");






