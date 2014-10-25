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


#ifdef _MSC_VER
  #pragma warning(disable:4267)  // conversion from 'size_t' to 'int', possible loss of data
#endif


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "OSS/SIP/SIPTLSConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {


SIPTLSConnectionManager::SIPTLSConnectionManager(SIPFSMDispatch* pDispatch): 
  _pDispatch(pDispatch)
{
}

SIPTLSConnectionManager::~SIPTLSConnectionManager()
{
}

void SIPTLSConnectionManager::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPTLSConnectionManager::deinitialize()
{

}

void SIPTLSConnectionManager::add(SIPTLSConnection::Ptr conn)
{
  _connections.insert(conn);
}

void SIPTLSConnectionManager::start(SIPTLSConnection::Ptr conn)
{
  _connections.insert(conn);
  conn->start(_pDispatch);
}

void SIPTLSConnectionManager::stop(SIPTLSConnection::Ptr conn)
{
  _connections.erase(conn);
  conn->stop();
}

void SIPTLSConnectionManager::stopAll()
{
  std::for_each(_connections.begin(), _connections.end(),
      boost::bind(&SIPTransportSession::stop, _1));
  _connections.clear();
}


} } // OSS::SIP


