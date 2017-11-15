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
  JS_METHOD_DECLARE(resize);
  
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

typedef std::vector<unsigned char> ByteArray;
inline bool js_byte_array_to_int_array(ByteArray& input, v8::Handle<v8::Array>& output, std::size_t sz)
{
  uint32_t i = 0;
  for (ByteArray::iterator iter = input.begin(); iter != input.end(); iter++)
  {
    output->Set(i++, v8::Int32::New(*iter));
    if (sz && i >= sz)
    {
      break;
    }
  }
  return output->Length() > 0;
}

inline  bool js_int_array_to_byte_array(v8::Handle<v8::Array>& input, ByteArray& output, bool resize = false)
{
  if (resize)
  {
    output.clear();
    output.reserve(input->Length());
  }
  else
  {
    std::fill(output.begin(), output.end(), 0);
  }
  for(uint32_t i = 0; i < input->Length(); i++)
  {
    uint32_t val = input->Get(i)->ToInt32()->Value();
    if (val >= 256)
    {
      return false;
    }
    if (resize)
    {
      output.push_back(val);
    }
    else
    {
      output[i] = val;
    }
  }
  return !output.empty();
}

inline bool js_string_to_byte_array(std::string& input, ByteArray& output, bool resize)
{
  if (resize)
  {
    output.clear();
    output.reserve(input.size());
  }
  else
  {
    std::fill(output.begin(), output.end(), 0);
  }
  
  for(uint32_t i = 0; i < input.size(); i++)
  {
    if (resize)
    {
      output.push_back((uint32_t)input.at(i));
    }
    else
    {
      output[i] = (uint32_t)input.at(i);
    }
  }
  return !output.empty();
}


#endif // OSS_BUFFEROBJECT_H_INCLUDED

