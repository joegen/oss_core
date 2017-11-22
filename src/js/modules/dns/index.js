"use-strict";

const _dns = require("./_dns.jso");
const async = require("async");
const Fifo = require("fifo").Fifo;

var work_items = new Fifo();

var insert_work = function(type, query, cb)
{
  var work = new Object();
  work.type = type;
  work.query = query;
  work.cb = cb;
  work_items.push(work);
}

var do_one_work = function()
{
  if (!work_items.empty())
  {
    var work = work_items.pop();
    lookup(work.type, work.query, work.cb);
  }
}

_dns._set_work_callback(do_one_work);

var lookup = function(type, query, cb)
{
  var contextCount = _dns._get_context_count();

  if (contextCount == 0)
  {
    insert_work(type, query, cb);
    return;
  }
  
  var context = _dns._acquire_context();
  
  if (type === "A")
  {
    _dns._lookup_a(query, cb, context);
  }
  else
  {
    throw new Error("Invalid type");
  }
  async.monitorFd(context.fd, function(fd, revents)
  {
    _dns._process_io_events(fd);
  });
  
  return context;
}

exports.lookup = lookup;
