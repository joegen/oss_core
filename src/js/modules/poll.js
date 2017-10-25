"use-strict";

var _poll = require("_poll");

function copyProps (src, dst) 
{
  for (var key in src) 
  {
    if (key !== "poll")
    {
      dst[key] = src[key]
    }
  }
}

exports.poll = function(pfds, timeout)
{
  if (timeout !== null) 
  {
    timeout = parseFloat(timeout);
    if (isNaN(timeout) || timeout < 0) 
    {
      throw new TypeError("timeout must be null or a positive number");
    }
    return _poll.poll(pfds, timeout);
  }
}
