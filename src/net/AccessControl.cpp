
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
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"

namespace OSS {
namespace Net {


AccessControl::AccessControl() :
  _enabled(false),
  _packetsPerSecondThreshold(100),
  _thresholdViolationRate(50),
  _currentIterationCount(0),
  _autoBanThresholdViolators(true),
  _banLifeTime(0),
  _denyAllIncoming(false)
{
  _lastTime = boost::posix_time::microsec_clock::local_time();
}


AccessControl::~AccessControl()
{
}

void AccessControl::logPacket(const boost::asio::ip::address& source, std::size_t bytesRead, ViolationReport* pReport)
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

      if (pReport)
        pReport->thresholdViolated = true;
      
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
              banAddress(suspect);
              
              if (pReport)
                pReport->violators.push_back(suspect.to_string());
            }
            else
            {
              OSS_LOG_WARNING("ALERT: Threshold Violator Address = " << suspect.to_string() <<
                " Packets sent within the last second is " << watermark
                << ". Violator is TRUSTED and will be allowed to bombard.");
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
    for (std::map<boost::asio::ip::address, boost::posix_time::ptime>::iterator iter = _banned.begin();
      iter != _banned.end(); iter++)
    {
      boost::posix_time::time_duration timeDiff = now - iter->second;
      if (timeDiff.total_milliseconds() >  _banLifeTime * 1000)
        parole.push_back(iter->first);
    }

    for (std::vector<boost::asio::ip::address>::iterator iter = parole.begin(); iter != parole.end(); iter++)
    {
      _banned.erase(*iter);
    }
  }

  if (isWhiteListed(source))
  {
    banned = false;
  }
  else
  {
    if (_denyAllIncoming)
    {
      banned = true;
    }
    else
    {
      banned = _banned.find(source) != _banned.end();
    }
    
    if (!banned)
    {
      banned = isBlackListed(source);
    }
    
  }
  _packetCounterMutex.unlock();

  return banned;
}

void AccessControl::getBannedAddresses(std::vector<boost::asio::ip::address>& banned)
{
  for (BannedSources::iterator iter = _banned.begin(); iter != _banned.end(); iter++)
  {
    banned.push_back(iter->first);
  }
}
void AccessControl::banAddress(const boost::asio::ip::address& source)
{
  if (!_enabled)
    return;
  
  _packetCounterMutex.lock();
  _banned[source] = boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time());
  if (_banCallback)
  {
    _banCallback(source);
  }
  _packetCounterMutex.unlock();
}

void AccessControl::clearAddress(const boost::asio::ip::address& source, bool addToWhiteList)
{
  if (!_enabled)
    return;
  _packetCounterMutex.lock();
  
  _banned.erase(source);
  _blackList.erase(source);

  if (addToWhiteList)
    whiteListAddress(source, false);
  _packetCounterMutex.unlock();
}


void AccessControl::whiteListAddress(const boost::asio::ip::address& address, bool removeFromBlackList)
{
  _packetCounterMutex.lock();
  
  OSS_LOG_NOTICE("AccessControl::whiteListAddress - " << address);
  
  if (removeFromBlackList)
    _banned.erase(address);

  _whiteList.insert(address);

  _packetCounterMutex.unlock();
}

void AccessControl::clearWhiteList(const boost::asio::ip::address& address)
{
  _packetCounterMutex.lock();
  _whiteList.erase(address);
  _packetCounterMutex.unlock();
}


void AccessControl::whiteListNetwork(const std::string& network)
{
  _packetCounterMutex.lock();
  
  OSS_LOG_NOTICE("AccessControl::whiteListNetwork - " << network);
  
   _networkWhiteList.insert(network);

  _packetCounterMutex.unlock();
}

void AccessControl::clearWhiteListNetwork(const std::string& network)
{
  _packetCounterMutex.lock();
  _networkWhiteList.erase(network);
  _packetCounterMutex.unlock();
}

bool AccessControl::isWhiteListed(const boost::asio::ip::address& address) const
{
  bool whiteListed = false;
 
  _packetCounterMutex.lock();

  whiteListed = _whiteList.find(address) != _whiteList.end();
  
  _packetCounterMutex.unlock();
  
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

  _packetCounterMutex.lock();
  for (std::set<std::string>::const_iterator iter = _networkWhiteList.begin();
    iter != _networkWhiteList.end(); iter++)
  {
    if (OSS::socket_address_cidr_verify(ipAddress, *iter))
    {
      _packetCounterMutex.unlock();
      return true;
    }
  }
  
  _packetCounterMutex.unlock();
  return false;
}


void AccessControl::blackListAddress(const boost::asio::ip::address& address, bool removeFromWhiteList)
{
  _packetCounterMutex.lock();
  
  OSS_LOG_NOTICE("AccessControl::blackListAddress - " << address);
  
  if (removeFromWhiteList)
    _whiteList.erase(address);

  _blackList.insert(address);

  _packetCounterMutex.unlock();
}

void AccessControl::blackListNetwork(const std::string& network)
{
  _packetCounterMutex.lock();
  
  OSS_LOG_NOTICE("AccessControl::blackListNetwork - " << network);
  
  _networkBlackList.insert(network);

  _packetCounterMutex.unlock();
}

bool AccessControl::isBlackListed(const boost::asio::ip::address& address) const
{
  bool blackListed = false;
 
  _packetCounterMutex.lock();

  blackListed = _blackList.find(address) != _blackList.end();
  
  _packetCounterMutex.unlock();
  
  if (!blackListed)
  {
    blackListed = isBlackListedNetwork(address);
  }
  
  if (blackListed && _banCallback)
  {
    _banCallback(address);
  }
  
  return blackListed;
}


bool AccessControl::isBlackListedNetwork(const boost::asio::ip::address& address) const
{
  boost::system::error_code ec;
  std::string ipAddress = address.to_string(ec);
  if (ec)
  {
    return false;
  }

  _packetCounterMutex.lock();
  for (std::set<std::string>::const_iterator iter = _networkBlackList.begin();
    iter != _networkBlackList.end(); iter++)
  {
    if (OSS::socket_address_cidr_verify(ipAddress, *iter))
    {
      _packetCounterMutex.unlock();
      return true;
    }
  }
  
  _packetCounterMutex.unlock();
  return false;
}

void AccessControl::clearNetwork(const std::string& cidr)
{
  _packetCounterMutex.lock();
  _networkBlackList.erase(cidr);
  _packetCounterMutex.unlock();
}


void AccessControl::denyAll(bool denyAll)
{
  OSS_LOG_NOTICE("AccessControl::denyAll set to " << (denyAll ? "true" : "false"));
  _denyAllIncoming = denyAll;
}


} } // OSS::SIP





