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

#ifndef ACCESSCONTROL_H_INCLUDED
#define	ACCESSCONTROL_H_INCLUDED


#include <set>
#include <map>
#include <vector>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "OSS/Persistent/KeyValueStore.h"


namespace OSS {
namespace Net {

class AccessControl
{
public:

  struct ViolationReport
  {
    bool thresholdViolated;
    std::vector<std::string> violators;
    ViolationReport() : thresholdViolated(false){};
  };
  
  AccessControl();
  
  AccessControl(OSS::Persistent::KeyValueStore* pStore);
  
  ~AccessControl();

  void logPacket(const boost::asio::ip::address& source, std::size_t bytesRead, ViolationReport* pReport = 0);
  
  void logPacket(const std::string& source, std::size_t bytesRead, ViolationReport* pReport = 0);

  bool isBannedAddress(const boost::asio::ip::address& source);
  
  bool isBannedAddress(const std::string& source);

  void banAddress(const boost::asio::ip::address& source);
  
  void banAddress(const std::string& source);

  void clearAddress(const boost::asio::ip::address& source, bool addToWhiteList);

  unsigned long getPacketsPerSecondThreshold() const;

  void setPacketsPerSecondThreshold(unsigned long threshold);

  unsigned long getThresholdViolationRate() const;

  void setThresholdViolationRate(unsigned long threshold);

  unsigned long getCurrentIterationCount() const;

  bool& autoBanThresholdViolators();

  void setBanLifeTime(int lifetime);

  int getBanLifeTime() const;

  bool& enabled();

  void whiteListAddress(const boost::asio::ip::address& address, bool removeFromBlackList = true);
  
  void whiteListAddress(const std::string& address, bool removeFromBlackList = true);
  
  void whiteListNetwork(const std::string& network);
  
  bool isWhiteListed(const boost::asio::ip::address& address) const;
  
  bool isWhiteListed(const std::string& address) const;
  
  bool isWhiteListedNetwork(const boost::asio::ip::address& address) const;
  
  bool isWhiteListedNetwork(const std::string& address) const;
  
  void setPersistentStore(OSS::Persistent::KeyValueStore* pStore);
  
  void denyAll(bool denyAll);
  
private:
  bool _enabled;
  unsigned long _packetsPerSecondThreshold;
  unsigned long _thresholdViolationRate;
  unsigned long _currentIterationCount;
  bool _autoBanThresholdViolators;
  int _banLifeTime;
  mutable boost::recursive_mutex _packetCounterMutex;
  std::map<boost::asio::ip::address, unsigned int> _packetCounter;
  std::set<boost::asio::ip::address> _whiteList;
  std::set<std::string> _networkWhiteList;
  std::map<boost::asio::ip::address, boost::posix_time::ptime> _blackList;
  boost::posix_time::ptime _lastTime;
  OSS::Persistent::KeyValueStore* _pStore;
  bool _denyAllIncoming;
};

//
// Inlines
//

inline bool& AccessControl::enabled()
{
  return _enabled;
}

inline unsigned long AccessControl::getPacketsPerSecondThreshold() const
{
  return _packetsPerSecondThreshold;
}

inline void AccessControl::setPacketsPerSecondThreshold(unsigned long threshold)
{
  _packetsPerSecondThreshold = threshold;
}

inline unsigned long AccessControl::getThresholdViolationRate() const
{
  return _thresholdViolationRate;
}

inline unsigned long AccessControl::getCurrentIterationCount() const
{
  return _currentIterationCount;
}

inline void AccessControl::setThresholdViolationRate(unsigned long threshold)
{
  _thresholdViolationRate = threshold;
}

inline bool& AccessControl::autoBanThresholdViolators()
{
  return _autoBanThresholdViolators;
}

inline void AccessControl::setBanLifeTime(int lifetime)
{
  _banLifeTime = lifetime;
}

inline int AccessControl::getBanLifeTime() const
{
  return _banLifeTime;
}

inline void AccessControl::banAddress(const std::string& source)
{
  banAddress(boost::asio::ip::address::from_string(source));
}

inline bool AccessControl::isBannedAddress(const std::string& source)
{
  return isBannedAddress(boost::asio::ip::address::from_string(source));
}

inline void AccessControl::whiteListAddress(const std::string& address, bool removeFromBlackList)
{
  whiteListAddress(boost::asio::ip::address::from_string(address), removeFromBlackList);
}

inline bool AccessControl::isWhiteListed(const std::string& address) const
{
  return isWhiteListed(boost::asio::ip::address::from_string(address));
}

inline bool AccessControl::isWhiteListedNetwork(const std::string& address) const
{
  return isWhiteListedNetwork(boost::asio::ip::address::from_string(address));
}

inline void AccessControl::logPacket(const std::string& source, std::size_t bytesRead, ViolationReport* pReport)
{
  logPacket(boost::asio::ip::address::from_string(source), bytesRead, pReport);
}

inline void AccessControl::setPersistentStore(OSS::Persistent::KeyValueStore* pStore)
{
  _pStore = pStore;
}


} } // OSS::SIP


#endif	/* ACCESSCONTROL_H_INCLUDED */

