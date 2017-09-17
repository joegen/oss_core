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



#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "OSS/JS/JSUtil.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace JS {



std::string string_from_js_string(v8::Handle<v8::Value> str)
{
  if (!str->IsString())
    return "";
  v8::String::Utf8Value value(str);
  return *value;
}

std::string string_from_js_value(const v8::Handle<v8::Value>& str)
{
  if (!str->IsString())
    return "";
  v8::String::Utf8Value value(str);
  return *value;
}

const char* cstring_from_js_string(const v8::String::Utf8Value& value)
{
  return *value ? *value : "<str conversion failed>";
}

void report_js_exception(v8::TryCatch &try_catch, bool show_line)
{
  using namespace v8;
  
  Handle<Message> message = try_catch.Message();

  v8::String::Utf8Value error(try_catch.Exception());
  if (error.length() > 0)
  {

    std::ostringstream errorMsg;

    /*
     02:50:22.863: [CID=00000000] JS: File undefined:12
    [CID=00000000] JS: Source Line   var notheetoo = nothere.subst(0, 10);
    [CID=00000000] JS:
                              ^
    [CID=00000000] JS:
    {TypeError: Cannot call method 'subst' of undefined
        at RouteProfile.isTellMeRoutable (unknown source)
        at RouteProfile.isRoutable (unknown source)
        at handle_request (unknown source)
    }
    */

    if (show_line && !message.IsEmpty())
    {
      // Print (filename):(line number): (message).
      String::Utf8Value filename(message->GetScriptResourceName());
      const char* filename_string = cstring_from_js_string(filename);
      int linenum = message->GetLineNumber();
      //fprintf(stderr, "%s:%i\n", filename_string, linenum);

      errorMsg << filename_string << ":" << linenum << std::endl;
      // Print line of source code.
      String::Utf8Value sourceline(message->GetSourceLine());
      const char* sourceline_string = cstring_from_js_string(sourceline);

      // HACK HACK HACK
      //
      // FIXME
      //
      // Because of how CommonJS modules work, all scripts are wrapped with a
      // "function (function (exports, __filename, ...) {"
      // to provide script local variables.
      //
      // When reporting errors on the first line of a script, this wrapper
      // function is leaked to the user. This HACK is to remove it. The length
      // of the wrapper is 62. That wrapper is defined in src/node.js
      //
      // If that wrapper is ever changed, then this number also has to be
      // updated. Or - someone could clean this up so that the two peices
      // don't need to be changed.
      //
      // Even better would be to get support into V8 for wrappers that
      // shouldn't be reported to users.
      int offset = linenum == 1 ? 62 : 0;

      //fprintf(stderr, "%s\n", sourceline_string + offset);
      errorMsg << sourceline_string + offset << std::endl;
      // Print wavy underline (GetUnderline is deprecated).
      int start = message->GetStartColumn();
      for (int i = offset; i < start; i++)
      {
        errorMsg << " ";
      }
      int end = message->GetEndColumn();
      for (int i = start; i < end; i++)
      {
        errorMsg << "^";
      }
      errorMsg << std::endl;
    }

    String::Utf8Value trace(try_catch.StackTrace());

    if (trace.length() > 0)
    {
      errorMsg << *trace;
    }
    OSS_LOG_ERROR(*error << std::endl << "{" << std::endl << errorMsg.str() << std::endl << "}");
  }
  else
  {
    OSS_LOG_ERROR("Unknown Exception");
  }
}



v8::Handle<v8::Value> log_info_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << " " << *value1;
    OSS::log_information(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << *value;
    OSS::log_information(msg.str());
  }
  
  return v8::Undefined();
}

v8::Handle<v8::Value> log_debug_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << " " << *value1;
    OSS::log_debug(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << *value;
    OSS::log_debug(msg.str());
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> log_error_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << "JavaScript Error: " << *value1;
    OSS::log_error(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << "JavaScript Error: " << *value;
    OSS::log_error(msg.str());
  }
  return v8::Undefined();
}

// Reads a file into a v8 string.
v8::Handle<v8::String> read_file(const std::string& name) {
  FILE* file = fopen(name.c_str(), "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = fread(&chars[i], 1, size - i, file);
    i += read;
  }
  fclose(file);
  v8::Handle<v8::String> result = v8::String::New(chars, size);
  delete[] chars;
  return result;
}

v8::Handle<v8::String> read_directory(const boost::filesystem::path& directory)
{
  std::string data;

  if (boost::filesystem::exists(directory))
  {
    try
    {
      boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr)
      {
        if (boost::filesystem::is_directory(itr->status()))
        {
          continue;
        }
        else
        {
          boost::filesystem::path currentFile = itr->path();
          std::string fileName = OSS::boost_file_name(currentFile);
          if (boost::filesystem::is_regular(currentFile))
          {
            if (OSS::string_ends_with(fileName, ".js"))
            {
              //OSS_LOG_INFO("Google V8 is loading " << currentFile);
              FILE* file = fopen(OSS::boost_path(currentFile).c_str(), "rb");
              if (file == NULL)
              {
                OSS_LOG_ERROR("Google V8 failed to open file " << currentFile);
                return v8::Handle<v8::String>();
              }

              fseek(file, 0, SEEK_END);
              int size = ftell(file);
              rewind(file);

              char* chars = new char[size + 1];
              chars[size] = '\0';
              for (int i = 0; i < size;) {
                int read = fread(&chars[i], 1, size - i, file);
                i += read;
              }
              fclose(file);
              data += chars;
              //OSS_LOG_INFO("Google V8 " << currentFile << " LOADED");
              delete[] chars;
            }

          }
        }
      }
    }
    catch(std::exception& e)
    {
      OSS_LOG_WARNING("Googe V8 exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_WARNING("Unknown Googe V8 exception.");
    }

  }
  return  v8::String::New(data.c_str(), data.size());
}

void wrap_external_object(v8::TryCatch&  try_catch, 
  v8::Persistent<v8::Context>* pContext, 
  v8::Persistent<v8::ObjectTemplate>* pRequestTemplate,
  v8::Handle<v8::Object>& objectInstance,
  OSS_HANDLE pObject)
{
  // Fetch the template for creating JavaScript request wrappers.
  // It only has to be created once, which we do on demand.
  v8::Handle<v8::ObjectTemplate> templ = *pRequestTemplate;

  // Set up an exception handler before calling the Process function

  
  objectInstance = templ->NewInstance();

  if (objectInstance.IsEmpty())
  {
    report_js_exception(try_catch, true);
    return;
  }

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  v8::Handle<v8::External> request_ptr = v8::External::New(pObject);

  // Store the request pointer in the JavaScript wrapper.
  objectInstance->SetInternalField(0, request_ptr);

}

} } // OS::JS
#endif // ENABLE_FEATURE_V8
