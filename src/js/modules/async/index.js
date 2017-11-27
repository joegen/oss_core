"use-strict";

const _async = require("_async.jso");
const _emitter = require("./event_emitter.js");

__copy_exports(_async, exports);
__copy_exports(_emitter, exports);

var json_parse = function()
{
  return JSON.parse.apply(this, arguments);
}

_async.__set_json_parser(json_parse);


