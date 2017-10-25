/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BufferObject.h
 * Author: joegen
 *
 * Created on October 25, 2017, 1:14 PM
 */

#ifndef OSS_BUFFEROBJECT_H_INCLUDED
#define OSS_BUFFEROBJECT_H_INCLUDED

#include <v8.h>
#include <vector>

namespace OSS {
namespace JS {  

class BufferObject : public ObjectWrap
{
public:
  typedef std::vector<unsigned char> ByteArray;
  static v8::Persistent<v8::Function> _constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Create(const v8::Arguments& args);
  
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

private:
  BufferObject();
  BufferObject(std::size_t size);
  BufferObject(const BufferObject& obj);
  ~BufferObject();
  ByteArray _buffer;
};


} } // OSS::JS

#endif // OSS_BUFFEROBJECT_H_INCLUDED

