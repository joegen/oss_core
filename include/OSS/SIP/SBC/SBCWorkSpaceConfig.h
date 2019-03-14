// OSS Software Solutions Application Programmer Interface
// Package: SBC
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

#ifndef SBCREDISCONFIG_H_INCLUDED
#define	SBCREDISCONFIG_H_INCLUDED


#include <boost/filesystem.hpp>
#include "OSS/Persistent/RedisClient.h"
#include "OSS/SIP/SBC/SBCWorkSpace.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
class SBCWorkSpaceConfig
{
public:
  struct HostEntry
  {
    std::string host;
    int port;
    std::string password;
  };
  
  typedef std::vector<HostEntry> HostEntries; 
  
  SBCWorkSpaceConfig(const std::string& componentName);
  SBCWorkSpaceConfig(const std::string& componentName, const boost::filesystem::path& configFile);
  ~SBCWorkSpaceConfig();
  void addHost(const std::string host, int port, const std::string& password);
  bool connect(SBCWorkSpace& redis, int workspace, bool allowMultiple = true);
  bool connect(Persistent::RedisClient& redis, int workspace);
  bool dumpConnectionInfoToFile();
  bool dumpConnectionInfoToFile(const boost::filesystem::path& infoFile);
protected:
   boost::filesystem::path _configFile;
   std::string _componentName;
   HostEntries _hosts;
   bool _enableRemoteDb;
};
  
} } } // OSS::SIP::SBC

#endif	// SBCREDISCONFIG_H_INCLUDED

