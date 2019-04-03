const _pinger = require("./_pinger.jso");
const assert = require("assert");
exports.ping = function(host, ttl, sequence, callback)
{
    assert(typeof callback == "function");
    return _pinger._ping_host(host, ttl, sequence, callback);
}
