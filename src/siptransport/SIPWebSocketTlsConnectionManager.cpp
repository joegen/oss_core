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
#include "OSS/SIP/SIPWebSocketTlsConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/UTL/Logger.h"

namespace OSS {
namespace SIP {


SIPWebSocketTlsConnectionManager::SIPWebSocketTlsConnectionManager(const SIPTransportSession::Dispatch& dispatch):
  _dispatch(dispatch),
  _portBase(10000),
  _portMax(12000)
{
}

SIPWebSocketTlsConnectionManager::~SIPWebSocketTlsConnectionManager()
{
}

void SIPWebSocketTlsConnectionManager::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPWebSocketTlsConnectionManager::deinitialize()
{

}

void SIPWebSocketTlsConnectionManager::add(SIPWebSocketTlsConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  SIPWebSocketTlsConnection* pConnection = dynamic_cast<SIPWebSocketTlsConnection*>(conn.get());
  OSS_VERIFY_NULL(pConnection);

  if (!pConnection->getIdentifier())
    pConnection->setIdentifier((OSS::UInt64)pConnection->_pServerConnection.get());

  _connections[conn->getIdentifier()] = conn;
  OSS_LOG_INFO("SIPWebSocketTlsConnection Added transport (" << pConnection->getIdentifier() << ") "
    << pConnection->getLocalAddress().toIpPortString() <<
    "->" << pConnection->getRemoteAddress().toIpPortString() );
}

void SIPWebSocketTlsConnectionManager::start(SIPWebSocketTlsConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  SIPWebSocketTlsConnection* pConnection = dynamic_cast<SIPWebSocketTlsConnection*>(conn.get());
  OSS_VERIFY_NULL(pConnection);

  if (!pConnection->getIdentifier())
    pConnection->setIdentifier((OSS::UInt64)pConnection->_pServerConnection.get());

  _connections[conn->getIdentifier()] = conn;

  pConnection->start(_dispatch);
  OSS_LOG_INFO("SIPWebSocketTlsConnection started reading from transport (" << pConnection->getIdentifier() << ") "
    << pConnection->getLocalAddress().toIpPortString() <<
    "->" << pConnection->getRemoteAddress().toIpPortString() );
}

void SIPWebSocketTlsConnectionManager::stop(SIPWebSocketTlsConnection::Ptr conn)
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);

  OSS_LOG_INFO("Deleting SIPWebSocketTlsConnection transport (" << conn->getIdentifier() << ") "
    << conn->getLocalAddress().toIpPortString() <<
    "->" << conn->getRemoteAddress().toIpPortString() );

  _connections.erase(conn->getIdentifier());
  conn->stop();
}

void SIPWebSocketTlsConnectionManager::stopAll()
{
  OSS::mutex_write_lock wlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPWebSocketTlsConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    iter->second->stop();
  }
  _connections.clear();
}

SIPWebSocketTlsConnection::Ptr SIPWebSocketTlsConnectionManager::findConnectionByAddress(const OSS::Net::IPAddress& target)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  for (std::map<OSS::UInt64, SIPWebSocketTlsConnection::Ptr>::iterator iter = _connections.begin();
    iter != _connections.end(); iter++)
  {
    if (iter->second->getRemoteAddress().compare(target, true))
      return iter->second;
  }
  return SIPWebSocketTlsConnection::Ptr();
}

SIPWebSocketTlsConnection::Ptr SIPWebSocketTlsConnectionManager::findConnectionById(OSS::UInt64 identifier)
{
  OSS::mutex_read_lock rlock(_rwConnectionsMutex);
  if (_connections.find(identifier) != _connections.end())
  {
    SIPWebSocketTlsConnection::Ptr conn = _connections[identifier];
    if (conn)
    {
      OSS_LOG_INFO("SIPWebSocketTlsConnectionManager::findConnectionById got transport (" << conn->getIdentifier() << ") "
      << conn->getLocalAddress().toIpPortString() <<
      "->" << conn->getRemoteAddress().toIpPortString() );
    }
    return conn;
  }
  return SIPWebSocketTlsConnection::Ptr();
}

} } // OSS::SIP


