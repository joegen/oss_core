
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

#include "OSS/Net/AccessControl.h"
#include "OSS/Logger.h"
#include "OSS/Net.h"

namespace OSS {
namespace Net {

  
static std::time_t to_time_t(boost::posix_time::ptime t)
{
  boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
  boost::posix_time::time_duration::sec_type x = (t - epoch).total_seconds();
  return time_t(x);
}


AccessControl::AccessControl() :
  _enabled(false),
  _packetsPerSecondThreshold(100),
  _thresholdViolationRate(50),
  _currentIterationCount(0),
  _autoBanThresholdViolators(true),
  _banLifeTime(0),
  _pStore(0)
{
  _lastTime = boost::posix_time::microsec_clock::local_time();
}

AccessControl::AccessControl(OSS::Persistent::KeyValueStore* pStore) :
  _enabled(false),
  _packetsPerSecondThreshold(100),
  _thresholdViolationRate(50),
  _currentIterationCount(0),
  _autoBanThresholdViolators(true),
  _banLifeTime(0),
  _pStore(pStore)
{
  _lastTime = boost::posix_time::microsec_clock::local_time();
}


AccessControl::~AccessControl()
{
}

void AccessControl::logPacket(const boost::asio::ip::address& source, std::size_t bytesRead)
{
   /****************************************************************************
    * The packet rate ratio allows the transport to detect a potential DoS     *
    * attack.  It works by detecting the packet read rate per second as        *
    * designated by the upper limit.  If the value of packet rate is 50/100,   *
    * the maximum packet rate before the SBC raises the alert level if a       *
    * potential denial of service attack is 100 packets per second.            *
    * When this happens, the transport layer checks if there is a particular   *
    * IP that is sending more than its allowable rate of 50 packets per second.*
    * If the sender is violating the threshold, it will be banned for 1 hour   *
    * which is the third parameter of 3600 seconds.                            *
    ****************************************************************************/
  
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
      OSS_LOG_WARNING("ALERT: Threshold Violation Detected.  Rate" << _currentIterationCount << " >= " << _packetsPerSecondThreshold);

      std::size_t watermark = 0;
      boost::asio::ip::address suspect;
      for (std::map<boost::asio::ip::address, unsigned int>::iterator iter = _packetCounter.begin();
        iter != _packetCounter.end(); iter++)
      {
        if (iter->second > watermark)
        {
          watermark = iter->second;
          suspect = iter->first;
          
          if (watermark >= _thresholdViolationRate && _autoBanThresholdViolators )
          {
            if (!isWhiteListed(suspect))
            {
              OSS_LOG_WARNING("ALERT: Threshold Violator Address = " << suspect.to_string() <<
                " Packets sent within the last second is " << watermark
                << ". Violator is now in jail for a maximum of " << _banLifeTime << " seconds.");
              _blackList[suspect] = now;
            }
            else
            {
              OSS_LOG_WARNING("ALERT: Threshold Violator Address = " << suspect.to_string() <<
                " Packets sent within the last second is " << watermark
                << ". Violator is TRUSTED and will be allowed to bombard.");
              _blackList[suspect] = now;
            }
          }
          else if (watermark >= _thresholdViolationRate && !_autoBanThresholdViolators )
          {
            OSS_LOG_WARNING("ALERT: Threshold Violator Address = " << suspect.to_string() <<
                " Packets sent within the last second is " << watermark
                << ". Automatic ban is disabled.  Allowing this IP to bombard.");
          }
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

bool AccessControl::isBannedAddress(const boost::asio::ip::address& source)
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

void AccessControl::banAddress(const boost::asio::ip::address& source)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();
  _blackList[source] = boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time());
  _packetCounterMutex.unlock();
}

void AccessControl::clearAddress(const boost::asio::ip::address& source, bool addToWhiteList)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();
  _blackList.erase(source);
  if (addToWhiteList)
    _whiteList.insert(source);
  _packetCounterMutex.unlock();
}


void AccessControl::whiteListAddress(const boost::asio::ip::address& address)
{
  _packetCounterMutex.lock();
  if (_whiteList.find(address) == _whiteList.end())
  {
    _blackList.erase(address);
    _whiteList.insert(address);
  }
  _packetCounterMutex.unlock();
}

void AccessControl::whiteListNetwork(const std::string& network)
{
  _packetCounterMutex.lock();
  if (_networkWhiteList.find(network) == _networkWhiteList.end())
    _networkWhiteList.insert(network);
  _packetCounterMutex.unlock();
}

bool AccessControl::isWhiteListed(const boost::asio::ip::address& address) const
{
  bool whiteListed = false;
  whiteListed = _whiteList.find(address) != _whiteList.end();
  if (!whiteListed)
    whiteListed = isWhiteListedNetwork(address);
  return whiteListed;
}

bool AccessControl::isWhiteListedNetwork(const boost::asio::ip::address& address) const
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




