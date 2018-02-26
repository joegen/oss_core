const system = require("system");
const object = require("object");
const defines = require("constants");

var dump_log = function(out, args) {
  var msg;
  if (args.length == 1) {
    msg = args[0];
  } else {
    for (var i = 0; i < args.length; i++) {
      if (i == 0)
        msg = args[i];
      else
        msg += args[i];

      if (i < args.length - 1) {
        msg += " ";
      }
    }
  }

  if (typeof msg === "undefined") {
    msg = "undefined";
  }

  if (out) {
    msg += "\n";
    system.write(out, msg.toString());
  } else {
    system.cout(msg.toString());
    system.endl();
  }
}

var Console = function(cout, cerr) {
  if (object.isNumber(cout)) {
    this.out = cout;
  } else {
    this.out = defines.STDOUT_FILENO;
  }

  if (object.isNumber(cerr)) {
    this.err = cerr;
  } else {
    this.cerr = defines.STDERR_FILENO;
  }
}

Console.prototype.log = function() {
  dump_log(this.out, arguments);
}

Console.prototype.error = function() {
  dump_log(this.err, arguments);
}
exports.Console = Console;

exports.log = function() {
  dump_log(undefined, arguments);
}

exports.error = function() {
  dump_log(defines.STDERR_FILENO, arguments);
}

exports.printStackTrace = function(e) {
  exports.error(e + "\n" + e.stack);
}