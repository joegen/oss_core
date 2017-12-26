
"use-strict"
const inotify = require("./_inotify.jso");
const MonitoredFile = require("./monitored_file.js").MonitoredFile;
exports.MonitoredFile = MonitoredFile;
__copy_exports(inotify, exports);