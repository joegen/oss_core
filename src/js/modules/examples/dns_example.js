"use-strict";

const async = require("async");
const dns = require("dns");
const system = require("system");

var logcount = 0;
for (var i = 0; i < 100; i++)
{
  dns.lookup("A", "www.ossapp.com", 2, function(result, common) 
  {
    if (result !== undefined)
    {
      log_info(logcount + JSON.stringify(result));
      log_info(logcount++ + JSON.stringify(common));
    }
    else
    {
      log_info(logcount++ + "Request timeout");
    }
    if (logcount == 100)
    {
      system.exit(0);
    }
  });
}

async.processEvents();


