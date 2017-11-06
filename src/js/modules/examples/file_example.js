const File = require("file").File;
const assert = require("assert");
const console = require("console");

var writer = new File("test_file.txt", "w+");
assert(writer.writeLine("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."));
assert(writer.writeLine("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."));
assert(writer.writeLine("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."));
assert(writer.writeLine("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."));
writer.close();


var reader = new File("test_file.txt", "r");

while (!reader.eof())
{
  var line = reader.readLine();
  if (typeof line !== "undefined")
  {
    console.log("LINE: " + line);
  }
}
reader.close();
