const system = require("system");
const object = require("object");
const defines = require("consts");

var Console = function(cout, cerr)
{
  if (object.isNumber(cout))
  {
    this.out = cout;
  }
  else
  {
    this.out = defines.STDOUT_FILENO;
  }
  
  if (object.isNumber(cerr))
  {
    this.err = cerr;
  }
  else
  {
    this.cerr = defines.STDERR_FILENO;
  }
}

Console.prototype.log = function(msg)
{
  msg += "\n";
  system.write(this.out, msg);
}

Console.prototype.error= function(msg)
{
  msg += "\n";
  system.write(this.err, msg);
}
exports.Console = Console;

exports.log = function(msg)
{
  msg += "\n";
  system.write(defines.STDIN_FILENO, msg);
}

exports.error = function(msg)
{
  msg += "\n";
  system.write(defines.STDERR_FILENO, msg);
}
