#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipSIPStack.h"


using OSS::JS::JSObjectWrap;
using namespace resip;

/// SipStack ///

JS_CLASS_INTERFACE(ResipSIPStack, "SipStack") 
{
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "run", run);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "shutdown", shutdown);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "addTransport", addTransport);
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
  self->stack()->addTransport(transport_type, port);
  return JSUndefined();
}