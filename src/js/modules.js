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
  var modulePath = __get_module_cononical_file_name(path);
  if (typeof modulePath === "undefined")
  {
    throw new ReferenceError("Unable to resolve canonical file name for module " + modulePath);
  }
  var cached = __get_cached_module(modulePath);
  if (typeof cached !== "undefined")
  {
    return cached;
  }
  
  var module = new Module();
  module.path = modulePath;
  
  if (module.path.search(".jso") != module.path.length - 4)
  {
    var script = __get_module_script(module.path);
    if (typeof script === "undefined")
    {
      throw new ReferenceError("Unable to load javascript exports for " + modulePath);
    }
    
    var current_path = __current_path();
    var parent_path = __parent_path(module.path);
    __chdir(parent_path);
    __compile_module(script, module.path)(module, module.exports);
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
      throw new ReferenceError("Unable to load plugin exports for " + modulePath);
    }
  }
  
  if (typeof module.exports !== "undefined")
  {
    __cache_module(module.path, module.exports);
  }
  
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


