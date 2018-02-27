const MonitoredFile = require("inotify").MonitoredFile;
const File = require("file").File;
const console = require("console");
const assert = require("assert");
const system = require("system");
const async = require("async");
const fs = require("filesystem");

const filename = "test_file.txt";

fs.remove(filename);
var writer = new File(filename, "w+");
assert(fs.exists(filename));

var monitor = new MonitoredFile(filename);
monitor.on("access", function(file) {
  console.log(file.path + "-> access")
});
monitor.on("attribute", function(file) {
  console.log(file.path + "-> attribute")
});
monitor.on("close_nowrite", function(file) {
  console.log(file.path + "-> close_nowrite")
});
monitor.on("close_write", function(file) {
  console.log(file.path + "-> close_write")
});
monitor.on("create", function(file) {
  console.log(file.path + "-> create")
});
monitor.on("delete", function(file) {
  console.log(file.path + "-> delete")
});
monitor.on("delete_self", function(file) {
  console.log(file.path + "-> delete_self")
});
monitor.on("modify", function(file) {
  console.log(file.path + "-> modify")
});
monitor.on("move_self", function(file) {
  console.log(file.path + "-> move_self")
});
monitor.on("move_from", function(file) {
  console.log(file.path + "-> move_from")
});
monitor.on("move_to", function(file) {
  console.log(file.path + "-> move_to")
});
monitor.on("open", function(file) {
  console.log(file.path + "-> open")
});
monitor.on("unmount", function(file) {
  console.log(file.path + "-> unmount")
});

assert(writer.writeLine("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."));
assert(writer.writeLine("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."));
assert(writer.writeLine("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."));
assert(writer.writeLine("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."));
writer.close();

var reader = new File(filename, "r");
while (!reader.eof()) {
  var line = reader.readLine();
}
reader.close();

fs.remove(filename);

function cleanup() {
  monitor.unmonitor();
  system.exit(0);
}

async.setTimeout(cleanup, 100, []);
