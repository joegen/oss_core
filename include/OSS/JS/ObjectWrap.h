// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_NODE_OBJECT_WRAP_H_
#define SRC_NODE_OBJECT_WRAP_H_

#include <v8.h>
#include "OSS/JS/JSIsolate.h"
#include <assert.h>


namespace OSS {
namespace JS {

class ObjectWrap {
 public:
  ObjectWrap ( ) 
  {
    refs_ = 0;
    _pIsolate = JSIsolate::getIsolate();
  }


  virtual ~ObjectWrap ( ) 
  {
    if (!handle_.IsEmpty()) 
    {
      assert(handle_.IsNearDeath());
      handle_.ClearWeak();
      handle_->SetInternalField(0, v8::Undefined());
      handle_.Dispose();
      handle_.Clear();
    }
  }


  template <class T>
  static inline T* Unwrap (v8::Handle<v8::Object> handle) 
  {
    if(handle.IsEmpty() || handle->InternalFieldCount() <= 0)
    {
      return 0;
    }
    return static_cast<T*>(handle->GetPointerFromInternalField(0));
  }
  
  template <class T>
  static v8::Local<v8::FunctionTemplate> ExportConstructorTemplate(const char * className, v8::Handle<v8::Object>& exports)
  {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(T::New);
    tpl->SetClassName(v8::String::NewSymbol(className));
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("ObjectType"), v8::String::NewSymbol(className));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    return tpl;
  }
  
  template <class T>
  static void FinalizeConstructorTemplate(const char * className, v8::Local<v8::FunctionTemplate>& tpl, v8::Handle<v8::Object>& exports)
  {
    T::_constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol(className), T::_constructor);
  }
  
  template <class T>
  static void ExportMethod(v8::Local<v8::FunctionTemplate>& tpl, const char* method,  v8::InvocationCallback callback)
  {
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol(method), v8::FunctionTemplate::New(callback)->GetFunction());
  }
  
  template <class T>
  static void ExportIndexHandler(v8::Local<v8::FunctionTemplate>& tpl, v8::IndexedPropertyGetter getter,  v8::IndexedPropertySetter setter = 0)
  {
    tpl->PrototypeTemplate()->SetIndexedPropertyHandler(getter, setter);
  }
 
  const JSIsolate::Ptr& getIsolate()
  {
    return _pIsolate;
  }

  v8::Persistent<v8::Object> handle_; // ro

 protected:
  inline void Wrap (v8::Handle<v8::Object> handle) 
  {
    assert(handle_.IsEmpty());
    assert(handle->InternalFieldCount() > 0);
    handle_ = v8::Persistent<v8::Object>::New(handle);
    handle_->SetPointerInInternalField(0, this);
    MakeWeak();
  }


  inline void MakeWeak (void) 
  {
    handle_.MakeWeak(this, WeakCallback);
    handle_.MarkIndependent();
  }

  /* Ref() marks the object as being attached to an event loop.
   * Refed objects will not be garbage collected, even if
   * all references are lost.
   */
  virtual void Ref() 
  {
    assert(!handle_.IsEmpty());
    refs_++;
    handle_.ClearWeak();
  }

  /* Unref() marks an object as detached from the event loop.  This is its
   * default state.  When an object with a "weak" reference changes from
   * attached to detached state it will be freed. Be careful not to access
   * the object after making this call as it might be gone!
   * (A "weak reference" means an object that only has a
   * persistant handle.)
   *
   * DO NOT CALL THIS FROM DESTRUCTOR
   */
  virtual void Unref() 
  {
    assert(!handle_.IsEmpty());
    assert(!handle_.IsWeak());
    assert(refs_ > 0);
    if (--refs_ == 0) 
    { 
      MakeWeak(); 
    }
  }


  int refs_; // ro
  JSIsolate::Ptr _pIsolate;

 private:
  static void WeakCallback (v8::Persistent<v8::Value> value, void *data) 
  {
    ObjectWrap *obj = static_cast<ObjectWrap*>(data);
    assert(value == obj->handle_);
    assert(!obj->refs_);
    assert(value.IsNearDeath());
    delete obj;
  }
};

} } // OSS::JS

#endif  // SRC_NODE_OBJECT_WRAP_H_
