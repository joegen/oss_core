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


#ifndef OSS_DUKTAPEMODULE_H_INCLUDED
#define OSS_DUKTAPEMODULE_H_INCLUDED


#include "OSS/OSS.h"

#include "OSS/UTL/Thread.h"
#include <boost/noncopyable.hpp>


namespace OSS {
namespace JS {
namespace DUK {


class DuktapeContext;

class DuktapeModule : boost::noncopyable
{
public: 
  typedef duk_ret_t (*duk_mod_init_func)(duk_context*); 
  
  DuktapeModule(DuktapeContext* pContext);
  ~DuktapeModule();
  
  bool loadLibrary(const std::string& path);
  bool loadJS(const std::string& path);
  bool loadCode(const std::string& coce);
  void callModuleInit(duk_mod_init_func initFunc);
  void unload();
  
  const std::string& getPath() const;
  bool isLoaded() const;
  
protected:
  DuktapeContext* _pContext;
  std::string _path;
  bool _isLoaded;
  void* _library;
  duk_mod_init_func _mod_init_func;
};
  

//
// Inlines
//

inline const std::string& DuktapeModule::getPath() const
{
  return _path;
}

inline bool DuktapeModule::isLoaded() const
{
  return _isLoaded;
}


} } } //OSS::JS::DUK  
  

#endif // OSS_DUKTAPEMODULE_H_INCLUDED

