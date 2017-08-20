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


#ifndef OSS_TLSCONTEXT_H_INCLUDED
#define OSS_TLSCONTEXT_H_INCLUDED

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/Net/Net.h"
#include "OSS/UTL/Logger.h"


namespace OSS  {
namespace Net {


class TlsContext
{
public:
  typedef boost::shared_ptr<boost::asio::ssl::context> Context;
  
  TlsContext();
  ~TlsContext();
  
  bool initialize(
    boost::asio::io_service* pIoService,
    bool verifyPeer,
    const std::string& peerCaFile, // can be empty
    const std::string& caDirectory, // can be empty
    const std::string& certPassword, // can be empty
    const std::string& certFile, // required
    const std::string& privateKey // required
  );
  
  const Context& getServerContext() const;
  const Context& getClientContext() const;
  
  std::string getTlsCertPassword() const;
protected:
  boost::asio::io_service* _pIoService;
  Context _pClientContext;
  Context _pServerContext;
  std::string _certPassword;
  
};

//
// Inlines
//

inline TlsContext::TlsContext() :
  _pIoService(0)
{

}

inline TlsContext::~TlsContext()
{
}

inline bool TlsContext::initialize(
  boost::asio::io_service* pIoService,
  bool verifyPeer,
  const std::string& peerCaFile, // can be empty
  const std::string& caDirectory, // can be empty
  const std::string& certPassword, // can be empty
  const std::string& certFile, // required
  const std::string& privateKey // required
)
{
  if (_pIoService || _pServerContext || _pClientContext)
  {
    OSS_LOG_ERROR("TlsContext::initialize - Not initializing TLS context.  Context are already initialized.");
    return false;
  }
  
  OSS_LOG_DEBUG("TlsContext::initialize - Certificate: " << certFile << " Private Key: " << privateKey << " Verify Peer: " << verifyPeer );
  
  _pIoService = pIoService;
  _pServerContext = Context(new boost::asio::ssl::context(*pIoService, boost::asio::ssl::context::sslv23_server));
  _pClientContext = Context(new boost::asio::ssl::context(*pIoService, boost::asio::ssl::context::sslv23_client));
  
  if (verifyPeer)
  {
    _pClientContext->set_verify_mode(boost::asio::ssl::context::verify_none);
    _pServerContext->set_verify_mode(boost::asio::ssl::context::verify_peer | boost::asio::ssl::context::verify_fail_if_no_peer_cert);
  }
  else
  {
    _pClientContext->set_verify_mode(boost::asio::ssl::context::verify_none);
    _pServerContext->set_verify_mode(boost::asio::ssl::context::verify_none);
  }
  
  if (!peerCaFile.empty())
  {
    try
    {
      _pClientContext->load_verify_file(peerCaFile);
      _pServerContext->load_verify_file(peerCaFile);
    }
    catch(...)
    {
      OSS_LOG_WARNING("TlsContext::initialize - Unable to load peerCaFile " << peerCaFile << " TLS will be disabled.");
      return false;
    }
  }

  if (!caDirectory.empty())
  {
    try
    {
      _pClientContext->add_verify_path(caDirectory);
      _pServerContext->add_verify_path(caDirectory);
    }
    catch(...)
    {
      OSS_LOG_ERROR("TlsContext::initialize - Unable to add peerCaPath " << caDirectory << " TLS will be disabled.");
      return false;
    }
  }

  _certPassword = certPassword;
  
  try
  {
    _pClientContext->set_password_callback(boost::bind(&TlsContext::getTlsCertPassword, this));
    _pClientContext->use_certificate_file(certFile, boost::asio::ssl::context::pem);
    _pClientContext->use_private_key_file(privateKey, boost::asio::ssl::context::pem);
    
    _pServerContext->set_password_callback(boost::bind(&TlsContext::getTlsCertPassword, this));
    _pServerContext->use_certificate_file(certFile, boost::asio::ssl::context::pem);
    _pServerContext->use_private_key_file(privateKey, boost::asio::ssl::context::pem);
  }
  catch(...)
  {
    OSS_LOG_ERROR("TlsContext::initialize - Unable to add tlsCertFile " << certFile << " TLS will be disabled.");
    return false;
  }

  return true;
}

inline const TlsContext::Context& TlsContext::getServerContext() const
{
  return _pServerContext;
}

inline const TlsContext::Context& TlsContext::getClientContext() const
{
  return _pClientContext;
}

inline std::string TlsContext::getTlsCertPassword() const
{
  return _certPassword;
}


} }

#endif //OSS_TLSCONTEXT_H_INCLUDED

