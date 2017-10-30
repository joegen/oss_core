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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <fcntl.h>
#include <unistd.h>

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  
  //
  // Mutable Properties
  //
  
  //
  // Standard fcntl system constants
  //
#ifdef __USE_XOPEN2K8
  CONST_EXPORT(O_DIRECTORY);    /* Must be a directory.	 */
  CONST_EXPORT(O_NOFOLLOW);     /* Do not follow links.	 */
  CONST_EXPORT(O_CLOEXEC);      /* Set close_on_exec.  */
#endif

#ifdef __USE_GNU
  CONST_EXPORT(O_DIRECT);       /* Direct disk access.	*/
  CONST_EXPORT(O_NOATIME);      /* Do not set atime.  */
  CONST_EXPORT(O_PATH);         /* Resolve pathname but do not open file.  */
#ifdef O_TMPFILE
  CONST_EXPORT(O_TMPFILE);      /* Atomically create nameless file.  */
#endif
#endif
  
  CONST_EXPORT(STDOUT_FILENO);
  CONST_EXPORT(STDIN_FILENO);
  CONST_EXPORT(STDERR_FILENO);
  
  return exports;
}

JS_REGISTER_MODULE(Const);
