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

#ifndef OSS_RTPDTLSCONTEXT_H_INCLUDED
#define	OSS_RTPDTLSCONTEXT_H_INCLUDED


#include <openssl/evp.h>
#include "OSS/UTL/Exception.h"

namespace OSS {
namespace RTP {

  
class RTPDTLSContext
  //
  // singleton DTLS context instance with support for the SRTP extension
  //
{
public:
  static bool initialize(const std::string& x509File, const std::string& privateKeyFile, const char* password, bool verifyCerts);
  /// Initialize the context from a certificate in PEM format.   
  /// If password protected, the 3rd parameter must be set.
  /// The 4th option enable or disable certificate verification
  /// Otherwise, second parameter should be set to 0 if password is not used.
  ///
  
  static bool initialize(const std::string& identity, bool verifyCerts);
  /// Initialize using a new self signed certificate
  /// The second option enable or disable certificate verification
  ///
  
  static RTPDTLSContext* instance();
  /// Returns the singleton instance of this context.
  /// This will return NULL if context is not initialized
  /// 
 
  static void releaseInstance();
  /// Deletes the singleton instance of this context
  ///
  
  X509& x509Cert();
  const X509& x509Cert() const;
  /// Returns a reference to the x509 structure when using self signed certificate.
  /// This will throw OSS::IllegalStateException if the context is not initialized
  ///
  
  EVP_PKEY& privateKey();
  const EVP_PKEY& privateKey() const;
  /// Returns a reference to the EVP private key structure when using self signed certificate.
  /// This will throw OSS::IllegalStateException if the context is not initialized
  ///
  
  SSL_CTX& sslContext();
  const SSL_CTX& sslContext() const;
  /// Returns a reference to the SSL Context structure
  /// This will throw OSS::IllegalStateException if the context is not initialized
  ///
  
  static bool willVerifyCerts();
  /// Returns true if certificate verification is enabled
  ///
protected:
  RTPDTLSContext();
  ~RTPDTLSContext();
  
private:
  static RTPDTLSContext* _pInstance;
  static X509* _pX509;
  static EVP_PKEY* _pPrivateKey;
  static SSL_CTX* _pSSLContext;
  static bool _verfifyCerts;
};

//
// Inlines
//

inline bool RTPDTLSContext::willVerifyCerts()
{
  return RTPDTLSContext::_verfifyCerts;
}

} } // OSS::RTP

#endif	// OSS_RTPDTLSCONTEXT_H_INCLUDED

