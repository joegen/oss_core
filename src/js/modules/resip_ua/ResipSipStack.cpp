#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipSIPStack.h"
#include "resip/stack/Embedded.hxx"


using OSS::JS::JSObjectWrap;
using namespace resip;

/// SipStack ///

JS_CLASS_INTERFACE(ResipSIPStack, "SipStack") 
{
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "run", run);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "shutdown", shutdown);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "addTransport", addTransport);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "sendStunTest", sendStunTest);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "getStunAddress", getStunAddress);
  JS_CLASS_INTERFACE_END(ResipSIPStack); 
}

ResipSIPStack::ResipSIPStack() :
  _stack(0),
  _pollGrp(0),
  _interruptor(0),
  _thread(0)
{
}

ResipSIPStack::~ResipSIPStack()
{
  if (_stack)
  {
    _stack->shutdownAndJoinThreads();
    _thread->shutdown();
    _thread->join();
  }
   
  delete _thread;
  delete _stack;
  delete _pollGrp;
  delete _interruptor;
}

JS_CONSTRUCTOR_IMPL(ResipSIPStack)
{
  OSS::JS::JSIsolate::getIsolate()->setForceAsync(true);
  ResipSIPStack* object = new ResipSIPStack();
  object->_pollGrp = FdPollGrp::create();
  object->_interruptor = new EventThreadInterruptor(*object->_pollGrp);
  object->_stack = new SipStack(0, DnsStub::EmptyNameserverList, object->_interruptor, false, 0, 0, object->_pollGrp);
  object->_thread = new EventStackThread(*object->_stack, *object->_interruptor, *object->_pollGrp);
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipSIPStack::run)
{
  js_method_arg_declare_self(ResipSIPStack, stack);
  stack->_stack->run();
  stack->_thread->run();
  return JSUndefined();
}

JS_METHOD_IMPL(ResipSIPStack::shutdown)
{
  js_method_arg_declare_self(ResipSIPStack, stack);
  stack->_stack->shutdownAndJoinThreads();
  stack->_thread->shutdown();
  stack->_thread->join();
  return JSUndefined();
}

JS_METHOD_IMPL(ResipSIPStack::addTransport)
{
  js_method_arg_declare_self(ResipSIPStack, self);
  js_method_arg_declare_int32(type, 0);
  js_method_arg_declare_uint32(port, 1);
  assert(self);
  TransportType transport_type = (TransportType)type;
  if (transport_type == resip::UDP)
  {
    resip::UdpTransport* transport = (resip::UdpTransport*)self->stack()->addTransport(transport_type, port, resip::V4, resip::StunEnabled);
    if (transport)
    {
      self->_udpTransports[port] = transport;
    }
  }
  else 
  {
    self->stack()->addTransport(transport_type, port);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ResipSIPStack::sendStunTest)
{
  js_method_arg_declare_self(ResipSIPStack, self);
  js_method_arg_declare_string(stunServer, 0);
  js_method_arg_declare_uint32(stunPort, 1);
  js_method_arg_declare_uint32(localUdpPort, 2);
  
  UDPTransports::iterator iter = self->_udpTransports.find(localUdpPort);
  if (iter == self->_udpTransports.end())
  {
    return JSBoolean(false);
  }
  
  hostent* h = gethostbyname(stunServer.c_str());
  in_addr sin_addr = *(struct in_addr*)h->h_addr;
  resip::Tuple stunDest(sin_addr, stunPort, UDP, Data::Empty);
  iter->second->stunSendTest(stunDest);
  return JSBoolean(true);
}

JS_METHOD_IMPL(ResipSIPStack::getStunAddress)
{
  js_method_arg_declare_self(ResipSIPStack, self);
  js_method_arg_declare_uint32(localUdpPort, 0);
  UDPTransports::iterator iter = self->_udpTransports.find(localUdpPort);
  if (iter == self->_udpTransports.end())
  {
    return JSUndefined();
  }
  
  resip::Tuple mappedAddress;
  mappedAddress.setPort(0);
  if (!iter->second->stunResult(mappedAddress))
  {
    return JSUndefined();
  }
  
  char ipAddress[INET_ADDRSTRLEN];
  struct sockaddr_in *addr_in = (struct sockaddr_in *) &(mappedAddress.getMutableSockaddr());
  inet_ntop(AF_INET, &(addr_in->sin_addr), ipAddress, INET_ADDRSTRLEN);

  JSLocalValueHandle result = JSObject();
  result->ToObject()->Set(JSLiteral("ip"), JSString(ipAddress));
  result->ToObject()->Set(JSLiteral("port"), JSUInt32(mappedAddress.getPort()));
  return result;
}