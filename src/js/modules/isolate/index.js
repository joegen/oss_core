"use-strict";

const _isolate = require("./_isolate.jso");
const Isolate = _isolate.Isolate;

var create = exports.create = function()
{
  return new Isolate(system.thread_self());
}
