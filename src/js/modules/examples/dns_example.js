"use-strict";

const async = require("async");
const dns = require("dns");
const system = require("system");

var logcount = 0;
for (var i = 0; i < 100; i++)
{
  dns.lookup("A", "www.ossapp.com", function(result, common) 
  {
    log_info(logcount + JSON.stringify(result));
    log_info(logcount++ + JSON.stringify(common));
    if (logcount == 100)
    {
      system.exit(0);
    }
  });
}

async.processEvents();


