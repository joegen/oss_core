// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SBC/SBCJSModuleManager.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
using OSS::JS::JSIsolate;
using OSS::JS::JSIsolateManager;

SBCJSModuleManager* SBCJSModuleManager::_pInstance = 0;

SBCJSModuleManager* SBCJSModuleManager::createInstance(SBCManager* pManager)
{
  if (pManager && !SBCJSModuleManager::_pInstance)
  {
    SBCJSModuleManager::_pInstance = new SBCJSModuleManager(pManager);
  }
  return SBCJSModuleManager::_pInstance;
}

SBCJSModuleManager* SBCJSModuleManager::instance()
{
  return SBCJSModuleManager::createInstance(0);
}

SBCJSModuleManager::SBCJSModuleManager(SBCManager* pManager) :
  _pManager(pManager),
  _pThread(0)
{
}

SBCJSModuleManager::~SBCJSModuleManager()
{
  stop();
}

void SBCJSModuleManager::run()
{
  assert(!_pThread);
  _pThread = new boost::thread(boost::bind(&SBCJSModuleManager::internal_run, this));
}

void SBCJSModuleManager::stop()
{
  if (_pIsolate)
  {
    _pIsolate->dispose();
  }
  _pIsolate.reset();
  if (_pThread)
  {
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

void SBCJSModuleManager::internal_run()
{
  OSS_LOG_INFO("SBCModuleManager - Running root isolate");
  _pIsolate = JSIsolateManager::instance().rootIsolate();
  std::string path = OSS::system_libdir() + "/oss_modules/karoo/_karoo_modules.js";
  boost::filesystem::path script(path);
  JSIsolateManager::instance().run(
    _pIsolate,
    script
  );
  OSS_LOG_INFO("SBCModuleManager - Root isolate thread ended");
}

bool SBCJSModuleManager::processTransactionEvent(const std::string& eventName, const SIPB2BTransaction::Ptr& pTransaction, OSS::JSON::Object& result)
{
  if (!_pIsolate)
  {
    return false;
  }
  OSS::JSON::Object event, arguments;
  event["method"] = OSS::JSON::String(eventName);
  arguments["type"] = OSS::JSON::String("method");
  arguments["dataSource"] = OSS::JSON::String("transaction");
  event["arguments"] = arguments;
  return JS::JSIsolateManager::instance().rootIsolate()->execute(event, result, 0, pTransaction.get());
}

bool SBCJSModuleManager::processRequestEvent(const std::string& eventName, const SIPMessage::Ptr& pMessage, OSS::JSON::Object& result)
{
  if (!_pIsolate)
  {
    return false;
  }
  OSS::JSON::Object event, arguments;
  event["method"] = OSS::JSON::String(eventName);
  arguments["type"] = OSS::JSON::String("method");
  arguments["dataSource"] = OSS::JSON::String("request");
  event["arguments"] = arguments;
  return JS::JSIsolateManager::instance().rootIsolate()->execute(event, result, 0, pMessage.get());
}

void SBCJSModuleManager::notifyTransactionEvent(const std::string& eventName, const SIPB2BTransaction::Ptr& pTransaction)
{
  if (!_pIsolate)
  {
    return;
  }
  
  OSS::JSON::Object event, arguments;
  event["method"] = OSS::JSON::String(eventName);
  arguments["type"] = OSS::JSON::String("notify");
  arguments["dataSource"] = OSS::JSON::String("transaction");
  event["arguments"] = arguments;
  JS::JSIsolateManager::instance().rootIsolate()->notify(event, pTransaction.get());
}

void SBCJSModuleManager::notifyCdrEvent(const std::string& eventName, const SBCCDRRecord& pCdrEvent)
{
  if (!_pIsolate)
  {
    return;
  }
  
  OSS::JSON::Object event, arguments;
  event["method"] = OSS::JSON::String(eventName);
  arguments["type"] = OSS::JSON::String("notify");
}

} } } // OSS::SIP::SBC



