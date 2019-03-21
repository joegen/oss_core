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

#ifndef SBCJSMODULEMANAGER_H
#define SBCJSMODULEMANAGER_H


#include "OSS/JS/JSIsolateManager.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SIP/SBC/SBCCDRRecord.h"
#include "OSS/JSON/Json.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/SIPMessage.h"

namespace OSS {
namespace SIP {
namespace SBC {
  
class SBCManager;

class SBCJSModuleManager
{
public:
  typedef OSS::JS::JSIsolate JSIsolate;
  typedef std::map<std::string, std::string> CustomEventArgs;
  void run(const std::string& scriptFile, bool threaded = true);
  void stop();
  bool processTransactionEvent(const std::string& eventName, const OSS::SIP::B2BUA::SIPB2BTransaction::Ptr& pTransaction, OSS::JSON::Object& result);
  bool processRequestEvent(const std::string& eventName, const OSS::SIP::SIPMessage::Ptr& pMessage, OSS::JSON::Object& result);
  void notifyTransactionEvent(const std::string& eventName, const OSS::SIP::B2BUA::SIPB2BTransaction::Ptr& pTransaction);
  void notifyCdrEvent(const std::string& eventName, const SBCCDRRecord& pCdrEvent);
  bool processCustomEvent(const std::string& eventName, const CustomEventArgs& args, CustomEventArgs& result);
  SBCManager* getManager();
  static SBCJSModuleManager* createInstance(SBCManager* pManager);
  static SBCJSModuleManager* instance();
  
protected:
  SBCJSModuleManager(SBCManager* pManager);
  ~SBCJSModuleManager();
  void internal_run();
  
private:
  static SBCJSModuleManager* _pInstance;
  SBCManager* _pManager;
  JSIsolate::Ptr _pIsolate;
  boost::thread* _pThread;
  std::string _scriptFile;
};

//
// Inlines
//

inline SBCManager* SBCJSModuleManager::getManager()
{
  return _pManager;
}


} } } // OSS::SIP::SBC

#endif // SBCJSMODULEMANAGER_H

