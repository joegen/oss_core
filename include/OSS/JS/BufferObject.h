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

#ifndef OSS_BUFFEROBJECT_H_INCLUDED
#define OSS_BUFFEROBJECT_H_INCLUDED

#include <v8.h>
#include <vector>
#include <OSS/JS/ObjectWrap.h>

class BufferObject : public OSS::JS::ObjectWrap
{
public:
  typedef std::vector<unsigned char> ByteArray;
  static v8::Persistent<v8::Function> _constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  
  //
  // Methods
  //
  static v8::Handle<v8::Value> size(const v8::Arguments& args);
  static v8::Handle<v8::Value> toArray(const v8::Arguments& args);
  static v8::Handle<v8::Value> toString(const v8::Arguments& args);
  static v8::Handle<v8::Value> fromArray(const v8::Arguments& args);
  static v8::Handle<v8::Value> fromString(const v8::Arguments& args);
  static v8::Handle<v8::Value> fromBuffer(const v8::Arguments& args);
  static v8::Handle<v8::Value> equals(const v8::Arguments& args);
  
  //
  // Properties
  //
  static v8::Handle<v8::Value> getAt(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> setAt(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  
  
  //
  // Helpers
  //
  static v8::Handle<v8::Value> createNew(uint32_t size);
  static v8::Persistent<v8::Function> createNewFunc;
  static bool isBuffer(v8::Local<v8::Value> value);
  static v8::Handle<v8::Value>  isBufferObject(const v8::Arguments& args);
  
  ByteArray& buffer();
private:
  BufferObject();
  BufferObject(std::size_t size);
  BufferObject(const BufferObject& obj);
  virtual ~BufferObject();
  ByteArray _buffer;
};

//
// Inlines
//
inline BufferObject::ByteArray& BufferObject::buffer()
{
  return _buffer;
}


#endif // OSS_BUFFEROBJECT_H_INCLUDED

