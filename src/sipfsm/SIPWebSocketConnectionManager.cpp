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
#include "OSS/SIP/SIPWebSocketConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/Logger.h"

namespace OSS {
namespace SIP {


SIPWebSocketConnectionManager::SIPWebSocketConnectionManager(SIPFSMDispatch* pDispatch):
  _pDispatch(pDispatch),
  _portBase(10000),
  _portMax(12000)
{
}

SIPWebSocketConnectionManager::~SIPWebSocketConnectionManager()
{
}

void SIPWebSocketConnectionManager::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPWebSocketConnectionManager::deinitialize()
{

}

void SIPWebSocketConnectionManager::add(SIPWebSocketConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  SIPWebSocketConnection* pConnection = dynamic_cast<SIPWebSocketConnection*>(conn.get());
  OSS_VERIFY_NULL(pConnection);

  if (!pConnection->getIdentifier())
    pConnection->setIdentifier((OSS::UInt64)pConnection->_pServer.get());

  _connections[conn->getIdentifier()] = conn;
  OSS_LOG_INFO("SIPWebSocketConnection Added transport (" << pConnection->getIdentifier() << ") "
    << pConnection->getLocalAddress().toIpPortString() <<
    "->" << pConnection->getRemoteAddress().toIpPortString() );
}

void SIPWebSocketConnectionManager::start(SIPWebSocketConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  SIPWebSocketConnection* pConnection = dynamic_cast<SIPWebSocketConnection*>(conn.get());
  OSS_VERIFY_NULL(pConnection);

  if (!pConnection->getIdentifier())
    pConnection->setIdentifier((OSS::UInt64)pConnection->_pServer.get());

  _connections[conn->getIdentifier()] = conn;

  pConnection->start(_pDispatch);
  OSS_LOG_INFO("SIPWebSocketConnection started reading from transport (" << pConnection->getIdentifier() << ") "
    << pConnection->getLocalAddress().toIpPortString() <<
    "->" << pConnection->getRemoteAddress().toIpPortString() );
}

void SIPWebSocketConnectionManager::stop(SIPWebSocketConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  OSS_LOG_INFO("Deleting SIPWebSocketConnection transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );

  _connections.erase(conn->getIdentifier());
  conn->stop();
}

void SIPWebSocketConnectionManager::stopAll()
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPWebSocketConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    iter->second->stop();
  }
  _connections.clear();
}

SIPWebSocketConnection::Ptr SIPWebSocketConnectionManager::findConnectionByAddress(const OSS::IPAddress& target)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPWebSocketConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    if (iter->second->getRemoteAddress().compare(target, true))
      return iter->second;
  }
  return SIPWebSocketConnection::Ptr();
}

SIPWebSocketConnection::Ptr SIPWebSocketConnectionManager::findConnectionById(OSS::UInt64 identifier)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  if (_connections.find(identifier) != _connections.end())
  {
    SIPWebSocketConnection::Ptr conn = _connections[identifier];
    if (conn)
    {
      OSS_LOG_INFO("SIPWebSocketConnectionManager::findConnectionById got transport (" << conn->getIdentifier() << ") "
      << conn->getLocalAddress().toIpPortString() <<
      "->" << conn->getRemoteAddress().toIpPortString() );
    }
    return conn;
  }
  return SIPWebSocketConnection::Ptr();
}

} } // OSS::SIP


