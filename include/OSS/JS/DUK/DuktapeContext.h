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


#ifndef OSS_DUKTAPECONTEXT_H_INCLUDED
#define	OSS_DUKTAPECONTEXT_H_INCLUDED


#include <map>
#include <vector>
#include <boost/filesystem.hpp>

#include "OSS/OSS.h"
#include "OSS/JS/DUK/duktape.h"
#include "OSS/JS/DUK/duk_module_node.h"
#include "OSS/UTL/Thread.h"
#include "OSS/JS/DUK/DuktapeModule.h"


namespace OSS {
namespace JS {
namespace DUK {


class DuktapeContext : public boost::noncopyable
{
public:
  typedef DuktapeModule::duk_mod_init_func ModuleInit;
  typedef std::map<intptr_t, DuktapeContext*> ContextMap;
  typedef std::map<std::string, DuktapeModule*> ModuleMap;
  typedef std::vector<boost::filesystem::path> ModuleDirectories;
  typedef std::map<std::string, ModuleInit> InternalModules;
  
  DuktapeContext(const std::string& name);
  ~DuktapeContext();
  
  const std::string& getName() const;
  duk_context& context();
  const duk_context& context() const;
  
  void initCommonJS();
  bool resolveModule(const std::string& parentId, const std::string& moduleId, std::string& resolvedResults);
  int loadModule(const std::string& moduleId);
  bool evalFile(const std::string& file, FILE* foutput, FILE* ferror);
  DuktapeModule* getModule(const std::string& moduleId);
  void createInternalModule(const std::string& moduleId, ModuleInit initFunc);
  
private:  
  std::string _name;
  duk_context* _pContext;
  
public:
  static DuktapeContext* rootInstance();
  static DuktapeContext* getContext(duk_context* ctx);
  static void createInternalModule(DuktapeContext* pContext, const std::string& moduleId, ModuleInit initFunc);
  static DuktapeModule* getModule(DuktapeContext* pContext, const std::string& moduleId);
  static void deleteModule(const std::string& moduleId);
  static bool addModuleDirectory(const std::string& path);
  static bool resolvePath(const std::string& file, std::string& absolutePath);
  static OSS::mutex_critic_sec _duk_mutex;
  static ModuleMap _moduleMap;
  static ModuleDirectories _moduleDirectories;
  static InternalModules _internalModules;
  
};


//
// Inlines
//

inline const std::string& DuktapeContext::getName() const
{
  return _name;
}

inline duk_context& DuktapeContext::context()
{
  return *_pContext;
}
inline const duk_context& DuktapeContext::context() const
{
  return *_pContext;
}

inline DuktapeModule* DuktapeContext::getModule(const std::string& moduleId)
{
  return DuktapeContext::getModule(this, moduleId);
}

inline void DuktapeContext::createInternalModule(const std::string& moduleId, DuktapeModule::duk_mod_init_func initFunc)
{
  DuktapeContext::createInternalModule(this, moduleId, initFunc);
}

} } } // OSS::JS::DUK

#endif	// OSS_DUKTAPECONTEXT_H_INCLUDED

