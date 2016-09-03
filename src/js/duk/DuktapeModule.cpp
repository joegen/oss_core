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


#include <dlfcn.h>
#include <fstream>
#include <streambuf>
#include "OSS/JS/DUK/DuktapeContext.h"
#include "OSS/JS/DUK/DuktapeModule.h"


namespace OSS {
namespace JS {
namespace DUK {

 
#define DUK_MOD_INIT_FUNC "duk_mod_init"  
typedef duk_ret_t (*duk_mod_init_func)(duk_context*); 

DuktapeModule::DuktapeModule(DuktapeContext* pContext) :
  _pContext(pContext),
  _isLoaded(false),
  _library(0),
  _mod_init_func(0)
{
}

DuktapeModule::~DuktapeModule()
{
}
  
bool DuktapeModule::loadLibrary(const std::string& path)
{
  assert(!_library);
  assert(!_mod_init_func);
  
  //
  // Load the shared library
  //
  _library = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
  if (!_library)
  {
    return false;
  }
  
  //
  // Get the mod_init function pointer
  //
  _mod_init_func = dlsym(_library, DUK_MOD_INIT_FUNC);
  if (!_mod_init_func)
  {
    unload();
    return false;
  }
  
  // duk_c_function
  duk_push_c_function(&_pContext->context(), *((duk_c_function*)_mod_init_func), 1 /* Number of arguments */);
  duk_call(&_pContext->context(), 0);
  //
  // the top of the stack has the exports object
  //
  duk_put_prop_string(&_pContext->context(), 2 /*idx of 'module'*/, "exports");
  
  _isLoaded = true;
  return _isLoaded;
}

bool DuktapeModule::loadJS(const std::string& path)
{
  std::string body;
  std::ifstream js(path.c_str());
  js.seekg(0, std::ios::end);   
  body.reserve(js.tellg());
  js.seekg(0, std::ios::beg);
  body.assign((std::istreambuf_iterator<char>(js)), std::istreambuf_iterator<char>());
  
  if (body.empty())
  {
    return false;
  }
  
  return loadCode(body);
}


bool DuktapeModule::loadCode(const std::string& code)
{
  if (_library)
  {
    return false;
  }
  
  std::string code_final;
  if (code.at(0) == '#' && code.at(1) == 'i')
  {
    //
    // We got a shebang.  Comment it out.
    //
    code_final = std::string("//") + code;
  }
  else
  {
    code_final = code;
  }
  _isLoaded = !!duk_push_lstring(&_pContext->context(), code_final.c_str(), code_final.length());
  return _isLoaded;
}

void DuktapeModule::unload()
{
  if (_library)
  {
    dlclose(_library);
    _library = 0;
    _mod_init_func = 0;
  }
  _isLoaded = false;
}
  

} } } //OSS::JS::DUK  
  


