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


#include "OSS/JS/JSObjectWrap.h"
#include "OSS/JS/JSIsolate.h"


namespace OSS {
namespace JS {


JSObjectWrap::JSObjectWrap ( ) 
{
  refs_ = 0;
  JSIsolate::Ptr ptr = JSIsolate::getIsolate();
  _pIsolatePtr = new JSIsolate::Ptr();
  *((JSIsolate::Ptr*)_pIsolatePtr) = ptr;
  _pIsolate = ptr.get();
}

JSObjectWrap::~JSObjectWrap ( ) 
{
  delete (JSIsolate::Ptr*)_pIsolatePtr;
  
  if (!handle_.IsEmpty()) 
  {
    assert(handle_.IsNearDeath());
    handle_.ClearWeak();
    handle_->SetInternalField(0, v8::Undefined());
    handle_.Dispose();
    handle_.Clear();
  }
}

void JSObjectWrap::Wrap (v8::Handle<v8::Object> handle) 
{
  assert(handle_.IsEmpty());
  assert(handle->InternalFieldCount() > 0);
  handle_ = v8::Persistent<v8::Object>::New(handle);
  handle_->SetPointerInInternalField(0, this);
  MakeWeak();
}


void JSObjectWrap::MakeWeak (void) 
{
  handle_.MakeWeak(this, WeakCallback);
  handle_.MarkIndependent();
}

/* Ref() marks the object as being attached to an event loop.
 * Refed objects will not be garbage collected, even if
 * all references are lost.
 */
void JSObjectWrap::Ref() 
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
void JSObjectWrap::Unref() 
{
  assert(!handle_.IsEmpty());
  assert(!handle_.IsWeak());
  assert(refs_ > 0);
  if (--refs_ == 0) 
  { 
    MakeWeak(); 
  }
}


void JSObjectWrap::WeakCallback (v8::Persistent<v8::Value> value, void *data) 
{
  JSObjectWrap *obj = static_cast<JSObjectWrap*>(data);
  assert(value == obj->handle_);
  assert(!obj->refs_);
  assert(value.IsNearDeath());
  delete obj;
}


} } // OSS::JS

