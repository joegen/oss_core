function Module()
{
  this.path = "";
  this.exports = {};
}

var __registered_modules = {};

function __get_cached_module(name)
{
  if (__registered_modules.hasOwnProperty(name))
  {
    return __registered_modules[name];
  }
}

function __cache_module(name, module)
{
  __registered_modules[name] = module;
}

function require(path)
{
  __lock_isolate();
  var modulePath = __get_module_cononical_file_name(path);
  if (typeof modulePath === "undefined")
  {
    __unlock_isolate();
    throw new ReferenceError("Unable to resolve canonical file name for module " + modulePath);
  }
  var cached = __get_cached_module(modulePath);
  if (typeof cached !== "undefined")
  {
    __unlock_isolate();
    return cached;
  }
  
  var module = new Module();
  module.path = modulePath;
  module.exports = {};
  
  if (module.path.search(".jso") != module.path.length - 4)
  {
    var script = __get_module_script(module.path);
    if (typeof script === "undefined")
    {
      __unlock_isolate();
      throw new ReferenceError("Unable to load javascript exports for " + modulePath);
    }
    
    var current_path = __current_path();
    var parent_path = __parent_path(module.path);
    __chdir(parent_path);
    try {
      var compiled = __compile_module(script, module.path);
      if (typeof compiled === "function")
      {
        compiled(module, module.exports)
      }
      else
      {
        module.exports = undefined;
      }
    } catch (e) {
      module.exports = undefined;
    }
    
    __chdir(current_path);
  }
  else
  {
    var exportFunc = __load_plugin(module.path);
    if (typeof exportFunc === "function")
    {
      module.exports = exportFunc();
    }
    else
    {
      __unlock_isolate();
      throw new ReferenceError("Unable to load plugin exports for " + modulePath);
    }
  }
  
  if (typeof module.exports !== "undefined")
  {
    __cache_module(module.path, module.exports);
  }
  
  __unlock_isolate();
  
  return module.exports;
}

//
// Initialize global plugins
//
function __copy_exports (src, dst) 
{
  for (var key in src) 
  {
    dst[key] = src[key]
  }
}

var __create_buffer_object = function(arg)
{
  return new Buffer(arg);
}
require("buffer");

//
// Clean up exports
//
function __cleanup_modules()
{
  for(var obj in __registered_modules)
  {
    var mod = __registered_modules[obj];
    if (mod.hasOwnProperty("__cleanup_exports") && typeof mod.__cleanup_exports === "function")
    {
      mod.__cleanup_exports();
    }
  }
  require("async").__stop_event_loop();
}

//
// format the stack trace
//
Error.prepareStackTrace = function(error, stack) {
    var trace = '';
    var max_width = 0;
    for (var i = 0; i < stack.length; i++){
        var frame = stack[i];

        var typeLength = 0;
        typeLength = (frame.getTypeName() !== null && frame.getTypeName() !== '[object global]') ? frame.getTypeName().length : 0;
        typeLength = typeLength.length > 50 ? 50 : typeLength;

        functionlength = frame.getFunctionName() !== null ? frame.getFunctionName().length : '<anonymous>'.length;
        functionlength = functionlength > 50 ? 50 : functionlength;

        if (typeLength + functionlength > max_width)
            max_width = typeLength + functionlength;
    }

    for (var i = 0; i < stack.length; i++) {
        var frame = stack[i];

        var filepath = frame.getFileName();

        var typeName = '';  
        if (frame.getTypeName() !== null && frame.getTypeName() !== '[object global]')
            typeName = frame.getTypeName().substring(0, 50) + '.';

        var functionName = '<anonymous>';
        if (frame.getFunctionName() !== null)
            functionName = frame.getFunctionName().substring(0, 50);

        var space = '';
        var width = max_width - (typeName.length + functionName.length) + 2;
        space = Array(width).join(' ');
        var line = '  at ' + typeName + functionName + ' (' + filepath + 
            ':' + frame.getLineNumber() + 
            ':' + frame.getColumnNumber() + '\n';

        trace += line;
    }
    return trace;
};

//
// Built in modules
//
const console = require("console");
const logger = require("logger");
const system = require("system");
const async = require("async");
const assert = require("assert");
const utils = require("utils");
const utils = require("timer");
const ossjs = async;


//
// Global methods
//
const exit = system.exit;
const _exit = system._exit;

const log_notice = logger.log_notice;
const log_info = logger.log_info;
const log_debug = logger.log_debug;
const log_trace = logger.log_trace;
const log_warning = logger.log_warning;
const log_error = logger.log_error;
const log_critical = logger.log_critical;
const log_fatal = logger.log_fatal;

//
// Overrides
//
Error.prototype.printStackTrace = function()
{
  console.error(this + "\n" + this.stack);
}

