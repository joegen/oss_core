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


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "OSS/SIP/SIPTCPConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/Logger.h"

namespace OSS {
namespace SIP {


SIPTCPConnectionManager::SIPTCPConnectionManager(SIPFSMDispatch* pDispatch): 
  _pDispatch(pDispatch),
  _portBase(10000),
  _portMax(12000)
{
  _currentIdentifier = OSS::getTicks();
}

SIPTCPConnectionManager::~SIPTCPConnectionManager()
{
}

void SIPTCPConnectionManager::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPTCPConnectionManager::deinitialize()
{

}

void SIPTCPConnectionManager::add(SIPTCPConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  if (!conn->getIdentifier())
    conn->setIdentifier(++_currentIdentifier);
  _connections[conn->getIdentifier()] = conn;
  OSS_LOG_INFO("SIPTCPConnection Added transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );
}

void SIPTCPConnectionManager::start(SIPTCPConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  if (!conn->getIdentifier())
    conn->setIdentifier(++_currentIdentifier);
  _connections[conn->getIdentifier()] = conn;
  conn->start(_pDispatch);
  OSS_LOG_INFO("SIPTCPConnection started reading from transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );
}

void SIPTCPConnectionManager::stop(SIPTCPConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  OSS_LOG_INFO("Deleting SIPTCPConnection transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );

  _connections.erase(conn->getIdentifier());
  conn->stop();
}

void SIPTCPConnectionManager::stopAll()
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPTCPConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    iter->second->stop();
  }
  _connections.clear();
}

SIPTCPConnection::Ptr SIPTCPConnectionManager::findConnectionByAddress(const OSS::IPAddress& target)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPTCPConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    if (iter->second->getRemoteAddress().compare(target, true))
      return iter->second;
  }
  return SIPTCPConnection::Ptr();
}

SIPTCPConnection::Ptr SIPTCPConnectionManager::findConnectionById(OSS::UInt64 identifier)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  if (_connections.find(identifier) != _connections.end())
  {
    SIPTCPConnection::Ptr conn = _connections[identifier];
    if (conn)
    {
      OSS_LOG_INFO("SIPTCPConnectionManager::findConnectionById got transport (" << conn->getIdentifier() << ") "
      << conn->getLocalAddress().toIpPortString() <<
      "->" << conn->getRemoteAddress().toIpPortString() );
    }
    return conn;
  }
  return SIPTCPConnection::Ptr();
}

} } // OSS::SIP


