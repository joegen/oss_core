
"use-strict";

const async = require("async");
const sleep = require("system").sleep;
const log = require("logger");

async.call(function(message, sleepTime)
{
  log.log(log.INFO, message);
  log.log(log.INFO, "Zzzzzzzzzz....");
  sleep(sleepTime);
  return "Hello, world!";
}, 
["I am about to sleep", 2], 
function(result)
{
  log.log(log.INFO, result);
  require("system").exit(0);
});

log.log(log.INFO, "I need to sleep");

async.processEvents();