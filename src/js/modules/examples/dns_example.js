"use-strict";

const async = require("async");
const dns = require("dns");
const system = require("system");


dns.lookup("A", "www.ossapp.com", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("A www.ossapp.com -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("SRV", "_sip._udp.ossapp.com", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("SRV _sip._udp.ossapp.com -> " +JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("AAAA", "ipv6.google.com", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("AAAA ipv6.google.com -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("AAAA", "ossapp.com", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("AAAA ossapp.com -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});


dns.lookup("MX", "ossapp.com", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("MX ossapp.com -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("A", "localhost", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("A localhost -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("A", "vmpool.gobitech.local", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("A vmpool.gobitech.local -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("A", "nohost.gobitech.local", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("A nohost.gobitech.local -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
});

dns.lookup("A", "bogusdomain.local", 2, function(result, common) 
{
  if (result !== undefined)
  {
    log_info("A bogusdomain.local -> " + JSON.stringify(common) + " " + JSON.stringify(result));
  }
  else
  {
    log_info("Request timeout");
  }
  system.exit(0);
});

async.processEvents();


