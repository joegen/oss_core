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

#include "OSS/SIP/SIPTransportRateLimitStrategy.h"
#include "OSS/Logger.h"
#include "OSS/Net.h"

namespace OSS {
namespace SIP {


SIPTransportRateLimitStrategy::SIPTransportRateLimitStrategy() :
  _enabled(false),
  _packetsPerSecondThreshold(100),
  _thresholdViolationRate(50),
  _currentIterationCount(0),
  _autoBanThresholdViolators(true),
  _banLifeTime(0)
{
  _lastTime = boost::posix_time::microsec_clock::local_time();
}

SIPTransportRateLimitStrategy::~SIPTransportRateLimitStrategy()
{
}

void SIPTransportRateLimitStrategy::logPacket(const boost::asio::ip::address& source, std::size_t bytesRead)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();

  std::map<boost::asio::ip::address, unsigned int>::iterator iter = _packetCounter.find(source);
  if (iter != _packetCounter.end())
    _packetCounter[source] = ++iter->second;
  else
    _packetCounter[source] = 1;

  if (++_currentIterationCount >= _packetsPerSecondThreshold)
  {
    boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());
    _currentIterationCount = 0;
    boost::posix_time::time_duration timeDiff = now - _lastTime;
    if (timeDiff.total_milliseconds() <=  1000)
    {
      //
      // We got a ratelimit violation
      //
      OSS_LOG_WARNING("ALERT: Threshold Violation Detected.  Rate >= " << _packetsPerSecondThreshold);

      std::size_t watermark = 0;
      boost::asio::ip::address suspect;
      for (std::map<boost::asio::ip::address, unsigned int>::iterator iter = _packetCounter.begin();
        iter != _packetCounter.end(); iter++)
      {
        if (iter->second > watermark)
        {
          watermark = iter->second;
          suspect = iter->first;
        }
      }

      if (watermark >= _thresholdViolationRate && _autoBanThresholdViolators)
      {
        if (_whiteList.find(source) == _whiteList.end())
        {
          OSS_LOG_WARNING("ALERT: Threshold Violator Address = " << source.to_string() <<
            " Packets sent within the last second is " << watermark
            << ". Violator is now in jail for a maximum of " << _banLifeTime << " seconds.");
          _blackList[source] = now;
        }
      }
    }
    //
    // Reset
    //
    _packetCounter.clear();
    _lastTime = now;
  }


  _packetCounterMutex.unlock();
}

bool SIPTransportRateLimitStrategy::isBannedAddress(const boost::asio::ip::address& source)
{
  if (!_enabled)
    return false;

  bool banned = false;
  _packetCounterMutex.lock();

  if (_banLifeTime > 0)
  {
    boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());
    std::vector<boost::asio::ip::address> parole;
    for (std::map<boost::asio::ip::address, boost::posix_time::ptime>::iterator iter = _blackList.begin();
      iter != _blackList.end(); iter++)
    {
      boost::posix_time::time_duration timeDiff = now - iter->second;
      if (timeDiff.total_milliseconds() >  _banLifeTime * 1000)
        parole.push_back(iter->first);
    }

    for (std::vector<boost::asio::ip::address>::iterator iter = parole.begin(); iter != parole.end(); iter++)
    {
      _blackList.erase(*iter);
    }
  }

  if (isWhiteListed(source))
    banned = false;
  else
    banned = _blackList.find(source) != _blackList.end();
  _packetCounterMutex.unlock();

  return banned;
}

void SIPTransportRateLimitStrategy::banAddress(const boost::asio::ip::address& source)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();
  _blackList[source] = boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time());
  _packetCounterMutex.unlock();
}

void SIPTransportRateLimitStrategy::clearAddress(const boost::asio::ip::address& source, bool addToWhiteList)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();
  _blackList.erase(source);
  if (addToWhiteList)
    _whiteList.insert(source);
  _packetCounterMutex.unlock();
}


void SIPTransportRateLimitStrategy::whiteListAddress(const boost::asio::ip::address& address)
{
  _packetCounterMutex.lock();
  if (_whiteList.find(address) == _whiteList.end())
  {
    _blackList.erase(address);
    _whiteList.insert(address);
  }
  _packetCounterMutex.unlock();
}

void SIPTransportRateLimitStrategy::whiteListNetwork(const std::string& network)
{
  _packetCounterMutex.lock();
  if (_networkWhiteList.find(network) == _networkWhiteList.end())
    _networkWhiteList.insert(network);
  _packetCounterMutex.unlock();
}

bool SIPTransportRateLimitStrategy::isWhiteListed(const boost::asio::ip::address& address) const
{
  bool whiteListed = false;
  whiteListed = _whiteList.find(address) != _whiteList.end();
  if (!whiteListed)
    whiteListed = isWhiteListedNetwork(address);
  return whiteListed;
}

bool SIPTransportRateLimitStrategy::isWhiteListedNetwork(const boost::asio::ip::address& address) const
{
  boost::system::error_code ec;
  std::string ipAddress = address.to_string(ec);
  if (ec)
    return false;

  for (std::set<std::string>::const_iterator iter = _networkWhiteList.begin();
    iter != _networkWhiteList.end(); iter++)
  {
    if (OSS::socket_address_cidr_verify(ipAddress, *iter))
      return true;
  }

  return false;
}

} } // OSS::SIP





