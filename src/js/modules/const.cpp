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
  CONST_EXPORT(O_TMPFILE);      /* Atomically create nameless file.  */
#endif
  
  CONST_EXPORT(STDOUT_FILENO);
  CONST_EXPORT(STDIN_FILENO);
  CONST_EXPORT(STDERR_FILENO);
  
  return exports;
}

JS_REGISTER_MODULE(Const);
