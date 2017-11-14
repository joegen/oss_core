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
#include "OSS/JS/modules/inet_socket/inet_socket.h"
#include "OSS/JS/modules/BufferObject.h"


#define HOST_BUF_SIZE 512
static char HOST_BUF[HOST_BUF_SIZE];

#define SERVICE_BUF_SIZE 256
static char SERVICE_BUF[SERVICE_BUF_SIZE];


JS_METHOD_IMPL(__create_inet_stream_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(4);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_uint32(2);
  js_method_arg_assert_int32(3);
  
  std::string host = js_method_arg_as_std_string(0);
  std::string service = js_method_arg_as_std_string(1);
  uint32_t proto = js_method_arg_as_uint32(2);
  int32_t flags = js_method_arg_as_int32(3);
  
  int ret = create_inet_stream_socket(host.c_str(), service.c_str(), proto, flags);
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__create_inet_dgram_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_uint32(0);
  js_method_arg_assert_int32(1);
  
  uint32_t proto = js_method_arg_as_uint32(0);
  int32_t flags = js_method_arg_as_int32(1);
  
  int ret = create_inet_dgram_socket(proto, flags);
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__sendto_inet_dgram_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(6);
  
  js_method_arg_assert_int32(0);
  js_method_arg_assert_buffer(1);
  js_method_arg_assert_uint32(2);
  js_method_arg_assert_string(3);
  js_method_arg_assert_string(4);
  js_method_arg_assert_int32(5);
  
  int32_t fd = js_method_arg_as_int32(0);
  BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 1);
  uint32_t size = js_method_arg_as_uint32(2);
  std::string host = js_method_arg_as_std_string(3);
  std::string service = js_method_arg_as_std_string(4);
  int32_t flags = js_method_arg_as_int32(5);
  
  int32_t ret = sendto_inet_dgram_socket(fd, pBuffer->buffer().data(), size, host.c_str(), service.c_str(), flags);
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__recvfrom_inet_dgram_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(4);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_buffer(1);
  js_method_arg_assert_uint32(2);
  js_method_arg_assert_int32(3);
  
  int32_t fd = js_method_arg_as_int32(0);
  BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 1);
  uint32_t size = js_method_arg_as_uint32(2);
  int32_t flags = js_method_arg_as_int32(3);
  
  memset(HOST_BUF, 0, HOST_BUF_SIZE);
  memset(SERVICE_BUF, 0, SERVICE_BUF_SIZE);
  uint32_t ret = recvfrom_inet_dgram_socket(fd, pBuffer->buffer().data(), size, HOST_BUF, HOST_BUF_SIZE, SERVICE_BUF, SERVICE_BUF_SIZE, flags, LIBSOCKET_NUMERIC);
  
  JSObjectHandle result = JSObject();
  result->Set(JSLiteral("size"), JSUInt32(ret));
  result->Set(JSLiteral("host"), JSString(HOST_BUF));
  result->Set(JSLiteral("port"), JSString(SERVICE_BUF));
  result->Set(JSLiteral("buffer"), js_method_arg(1));
  
  return result;
}

JS_METHOD_IMPL(__connect_inet_dgram_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_string(2);
  
  int32_t fd = js_method_arg_as_int32(0);
  std::string host = js_method_arg_as_std_string(1);
  std::string service = js_method_arg_as_std_string(2);
  
  int32_t ret =  connect_inet_dgram_socket(fd, host.c_str(), service.c_str());
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__destroy_inet_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int32_t fd = js_method_arg_as_int32(0);
  int32_t ret = destroy_inet_socket(fd);
  return JSInt32(ret);
}

JS_METHOD_IMPL(__shutdown_inet_stream_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_int32(1);
  int32_t fd = js_method_arg_as_int32(0);
  int32_t method = js_method_arg_as_int32(1);
  int32_t ret = shutdown_inet_stream_socket(fd, method);
  return JSInt32(ret);
}

JS_METHOD_IMPL(__create_inet_server_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(5);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_uint32(2);
  js_method_arg_assert_uint32(3);
  js_method_arg_assert_int32(4);
  
  std::string host = js_method_arg_as_std_string(0);
  std::string service = js_method_arg_as_std_string(1);
  uint32_t proto = js_method_arg_as_uint32(2);
  uint32_t family = js_method_arg_as_uint32(3);
  int32_t flags = js_method_arg_as_int32(4);
  
  int32_t ret = create_inet_server_socket(host.c_str(), service.c_str(), proto, family, flags);
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__accept_inet_stream_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_int32(1);
  
  int32_t fd = js_method_arg_as_int32(0);
  int32_t flags = js_method_arg_as_int32(1);
  
  memset(HOST_BUF, 0, HOST_BUF_SIZE);
  memset(SERVICE_BUF, 0, SERVICE_BUF_SIZE);
  int32_t ret = accept_inet_stream_socket(fd, HOST_BUF, HOST_BUF_SIZE, SERVICE_BUF, SERVICE_BUF_SIZE, LIBSOCKET_NUMERIC, flags);
  
  return JSInt32(ret);
}

JS_METHOD_IMPL(__get_address_family)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string host = js_method_arg_as_std_string(0);
  int32_t ret = get_address_family(host.c_str());
  return JSInt32(ret);
}

JS_METHOD_IMPL(__create_multicast_socket)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_string(2);
  
  std::string group = js_method_arg_as_std_string(0);
  std::string port = js_method_arg_as_std_string(1);
  std::string ifname = js_method_arg_as_std_string(2);
  
  int32_t ret = create_multicast_socket(group.c_str(), port.c_str(), ifname.c_str());
  
  
  return JSInt32(ret);
}

JS_EXPORTS_INIT()
{
  js_enter_scope();

  js_export_method("create_inet_stream_socket", __create_inet_stream_socket);
  js_export_method("create_inet_dgram_socket", __create_inet_dgram_socket);
  js_export_method("sendto_inet_dgram_socket", __sendto_inet_dgram_socket);
  js_export_method("recvfrom_inet_dgram_socket", __recvfrom_inet_dgram_socket);
  js_export_method("connect_inet_dgram_socket", __connect_inet_dgram_socket);
  js_export_method("destroy_inet_socket", __destroy_inet_socket);
  js_export_method("shutdown_inet_stream_socket", __shutdown_inet_stream_socket);
  js_export_method("create_inet_server_socket", __create_inet_server_socket);
  js_export_method("accept_inet_stream_socket", __accept_inet_stream_socket);
  js_export_method("get_address_family", __get_address_family);
  js_export_method("create_multicast_socket", __create_multicast_socket);
  
  js_export_int32("SOCKET_TCP", LIBSOCKET_TCP);
  js_export_int32("SOCKET_UDP", LIBSOCKET_UDP);
  js_export_int32("SOCKET_IPV4", LIBSOCKET_IPv4);
  js_export_int32("SOCKET_IPV6", LIBSOCKET_IPv6);
  js_export_int32("SOCKET_BOTH", LIBSOCKET_BOTH);
  js_export_int32("SOCKET_READ", LIBSOCKET_READ);
  js_export_int32("SOCKET_WRITE", LIBSOCKET_WRITE);
  js_export_int32("SOCKET_NUMERIC", LIBSOCKET_NUMERIC);
  
  js_export_const(MSG_CONFIRM);
  js_export_const(MSG_DONTROUTE);
  js_export_const(MSG_DONTWAIT);
  js_export_const(MSG_EOR);
  js_export_const(MSG_MORE);
  js_export_const(MSG_NOSIGNAL);
  js_export_const(MSG_OOB);
  
  js_export_const(MSG_CMSG_CLOEXEC);
  js_export_const(MSG_DONTWAIT);
  js_export_const(MSG_ERRQUEUE);
  js_export_const(MSG_PEEK);
  js_export_const(MSG_TRUNC);
  js_export_const(MSG_WAITALL);
  js_export_const(MSG_CTRUNC);
  
  js_export_const(SOCK_NONBLOCK);
  js_export_const(SOCK_CLOEXEC);
  
  js_export_const(SO_DEBUG);
  js_export_const(SO_BROADCAST);
  js_export_const(SO_REUSEADDR);
  js_export_const(SO_KEEPALIVE);
  js_export_const(SO_LINGER);
  js_export_const(SO_OOBINLINE);
  js_export_const(SO_SNDBUF);
  js_export_const(SO_RCVBUF);
  js_export_const(SO_DONTROUTE);
  js_export_const(SO_RCVLOWAT);
  js_export_const(SO_RCVTIMEO);
  js_export_const(SO_SNDLOWAT);
  js_export_const(SO_SNDTIMEO);
  

  js_export_finalize();
}

JS_REGISTER_MODULE(JSInetSocket);
