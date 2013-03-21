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
//


#include "OSS/STUN/STUNServer.h"


struct NullStream:
  std::ostream {
  struct nullbuf: std::streambuf {
    int overflow(int c) { return traits_type::not_eof(c); }
  } m_sbuf;
  NullStream(): std::ios(&m_sbuf), std::ostream(&m_sbuf) {}
};
static NullStream null_stream;
#define clog null_stream

#include "OSS/STUN/VovidaUDP.h"
#include "OSS/STUN/VovidaUDP.inl"
#include "OSS/STUN/VovidaSTUN.h"
#include "OSS/STUN/VovidaSTUN.inl"

#undef clog
#define clog clog


namespace OSS {
namespace STUN {


STUNServer::STUNServer() : 
  _config(0),
  _exitSync(0, 0xFFFF)
{

}

STUNServer::~STUNServer()
{

}

bool STUNServer::initialize(
  const OSS::IPAddress& primary,
  const OSS::IPAddress& secondary)
{
  _primaryIp = primary;
  _secondaryIp = secondary;
  return true;
}

void STUNServer::run()
{
  OSS::thread_pool::static_schedule(boost::bind(&STUNServer::internal_run, this));
}

void STUNServer::internal_run()
{
  StunAddress4 primaryAddr;
  StunAddress4 secondaryAddr;

  stunParseServerName(_primaryIp.toString().c_str(), primaryAddr);
  stunParseServerName(_secondaryIp.toString().c_str(), secondaryAddr);

  primaryAddr.port = _primaryIp.getPort();
  secondaryAddr.port = _secondaryIp.getPort();

  if (primaryAddr.port == 0)
    primaryAddr.port = STUN_PORT;

  if (secondaryAddr.port == 0)
    secondaryAddr.port = STUN_PORT + 1;

  StunServerInfo* pInfo = new StunServerInfo();
  _config = pInfo;

  if (stunInitServer(*pInfo, primaryAddr, secondaryAddr, 0, 0))
  {
    while (stunServerProcess(*pInfo, 0));
  }
  _exitSync.set();
}

void STUNServer::stop()
{
  StunServerInfo* pInfo = static_cast<StunServerInfo*>(_config);
  if (pInfo)
  {
    stunStopServer(*pInfo);
    delete pInfo;
    _exitSync.wait(10000);
  }
}



} } // OSS::STUN