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
#include <OSS/JS/JSPlugin.h>

class BufferObject : public OSS::JS::ObjectWrap
{
public:
  typedef std::vector<unsigned char> ByteArray;
  
  
  JS_CONSTRUCTOR_DECLARE();

  //
  // Methods
  //
  JS_METHOD_DECLARE(size);
  JS_METHOD_DECLARE(toArray);
  JS_METHOD_DECLARE(toString);
  JS_METHOD_DECLARE(fromArray);
  JS_METHOD_DECLARE(fromString);
  JS_METHOD_DECLARE(fromBuffer);
  JS_METHOD_DECLARE(equals);
  
  //
  // Properties
  //
  JS_INDEX_GETTER_DECLARE(getAt);
  JS_INDEX_SETTER_DECLARE(setAt);
  
  
  //
  // Helpers
  //
  static JSValueHandle createNew(uint32_t size);
  static JSPersistentFunctionHandle createNewFunc;
  static bool isBuffer(JSValueHandle value);
  static JSValueHandle  isBufferObject(JSArguments& args);
  
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

