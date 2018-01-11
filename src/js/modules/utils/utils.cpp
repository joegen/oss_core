#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"

JS_METHOD_IMPL(cidrVerify)
{
  js_method_arg_declare_string(ip, 0);
  js_method_arg_declare_string(cidr, 1);
  if (ip.empty() || cidr.empty())
    return JSUndefined();
  return JSBoolean(OSS::socket_address_cidr_verify(ip, cidr));
}

JS_METHOD_IMPL(wildCardCompare)
{
  js_method_arg_declare_string(wild, 0);
  js_method_arg_declare_string(input, 1);
  return JSBoolean(OSS::string_wildcard_compare(wild.c_str(), input));
}

JS_METHOD_IMPL(isIpInRange)
{
  js_method_arg_declare_string(low, 0);
  js_method_arg_declare_string(high, 1);
  js_method_arg_declare_string(strIp, 2);

  if (low.empty() || high.empty() || strIp.empty())
    return JSUndefined();
  
  return JSBoolean(OSS::socket_address_range_verify(low, high, strIp));
}

JS_METHOD_IMPL(md5Hash)
{
  js_method_arg_declare_string(input, 0);
  return JSString(OSS::string_md5_hash(input.c_str()).c_str());
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_method("cidrVerify", cidrVerify);
  js_export_method("wildCardCompare", wildCardCompare);
  js_export_method("isIpInRange", isIpInRange);
  js_export_method("md5Hash", md5Hash);
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSUtils);
