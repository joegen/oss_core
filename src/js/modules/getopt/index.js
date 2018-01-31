"use-strict";
var _opt = require("./_getopt.jso");
exports.getopt = _opt.getopt;
exports.argc = _opt.argc;
exports.argv = _opt.argv;

  
Object.defineProperties(exports, {
  "optind" : { get: function() { return _opt.optind; } },
  "optind" : { set: function(val) {_opt.optind = val} },
  "opterr" : { get: function() { return _opt.opterr; } },
  "opterr" : { set: function(val) {_opt.opterr = val} },
  "optarg" : { get: function() { return _opt.optarg; } },
  "optopt" : { get: function() { return _opt.optopt; } }
});

