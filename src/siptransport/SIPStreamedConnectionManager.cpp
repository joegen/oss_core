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


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "OSS/SIP/SIPStreamedConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/UTL/Logger.h"

namespace OSS {
namespace SIP {


SIPStreamedConnectionManager::SIPStreamedConnectionManager(SIPFSMDispatch* pDispatch): 
  _pDispatch(pDispatch),
  _portBase(10000),
  _portMax(12000)
{
  _currentIdentifier = OSS::getTicks();
}

SIPStreamedConnectionManager::~SIPStreamedConnectionManager()
{
}

void SIPStreamedConnectionManager::initialize(const boost::filesystem::path& cfgDirectory)
{
}

void SIPStreamedConnectionManager::deinitialize()
{
}

void SIPStreamedConnectionManager::add(SIPStreamedConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  if (!conn->getIdentifier())
    conn->setIdentifier(++_currentIdentifier);
  _connections[conn->getIdentifier()] = conn;
  OSS_LOG_INFO("SIPStreamedConnectionManager Added transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );
}

void SIPStreamedConnectionManager::start(SIPStreamedConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  if (!conn->getIdentifier())
    conn->setIdentifier(++_currentIdentifier);
  _connections[conn->getIdentifier()] = conn;
  conn->start(_pDispatch);
  OSS_LOG_INFO("SIPStreamedConnectionManager started reading from transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );
}

void SIPStreamedConnectionManager::stop(SIPStreamedConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  OSS_LOG_INFO("Deleting SIPStreamedConnection transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );

  _connections.erase(conn->getIdentifier());
  conn->stop();
}

void SIPStreamedConnectionManager::stopAll()
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPStreamedConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    iter->second->stop();
  }
  _connections.clear();
}

SIPStreamedConnection::Ptr SIPStreamedConnectionManager::findConnectionByAddress(const OSS::Net::IPAddress& target)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPStreamedConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    if (iter->second->getRemoteAddress().compare(target, true))
      return iter->second;
  }
  return SIPStreamedConnection::Ptr();
}

SIPStreamedConnection::Ptr SIPStreamedConnectionManager::findConnectionById(OSS::UInt64 identifier)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  if (_connections.find(identifier) != _connections.end())
  {
    SIPStreamedConnection::Ptr conn = _connections[identifier];
    if (conn)
    {
      OSS_LOG_INFO("SIPStreamedConnectionManager::findConnectionById got transport (" << conn->getIdentifier() << ") "
      << conn->getLocalAddress().toIpPortString() <<
      "->" << conn->getRemoteAddress().toIpPortString() );
    }
    return conn;
  }
  return SIPStreamedConnection::Ptr();
}

} } // OSS::SIP


