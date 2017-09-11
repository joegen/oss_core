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
  var modulePath = __get_module_cononical_file_name(path);
  if (typeof modulePath === "undefined")
  {
    return;
  }
  var cached = __get_cached_module(modulePath);
  if (typeof cached !== "undefined")
  {
    return cached;
  }
  var module = new Module();
  module.path = modulePath;
  var script = __get_module_script(module.path);
  if (typeof script === "undefined")
  {
    return;
  }
  __compile_module(script, module.path)(module, module.exports);
  __cache_module(module.path, module.exports);
  return module.exports;
}