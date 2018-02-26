"use-strict";

var system = require("system");
var assert = require("assert");
var consts = require("constants");

var pipefd = system.pipe();
assert.ok(typeof pipefd === "object");
assert.ok(typeof pipefd[0] == "number");
assert.ok(typeof pipefd[1] == "number");

var pid = system.fork();
assert.ok(typeof pid === "number");
assert.ok(pid !== -1);

if (pid === 0) {
  // Child reads from pipe
  system.close(pipefd[1]);
  while (true) {
    var result = system.read(pipefd[0], 1);
    if (typeof result !== "object")
      break;
    system.write(consts.STDOUT_FILENO, result);
  }
  system.write(consts.STDOUT_FILENO, "\n");
  system.close(pipefd[0])
  system._exit(0);
} else {
  // Parent writes to the pipe
  system.close(pipefd[0]);
  system.write(pipefd[1], "Hello Pipes!");
  system.close(pipefd[1]);
  system.wait();
  system.exit(0);
}