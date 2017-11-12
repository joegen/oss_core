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
#include <boost/filesystem.hpp>

JS_METHOD_IMPL(__exists)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string filename = js_method_arg_as_std_string(0);
  boost::filesystem::path file(filename.c_str());
  return JSBoolean(boost::filesystem::exists(file));
}

JS_METHOD_IMPL(__remove)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string filename = js_method_arg_as_std_string(0);
  boost::system::error_code ec;
  boost::filesystem::path file(filename.c_str());
  return JSBoolean(boost::filesystem::remove(file, ec));
}

JS_METHOD_IMPL(__remove_all)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string filename = js_method_arg_as_std_string(0);
  boost::system::error_code ec;
  boost::filesystem::path file(filename.c_str());
  return JSUInt32(boost::filesystem::remove_all(file, ec));
}

JS_EXPORTS_INIT()
{
  js_export_method(("exists"), __exists);
  js_export_method(("remove"), __remove);
  js_export_method(("remove_all"), __remove_all);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSFileSystem);
