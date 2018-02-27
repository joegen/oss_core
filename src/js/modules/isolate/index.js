"use-strict";

const _isolate = require("isolate");
const Isolate = _isolate.Isolate;

__copy_exports(_isolate, exports);

var create = exports.create = function() {
  return new Isolate(system.thread_self());
}
