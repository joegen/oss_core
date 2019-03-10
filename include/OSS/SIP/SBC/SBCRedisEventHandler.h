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

#ifndef SBCREDISEVENTHANDLER_H_INCLUDED
#define	SBCREDISEVENTHANDLER_H_INCLUDED


#include "OSS/OSS.h"


namespace OSS {
namespace SIP {
namespace SBC {

  
class SBCManager;

class SBCRedisEventHandler
{
public:
  
  SBCRedisEventHandler();
  
  ~SBCRedisEventHandler();
  
  bool initialize(SBCManager* pManager);

protected:
   struct Task
  {
    std::string taskName;
    std::map<std::string, std::string> taskProperties;
  };
  
  void runTasks();
  void handleTask(Task& task);
  void initializeBlockedSources();
  void setLogLevel(const std::string& eventName, json::Object& eventObject);
  void addLocalAccount(const std::string& eventName, json::Object& eventObject);
  void handleBlockSource(const boost::asio::ip::address& address);
  void unblockSource(const std::string& eventName, json::Object& eventObject);
  void blockSource(const std::string& eventName, json::Object& eventObject);
  void blockNetwork(const std::string& eventName, json::Object& eventObject);
  void unblockNetwork(const std::string& eventName, json::Object& eventObject);
  void whiteListSource(const std::string& eventName, json::Object& eventObject);
  void whiteListNetwork(const std::string& eventName, json::Object& eventObject);
  void removeWhiteListSource(const std::string& eventName, json::Object& eventObject);
  void removeWhiteListNetwork(const std::string& eventName, json::Object& eventObject);
  
private:
  BlockingQueue<Task*> _tasks;
  SBCManager* _pManager;
  boost::thread* _pTaskThread;
};
  

  
//
// Inlines
//
  
} } } // OSS::SIP::SBC


#endif	// SBCREDISEVENTHANDLER_H_INCLUDED

