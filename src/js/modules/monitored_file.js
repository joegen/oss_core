"use-strict"

const system = require("system");
const inotify = require("inotify");
const async = require("async");

var MonitoredFile = function(path)
{
  var monitored_file = this;
  var inotify_fd = this._fd = inotify.inotify_init();
  this.path = path;
  
  this._on_access = function(file){};
  this._on_attribute = function(file){};
  this._on_close_nowrite = function(file){};
  this._on_close_write = function(file){};
  this._on_create = function(file){};
  this._on_delete = function(file){};
  this._on_delete_self = function(file){};
  this._on_modify = function(file){};
  this._on_move_self = function(file){};
  this._on_move_from = function(file){};
  this._on_move_to = function(file){};
  this._on_open = function(file){};
  this._on_unmount = function(file){};
  
  this.on = function(name, func)
  {
    if (name === "access")            monitored_file._on_access = func;
    if (name === "attribute")         monitored_file._on_attribute = func;
    if (name === "close_nowrite")     monitored_file._on_close_nowrite = func;
    if (name === "close_write")       monitored_file._on_close_write = func;
    if (name === "create")            monitored_file._on_create = func;
    if (name === "delete")            monitored_file._on_delete = func;
    if (name === "delete_self")       monitored_file._on_delete_self = func;
    if (name === "modify")            monitored_file._on_modify = func;
    if (name === "move_self")         monitored_file._on_move_self = func;
    if (name === "move_from")         monitored_file._on_move_from = func;
    if (name === "move_to")           monitored_file._on_move_to = func;
    if (name === "open")              monitored_file._on_open = func;
    if (name === "unmount")           monitored_file._on_unmount = func;
  }
  
  this._on_inotify_readable = function(fd)
  {
    var revents = inotify.inotify_get_events(inotify_fd);
    if (revents & inotify.IN_ACCESS)        monitored_file._on_access(monitored_file);
    if (revents & inotify.IN_ATTRIB)        monitored_file._on_attribute(monitored_file);
    if (revents & inotify.IN_CLOSE_NOWRITE) monitored_file._on_close_nowrite(monitored_file);
    if (revents & inotify.IN_CLOSE_WRITE)   monitored_file._on_close_write(monitored_file);
    if (revents & inotify.IN_CREATE)        monitored_file._on_create(monitored_file);
    if (revents & inotify.IN_DELETE)        monitored_file._on_delete(monitored_file);
    if (revents & inotify.IN_DELETE_SELF)   monitored_file._on_delete_self(monitored_file);
    if (revents & inotify.IN_MODIFY)        monitored_file._on_modify(monitored_file);
    if (revents & inotify.IN_MOVE_SELF)     monitored_file._on_move_self(monitored_file);
    if (revents & inotify.IN_MOVED_FROM)    monitored_file._on_move_from(monitored_file);
    if (revents & inotify.IN_MOVED_TO)      monitored_file._on_move_to(monitored_file);
    if (revents & inotify.IN_OPEN)          monitored_file._on_open(monitored_file);
    if (revents & inotify.IN_UNMOUNT)       monitored_file._on_unmount(monitored_file);
  }
  
  async.monitorFd(this._fd, this._on_inotify_readable);
  this._wd = inotify.inotify_add_watch(this._fd, this.path, inotify.IN_ALL_EVENTS);
}

MonitoredFile.prototype.unmonitor = function()
{
  inotify.inotify_rm_watch(this._fd, this._wd);
  system.close(this._fd);
}

exports.MonitoredFile = MonitoredFile;

