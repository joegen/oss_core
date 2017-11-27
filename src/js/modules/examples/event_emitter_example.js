
"use-strict"

const async = require("async");
const EventEmitter = async.EventEmitter;
const console = require("console");
const system = require("system");

var MyEmitter = function()
{
  EventEmitter.call(this);
}
MyEmitter.prototype = Object.create(EventEmitter.prototype);

const myEmitter = new MyEmitter();
myEmitter.on('event', function(event, a, b) 
{
  console.log(event, a, b);
});

myEmitter.on('exit', function(event, a, b) 
{
  system.exit(0);
});

//
// Use the emit function
//
myEmitter.emit('event', 'a', 'b');

//
// Use the async emitter.  You may also emit from C++ directly using QueueObject::json_enqueue
// and receive the event in JS layer.
//
async.emit_json_string(myEmitter._fd,'["exit"]');

async.processEvents();