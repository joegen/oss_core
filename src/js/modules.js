function Module()
{
}

Module.prototype.exports = {};
Module.prototype.path = {}

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
  var cached = __get_cached_module(path);
  if (typeof cached !== "undefined")
  {
    return cached;
  }
  var module = new Module();
  module.path = path;
  var script = __get_module_script(module.path);
  __compile_module(script, module.path)(module);
  __cache_module(module.path, module.exports);
  return module.exports;
}