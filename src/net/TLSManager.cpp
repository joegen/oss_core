
/*
 * Copyright (C) OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <Poco/Net/PrivateKeyPassphraseHandler.h>

#include "Poco/Net/SSLManager.h"
#include "OSS/Net/TLSManager.h"


namespace OSS {
namespace Net {  
  

#define TLS_DEFAULT_CIPHER_LIST "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
  

class Passphrase : public Poco::Net::PrivateKeyPassphraseHandler
{
public:
  Passphrase(bool onServerSide, const std::string& passPhrase) :
    Poco::Net::PrivateKeyPassphraseHandler(onServerSide),
    _passPhrase(passPhrase)
  {
  }
  
  void onPrivateKeyRequested(const void* pSender, std::string& privateKey)
  {
    privateKey = _passPhrase;
  }
  
  std::string _passPhrase;
};

class AllowInvalidCert : public Poco::Net::InvalidCertificateHandler
{
public:
  AllowInvalidCert(bool onServerSize, bool allow) :
    Poco::Net::InvalidCertificateHandler(onServerSize),
    _allow(allow)
  {
  }
    
  void onInvalidCertificate(const void* pSender, Poco::Net::VerificationErrorArgs& errorCert)
  {
    errorCert.setIgnoreError(_allow);
  }
  
  bool _allow;
};
  
TLSManager& TLSManager::instance()
{
  static TLSManager _gInstance;
  static bool _gInitialized = false;
  
  if (!_gInitialized)
  {
    Poco::Net::initializeSSL();
    _gInitialized = true;
  }
  
  return _gInstance;
}

Poco::Net::Context::VerificationMode convert_mode(TLSManager::VerificationMode mode)
{
  switch (mode)
  {
    case TLSManager::VERIFY_NONE:
      return Poco::Net::Context::VERIFY_NONE;
    case TLSManager::VERIFY_RELAXED:
      return Poco::Net::Context::VERIFY_RELAXED;
    case TLSManager::VERIFY_STRICT:
      return Poco::Net::Context::VERIFY_STRICT;
    case TLSManager::VERIFY_ONCE:
      return Poco::Net::Context::VERIFY_ONCE;
    default:
      return Poco::Net::Context::VERIFY_RELAXED;
  } 
}

void TLSManager::initializeServer(
  const std::string& privateKeyFile,
  const std::string& certificateFile,
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode)
{
  static Poco::Net::Context::Ptr _gpContext;
  if (_gpContext)
    return;
  
  if (!privateKeyFile.empty() && !certificateFile.empty())
    _gpContext = new Poco::Net::Context(Poco::Net::Context::SERVER_USE, privateKeyFile, certificateFile, caLocation, convert_mode(verificationMode), 9, false, TLS_DEFAULT_CIPHER_LIST);
  else
    _gpContext = new Poco::Net::Context(Poco::Net::Context::SERVER_USE, caLocation, convert_mode(verificationMode), 9, false, TLS_DEFAULT_CIPHER_LIST);
  static Poco::SharedPtr<Poco::Net::PrivateKeyPassphraseHandler> _gpPassphrase = new Passphrase(true, passPhrase);
	static Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> _gpAllowInvalidCert = new AllowInvalidCert(true, allowInvalidCert);
  Poco::Net::SSLManager::instance().initializeServer(_gpPassphrase, _gpAllowInvalidCert, _gpContext);
}

void TLSManager::initializeClient(
  const std::string& privateKeyFile,
  const std::string& certificateFile,
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode)
{
  static Poco::Net::Context::Ptr _gpContext;
  if (_gpContext)
    return;
  
  if (!privateKeyFile.empty() && !certificateFile.empty())
    _gpContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, privateKeyFile, certificateFile, caLocation, convert_mode(verificationMode), 9, false, TLS_DEFAULT_CIPHER_LIST);
  else
    _gpContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, caLocation, convert_mode(verificationMode), 9, false, TLS_DEFAULT_CIPHER_LIST);
  static Poco::SharedPtr<Poco::Net::PrivateKeyPassphraseHandler> _gpPassphrase = new Passphrase(false, passPhrase);
	static Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> _gpAllowInvalidCert = new AllowInvalidCert(false, allowInvalidCert);
  Poco::Net::SSLManager::instance().initializeClient(_gpPassphrase, _gpAllowInvalidCert, _gpContext);
}


} }



