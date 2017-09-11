// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef JSBase_H_INCLUDED
#define JSBase_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "OSS/OSS.h"
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/tuple/tuple.hpp>

#include "OSS/JS/JS.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/BlockingQueue.h"

#include "v8.h"

namespace OSS {
namespace JS {


OSS_CREATE_INLINE_EXCEPTION(JSBaseException, OSS::IOException, "Javascript SIPMessage Processor Exception");

typedef v8::Handle<v8::Value> jsval;
typedef v8::Arguments jsargs;
typedef v8::String jsstring;
typedef v8::String::Utf8Value jsstringutf8;
typedef v8::FunctionTemplate jsfunc;
#define jsvoid v8::Undefined
typedef v8::Boolean jsbool;
typedef v8::Integer jsint;
typedef v8::HandleScope jsscope;
typedef v8::Handle<v8::External> jsfield;

std::string jsvalToString(const jsval& str);

class OSS_API JSBase : boost::noncopyable
{
public:
  struct Module
  {
    std::string name;
    std::string script;
  };
  
  typedef std::map<std::string, Module> InternalModules;
  typedef std::vector<Module> ModuleHelpers;
  
  JSBase(const std::string& contextName);
    /// Create a new JSBase

  virtual ~JSBase();
    /// Destroy the JSBase

  bool run(const boost::filesystem::path& script);
  
  bool initialize(const boost::filesystem::path& script,
    const std::string& functionName,
    void(*extensionGlobals)(OSS_HANDLE) = 0,
    const std::string& preloaded = std::string());
    /// Initialize the javascript context and the object template.
    /// The function indicated by funtionName must exist in the script

  bool recompile();
    /// Recompile the current active java script.

  bool processRequest(OSS_HANDLE request);
    /// Process the request

  bool isInitialized() const;
    /// Returns true if the script has been initialized

  virtual void initGlobalFuncs(OSS_HANDLE objectTemplate);
    /// Initialize global functions that will be exposed to the java script engine

  const boost::filesystem::path& getScriptFilePath() const;
    /// return the path of the script file

  const std::string& getContextName() const;
    /// returns the name of the context

  void setGlobalScriptsDirectory(const std::string& globalScriptsDirectory);
    /// Set the directory for global exports.  If not set, it will default to global.detail

  void setHelperScriptsDirectory(const std::string& helperScriptsDirectory);
    /// Set the helper scripts export. If not set, it will default to {scriptname}.detail
  
  bool callFunction(const std::string& funcName);
    /// Call a JS funciton with zero arguments
  
  static void addGlobalScript(const std::string& script);
    /// Prepend JS code the the compiled script

  static void registerInternalModule(const Module& module);

  static void registerModuleHelper(const Module& module);
  
  static bool compileModuleHelpers();
  
  static bool initModules();
protected:
  bool internalInitialize(const boost::filesystem::path& script,
    const std::string& functionName,
    void(*extensionGlobals)(OSS_HANDLE),
    const std::string& preloaded = std::string());
    /// Initialize the javascript context and the object template.
    /// The function indicated by funtionName must exist in the scri

  bool internalProcessRequest(OSS_HANDLE request);
    /// Process the request

  bool internalRecompile();
    /// Recompile the script

  std::string _contextName;
  boost::filesystem::path _script;
  std::string _globalScriptsDirectory;
  std::string _helperScriptsDirectory;
  OSS_HANDLE _context;
  OSS_HANDLE _processFunc;
  OSS_HANDLE _requestTemplate;
  OSS_HANDLE _globalTemplate;
  bool _isInitialized;
  static OSS::mutex _mutex;
  std::string _functionName;
  void(*_extensionGlobals)(OSS_HANDLE);
  std::string _preloaded;
  friend class JSWorker;
  
public:
  static InternalModules _modules;
  static ModuleHelpers _moduleHelpers;
  static std::vector<std::string> _globalScripts;
};




//
// Inlines
//

inline const boost::filesystem::path& JSBase::getScriptFilePath() const
{
  return _script;
}

inline bool JSBase::isInitialized() const
{
  return _isInitialized;
}

inline const std::string& JSBase::getContextName() const
{
  return _contextName;
}

inline void JSBase::setGlobalScriptsDirectory(const std::string& globalScriptsDirectory)
{
  _globalScriptsDirectory = globalScriptsDirectory;
}

inline void JSBase::setHelperScriptsDirectory(const std::string& helperScriptsDirectory)
{
  _helperScriptsDirectory = helperScriptsDirectory;
}

inline void JSBase::initGlobalFuncs(OSS_HANDLE objectTemplate) 
{
}

inline bool JSBase::run(const boost::filesystem::path& script)
{
  return initialize(script, "", 0, "");
}

} } // OSS::JS


#endif // ENABLE_FEATURE_V8

#endif // JSBase_H_INCLUDED


