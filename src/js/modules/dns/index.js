"use-strict";

const _dns = require("./_dns.jso");
const async = require("async");
const Fifo = require("fifo").Fifo;

var work_items = new Fifo();

var on_work_timeout = function(work, timeout, cb)
{
  work.valid = false;
  cb(undefined, undefined);
  do_one_work();
}

var insert_work = function(type, query, timeout, cb)
{
  var work = new Object();
  work.type = type;
  work.query = query;
  work.cb = cb;
  work.timeout = timeout;
  work.timerId = async.setTimeout(on_work_timeout, timeout * 1000, [work, timeout, cb]);
  work.valid = true;
  work_items.push(work);
}

var do_one_work = function()
{
  while (true)
  {
    if (!work_items.empty())
    {
      var work = work_items.pop();
      if (work.valid)
      {
        async.clearTimeout(work.timerId);
        lookup(work.type, work.query, work.timeout, work.cb);
        break;
      }
    }
    else
    {
      break;
    }
  }
}

_dns._set_work_callback(do_one_work);

var on_context_timer = function(context, timeoutSec, cb)
{
  if ((context.timeout -= timeoutSec) <= 0)
  {
    _dns._relinquish_context(context);
    do_one_work();
    cb(undefined, undefined);
    return;
  }
  _dns._get_next_context_timeout(context);
  context.timerId = async.setTimeout(on_context_timer, 1000, [context, timeoutSec, cb]);
}

var lookup = function(type, query, timeout, cb)
{
  var contextCount = _dns._get_context_count();

  if (contextCount == 0)
  {
    insert_work(type, query, timeout, cb);
    return;
  }
  
  var context = _dns._acquire_context();
  var contextTimeout = 1;
  
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
    async.clearTimeout(context.timerId);
    _dns._process_io_events(fd);
  });
  
  //
  // start the next timer for this context
  //
  context.timeout = timeout;
  context.timerId = async.setTimeout(on_context_timer, 1000, [context, 1, cb]);
  
  return context;
}

exports.lookup = lookup;
