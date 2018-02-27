"use-strict"

const async = require("async");
const EventEmitter = async.EventEmitter;
const console = require("console");
const system = require("system");

var MyEmitter = function() {
  EventEmitter.call(this);
}
MyEmitter.prototype = Object.create(EventEmitter.prototype);

const myEmitter = new MyEmitter();
myEmitter.on('event', function(a, b) {
  console.log(a, b);
});

myEmitter.onAnyEvent(function(event, c, d) {
  console.log(event, c, d);
});

myEmitter.on('exit', function() {
  system.exit(0);
});

//
// Use the emit function
//
myEmitter.emit('event', 'a', 'b');
myEmitter.emit('any', 'c', 'd');
myEmitter.emit('exit');
