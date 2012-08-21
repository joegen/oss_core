// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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

#ifndef SIPTRANSPORTRATELIMITSTRATEGY_H
#define	SIPTRANSPORTRATELIMITSTRATEGY_H


#include <set>
#include <map>
#include <vector>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace OSS {
namespace SIP {

class SIPTransportRateLimitStrategy
{
public:

  SIPTransportRateLimitStrategy();
  ~SIPTransportRateLimitStrategy();

  void logPacket(const boost::asio::ip::address& source, std::size_t bytesRead);

  bool isBannedAddress(const boost::asio::ip::address& source);

  void banAddress(const boost::asio::ip::address& source);

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

  void whiteListAddress(const boost::asio::ip::address& address);
  void whiteListNetwork(const std::string& network);
  bool isWhiteListed(const boost::asio::ip::address& address) const;
  bool isWhiteListedNetwork(const boost::asio::ip::address& address) const;
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
};

//
// Inlines
//

inline bool& SIPTransportRateLimitStrategy::enabled()
{
  return _enabled;
}

inline unsigned long SIPTransportRateLimitStrategy::getPacketsPerSecondThreshold() const
{
  return _packetsPerSecondThreshold;
}

inline void SIPTransportRateLimitStrategy::setPacketsPerSecondThreshold(unsigned long threshold)
{
  _packetsPerSecondThreshold = threshold;
}

inline unsigned long SIPTransportRateLimitStrategy::getThresholdViolationRate() const
{
  return _thresholdViolationRate;
}

inline unsigned long SIPTransportRateLimitStrategy::getCurrentIterationCount() const
{
  return _currentIterationCount;
}

inline void SIPTransportRateLimitStrategy::setThresholdViolationRate(unsigned long threshold)
{
  _thresholdViolationRate = threshold;
}

inline bool& SIPTransportRateLimitStrategy::autoBanThresholdViolators()
{
  return _autoBanThresholdViolators;
}

inline void SIPTransportRateLimitStrategy::setBanLifeTime(int lifetime)
{
  _banLifeTime = lifetime;
}

inline int SIPTransportRateLimitStrategy::getBanLifeTime() const
{
  return _banLifeTime;
}



} } // OSS::SIP


#endif	/* SIPTRANSPORTRATELIMITSTRATEGY_H */

