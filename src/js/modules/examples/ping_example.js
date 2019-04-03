
//
// NOTE:  ICMP sockets require root privilege.  If you are running this
// using a normal user.  Execute the following as root
// `setcap cap_net_raw+ep /usr/bin/oss_core`

"use-strict";

var system = require("system");
var assert = require("assert");
var pinger = require("pinger");
var console = require("console");
var async = require("async");

var i = 0;
var count = 10;
var timeout = 500; // millisseconds
var host = "bridge.ossapp.com";


var ping = function()
{
    pinger.ping(host, i, timeout, function(reply) {
        console.log(JSON.stringify(reply));
    });
    
    if (i < count) {
        i++;
        async.setTimeout(ping, timeout, [0]);
    } else {
        system.exit(0);
    }
}

ping();