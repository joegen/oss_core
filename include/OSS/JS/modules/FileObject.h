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

#ifndef OSS_FILEOBJECT_H_INCLUDED
#define OSS_FILEOBJECT_H_INCLUDED


#include <stdio.h>
#include "OSS/JS/JSPlugin.h"

class FileObject : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(_fopen);
  JS_METHOD_DECLARE(_fclose);
  JS_METHOD_DECLARE(_fseek);
  JS_METHOD_DECLARE(_rewind);
  JS_METHOD_DECLARE(_fflush);
  JS_METHOD_DECLARE(_feof);
  JS_METHOD_DECLARE(_ferror);
  JS_METHOD_DECLARE(_fread);
  JS_METHOD_DECLARE(_fwrite);
  JS_METHOD_DECLARE(_fgets);
  JS_METHOD_DECLARE(_fileno);
  JS_METHOD_DECLARE(_fmemopen);
  JS_METHOD_DECLARE(_fputc);
  JS_METHOD_DECLARE(_fputs);
  JS_METHOD_DECLARE(_ftell);
  JS_METHOD_DECLARE(_flock);
  
private:
  FileObject();
  virtual ~FileObject();
  FILE* _pFile;
};
#endif // OSS_FILEOBJECT_H_INCLUDED

