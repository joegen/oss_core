"use-strict";

const sock = require("inet_socket");
const system = require("system");
const async = require("async");
const error = require("error");
const poll = require("poll");
const buffer = require("buffer");
const console = require("console");

const READ_BUFFER_SIZE = 1024 * 10;
var READ_BUFFER = new Buffer(READ_BUFFER_SIZE);

const WRITE_BUFFER_SIZE = 1024 * 10;
var WRITE_BUFFER = new Buffer(WRITE_BUFFER_SIZE);


 var receive_from = function(socket, size, flags)
{
  if (typeof flags === "undefined")
  {
    flags = 0;
  }
  var ret = sock.recvfrom_inet_dgram_socket(socket._fd, socket._read_buffer, size, flags);

  var result = new Object();
  result.size = ret.size;
  result.host = ret.host
  result.port = ret.port;
  if (ret.size > 0)
  {
    result.data = socket._read_buffer.toArray(ret.size);
  }
  else
  {
    result.data = [];
  }
  return result;
}
  
var send_to = function(socket, host, port, data, flags)
{
  var size = 0;
  if (buffer.isBuffer(data))
  {
    socket._write_buffer.fromBuffer(data, false);
    size = data.size();
  }
  else if (typeof data === "string")
  {
    socket._write_buffer.fromString(data, false);
    size = data.length;
  }
  else if (typeof data === "array" || typeof data === "object")
  {
    socket._write_buffer.fromArray(data, false);
    size = data.length;
  }
  else
  {
    throw new Error("Invalid data argument.  Must be buffer, array or string");
  }

  if (typeof flags === "undefined")
  {
    flags = 0;
  }
  return sock.sendto_inet_dgram_socket(socket._fd, socket._write_buffer, size, host, port, flags);
}

var socket_write = function(socket, data)
{
  var size = 0;
  if (buffer.isBuffer(data))
  {
    socket._write_buffer.fromBuffer(data, false);
    size = data.size();
  }
  else if (typeof data === "string")
  {
    socket._write_buffer.fromString(data, false);
    size = data.length;
  }
  else if (typeof data === "array" || typeof data === "object")
  {
    socket._write_buffer.fromArray(data, false);
    size = data.length;
  }
  else
  {
    throw new Error("Invalid data argument.  Must be buffer, array or string");
  }
  return system.write(socket._fd, socket._write_buffer, size);
}

var create_udp_socket_properties = function(socket, read_buf, write_buf)
{
  if (!buffer.isBuffer(read_buf))
  {
    socket._read_buffer = READ_BUFFER;
  }
  else
  {
    socket._read_buffer = read_buf;
  }
  
  if (!buffer.isBuffer(write_buf))
  {
    socket._write_buffer = WRITE_BUFFER;
  }
  else
  {
    socket._write_buffer = write_buf;
  }
  
  socket._on_pollin = function(socket) {}
  socket._on_pollerr = function(socket) {}
  socket._on_pollhup = function(socket) {}
  
 
  socket.on = function(name, func)
  {
    if (name === "read")       socket._on_pollin = func;
    if (name === "error")      socket._on_pollerr = func;
    if (name === "hangup")     socket._on_pollhup = func;
  }
  
  socket._on_poll_event = function(fd, revents)
  {
    if (revents & poll.POLLIN || revents & poll.POLLPRI)
    {
      socket._on_pollin(socket);
    }
    else if (revents & poll.POLLER)
    {
      socket._on_pollerr(socket);
    }
    else if (revents & poll.POLLHUP)
    {
      socket._on_pollhup(socket);
    }
  }
  
  socket.receiveFrom = function(size, flags)
  {
    return receive_from(socket, size, flags);
  }
  
  socket.sendTo = function(host, port, data, flags)
  {
    return send_to(socket, host, port, data, flags);
  }
  
  socket.write = function(data)
  {
    return socket_write(socket, data);
  }
  
  socket.getFd = function(){ return socket._fd };
  
  async.monitorFd(socket._fd, socket._on_poll_event);
}

var UdpSocketServer = function(host, port, flags, read_buf, write_buf)
{
  this._address_family = sock.get_address_family(host);
  if (this._address_family == -1)
  {
    throw new Error("Unable to determine address family");
  }
  
  if (typeof flags === "undefined")
  {
    flags = 0;
  }
  
  this._fd = sock.create_inet_server_socket(host, port, sock.SOCKET_UDP, this._address_family, flags);
  if (this._fd == -1)
  {
    throw new Error("Unable to create new socket");
  }
  create_udp_socket_properties(this, read_buf, write_buf);
}

var UdpSocketClient = function(addressFamily, flags, read_buf, write_buf)
{
  
  this._address_family = addressFamily;

  if (typeof flags === "undefined")
  {
    flags = 0;
  }
  this._fd = sock.create_inet_dgram_socket(addressFamily, flags);
  if (this._fd == -1)
  {
    throw new Error("Unable to create new socket");
  }

  this.connect = function(host, port)
  {
    return sock.connect_inet_dgram_socket(this._fd, host, port);
  }
  create_udp_socket_properties(this, read_buf, write_buf);
}


exports.UdpSocketServer = UdpSocketServer;
exports.UdpSocketClient = UdpSocketClient;