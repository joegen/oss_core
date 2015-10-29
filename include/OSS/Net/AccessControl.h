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
  
  typedef std::map<boost::asio::ip::address, boost::posix_time::ptime> BannedSources;
  typedef boost::function<void(const boost::asio::ip::address&)> BanCallback;
  typedef boost::function<void(const boost::asio::ip::address&)> BlackListCallback;
  typedef std::set<std::string> NetworkBlackList;
  typedef std::set<std::string> NetworkWhiteList;
  typedef std::set<boost::asio::ip::address> IPBlackList;
  typedef std::set<boost::asio::ip::address> IPWhiteList;
  typedef std::map<boost::asio::ip::address, unsigned int> PacketCounter;
  
  AccessControl();
  
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
  
  void blackListAddress(const boost::asio::ip::address& address, bool removeFromWhiteList = true);
  void blackListAddress(const std::string& address, bool removeFromWhiteList = true);
  void blackListNetwork(const std::string& network);
  bool isBlackListed(const boost::asio::ip::address& address) const;
  bool isBlackListed(const std::string& address) const;
  bool isBlackListedNetwork(const boost::asio::ip::address& address) const;
  bool isBlackListedNetwork(const std::string& address) const;
  
  void denyAll(bool denyAll);
  
  void getBannedAddresses(std::vector<boost::asio::ip::address>& banned);
  
  void setBanCallback(const BanCallback& banCallback);
  
private:
  bool _enabled;
  unsigned long _packetsPerSecondThreshold;
  unsigned long _thresholdViolationRate;
  unsigned long _currentIterationCount;
  bool _autoBanThresholdViolators;
  int _banLifeTime;
  mutable boost::recursive_mutex _packetCounterMutex;
  std::map<boost::asio::ip::address, unsigned int> _packetCounter;
  IPWhiteList _whiteList;
  NetworkWhiteList _networkWhiteList;
  IPBlackList _blackList;
  NetworkBlackList _networkBlackList;
  BannedSources _banned;
  boost::posix_time::ptime _lastTime;
  bool _denyAllIncoming;
  BanCallback _banCallback;
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

inline void AccessControl::blackListAddress(const std::string& address, bool removeFromBlackList)
{
  blackListAddress(boost::asio::ip::address::from_string(address), removeFromBlackList);
}

inline bool AccessControl::isBlackListed(const std::string& address) const
{
  return isBlackListed(boost::asio::ip::address::from_string(address));
}

inline bool AccessControl::isBlackListedNetwork(const std::string& address) const
{
  return isBlackListedNetwork(boost::asio::ip::address::from_string(address));
}

inline void AccessControl::logPacket(const std::string& source, std::size_t bytesRead, ViolationReport* pReport)
{
  logPacket(boost::asio::ip::address::from_string(source), bytesRead, pReport);
}

inline void AccessControl::setBanCallback(const BanCallback& banCallback)
{
  _banCallback = banCallback;
}


} } // OSS::SIP


#endif	/* ACCESSCONTROL_H_INCLUDED */

