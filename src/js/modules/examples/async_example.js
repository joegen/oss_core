"use-strict";

const async = require("async");
const sleep = require("system").sleep;

async.call(function(message, sleepTime) {
    log_info(message);
    log_info("Zzzzzzzzzz....");
    sleep(sleepTime);
    return "Hello, world!";
  }, ["I am about to sleep", 2],
  function(result) {
    log_info(result);
    exit(0);
  });

log_info("I need to sleep");