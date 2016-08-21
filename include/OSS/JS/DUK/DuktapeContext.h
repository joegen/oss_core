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


#include "OSS/OSS.h"
#include "OSS/JS/DUK/duktape.h"
#include "OSS/UTL/Thread.h"
#include <boost/noncopyable.hpp>

namespace OSS {
namespace JS {
namespace DUK {


class DuktapeContext : public boost::noncopyable
{
public:
  DuktapeContext(const std::string& name);
  ~DuktapeContext();

  const std::string& getName() const;
  
  duk_context& context();
  const duk_context& context() const;
private:  
  std::string _name;
  duk_context* _pContext;
  static OSS::mutex_critic_sec _duk_mutex;
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

} } } // OSS::JS::DUK

#endif	// OSS_DUKTAPECONTEXT_H_INCLUDED

