
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

#include <stdio.h>
#include <openssl/e_os2.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/bn.h>

#include "OSS/Net/DTLSContext.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace Net {

  
//
// Static members
//  
DTLSContext* DTLSContext::_pInstance = 0;
X509* DTLSContext::_pX509 = 0;
EVP_PKEY* DTLSContext::_pPrivateKey = 0;
SSL_CTX* DTLSContext::_pSSLContext = 0;
bool DTLSContext::_verfifyCerts = false;

//
// Constants
//
static const int DEFAULT_CONTEXT_EXPIRE = 365; /// Our self signed cert lasts for a year
static const int MAX_KEY_LENGTH = 1024;
static const char* DEFAULT_SRTP_CIPHER = "SRTP_AES128_CM_SHA1_80";
static const char* DEFAULT_CIPHER_LIST = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
  
//
// X509 *PEM_read_X509(FILE *fp, X509 **x, pem_password_cb *cb, void *u);
//
// The first argument is a pointer to a FILE object returned by fopen(). 
// You can leave the last three arguments as NULL. 
// This function returns a pointer to the X509 object created, or NULL upon error. 
//

static EVP_PKEY* generate_key()
{
  EVP_PKEY* pKey = EVP_PKEY_new();
  RSA* pRsa = RSA_new();
  BIGNUM* e = BN_new();
  int ret = 0;
  unsigned long  rsa_f4 = RSA_F4;
  
  if (!pKey || !pRsa)
  {
    goto freeobjects;
  }
  
  ret = BN_set_word(e, rsa_f4);
  if (!ret)
  {
    goto freeobjects;
  }
  
  //
  // Generate the key pair and store it in pRsa
  //
  ret = RSA_generate_key_ex(pRsa, MAX_KEY_LENGTH, e, 0);
  if (!ret)
  {
    goto freeobjects;
  }
  //
  // set the key referenced by pRsa to pKey
  //
  ret = EVP_PKEY_set1_RSA(pKey, pRsa);
  if (!ret)
  {
    goto freeobjects;
  }
  
  return pKey;
  
freeobjects:
  
  if (pRsa)
  {
    RSA_free(pRsa);
  }

  if (pKey)
  {
    EVP_PKEY_free(pKey);
  }

  if (e)
  {
    BN_free(e);
  }
  return 0;
}

//
// Generate a self signed certificate
//
static X509* generate_cert(const std::string& identity, EVP_PKEY* pKey)
{
  int ret = 0;
  X509* pCert = X509_new();
  X509_NAME* pSubject = X509_NAME_new();
  X509_EXTENSION* pExtension = 0;
  std::string altIdentity = std::string("URI:sip:") + identity;
  char basicConstraints[] = "CA:FALSE"; 
  
  if (!pCert)
  {
    goto freeobjects;
  }
  
  //
  // Set a random serial number
  //
  X509_set_version(pCert,2);
	ASN1_INTEGER_set(X509_get_serialNumber(pCert), OSS::getRandom());
  //
  // Set the expiration
  //
	X509_gmtime_adj(X509_get_notBefore(pCert),0);
	X509_gmtime_adj(X509_get_notAfter(pCert),(long)60*60*24*DEFAULT_CONTEXT_EXPIRE);
  //
  // Attach our key to this cert
  //
	ret = X509_set_pubkey(pCert,pKey);
  if (!ret)
  {
    goto freeobjects;
  }
  //
  // Assign our identity as the CN
  //
  ret = X509_NAME_add_entry_by_txt( pSubject, "CN", MBSTRING_ASC, (unsigned char *) identity.data(), identity.size(), -1, 0);
  if (!ret)
  {
    goto freeobjects;
  }
  ret = X509_set_issuer_name(pCert, pSubject);
  if (!ret)
  {
    goto freeobjects;
  }
  ret = X509_set_subject_name(pCert, pSubject);
  //
  // Assign a URI alternative name extension 
  //
  pExtension = X509V3_EXT_conf_nid(0 , 0 , NID_subject_alt_name, (char*) altIdentity.c_str());
  X509_add_ext(pCert, pExtension, -1);
  X509_EXTENSION_free(pExtension);
  //
  // Add a basic constraint indicating that this certificate is not a CA
  //
  pExtension = X509V3_EXT_conf_nid(0, 0, NID_basic_constraints, basicConstraints);
  ret = X509_add_ext(pCert, pExtension, -1);
  X509_EXTENSION_free(pExtension);
  //
  // Now sign it using SHA1
  //
  ret = X509_sign(pCert, pKey, EVP_sha1());
  if (!ret)
  {
    goto freeobjects;
  }
  return pCert;
  
freeobjects:
  X509_free(pCert);
  X509_NAME_free(pSubject);
  return 0;
}

static int ssl_verification_cb(int preverify_ok, X509_STORE_CTX* ctx)
{
  char buf[512];
  int ret = 0;
  int err = 0;
  
  err = X509_STORE_CTX_get_error(ctx);
  ret = (err == X509_V_OK);
  if (ret && !preverify_ok)
  {
    //
    // Not sure if this can happen.  but just in case there is no error but 
    // preverify_ok is not true, we set ret to 0;
    //
    ret = 0;
  }
  
  if (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT)
  {
    //
    // Just issue a warning.  Let DTLSContext::willVerifyCerts() decide
    //
    X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
    OSS_LOG_WARNING("DTLSContext: Cert " << buf << "Error: X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT");
  }
  else if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT || err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
  {
    //
    // We allow self signed certs for RTP because it is a peer to peer connection regardless
    // of the status of DTLSContext::willVerifyCerts()
    //
    ret = 1;
  }
  
  if (!preverify_ok && !DTLSContext::willVerifyCerts())
  {
    ret = 1;
  }
  else if (!ret && !DTLSContext::willVerifyCerts())
  {
    ret = 1;
  }
  
  return ret;
}

static SSL_CTX* prepare_context(SSL_CTX* pSSLContext)
{
  //
  // Set verification mode
  //
  SSL_CTX_set_verify_depth (pSSLContext, 2);  
  SSL_CTX_set_verify(pSSLContext, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, ssl_verification_cb);
  //
  // Set the default cipher for srtp extension
  //
  SSL_CTX_set_tlsext_use_srtp(pSSLContext, DEFAULT_SRTP_CIPHER);
  //
  // Add all known Ciphers
  //
  SSL_CTX_set_cipher_list(pSSLContext, DEFAULT_CIPHER_LIST);
  //
  // Set read-ahead to true
  //
  SSL_CTX_set_read_ahead(pSSLContext, 1);
  
  return pSSLContext;
}

static SSL_CTX* generate_context(X509* pX509, EVP_PKEY* pKey)
{
  int ret;
  SSL_CTX* pSSLContext = SSL_CTX_new(DTLSv1_method());
  if (!pSSLContext)
  {
    return 0;
  }
  
  ret = SSL_CTX_use_certificate(pSSLContext, pX509);
  if (!ret)
  {
    SSL_CTX_free(pSSLContext);
    return 0;
  }
  
  ret = SSL_CTX_use_PrivateKey(pSSLContext, pKey);
  if (!ret)
  {
    SSL_CTX_free(pSSLContext);
    return 0;
  }
  
  return prepare_context(pSSLContext);
}

static SSL_CTX* generate_context(const std::string& x509File, const std::string& privateKeyfile, const char* password)
{
  int ret;
  SSL_CTX* pSSLContext = SSL_CTX_new(DTLSv1_method());
  if (!pSSLContext)
  {
    return 0;
  }
  
  ret = SSL_CTX_use_certificate_file(pSSLContext, x509File.c_str(), SSL_FILETYPE_PEM);
  if (!ret)
  {
    SSL_CTX_free(pSSLContext);
    return 0;
  }
  
  ret = SSL_CTX_use_PrivateKey_file(pSSLContext, privateKeyfile.c_str(), SSL_FILETYPE_PEM);
  if (!ret)
  {
    SSL_CTX_free(pSSLContext);
    return 0;
  }
  
  ret = SSL_CTX_check_private_key(pSSLContext);
  if (!ret)
  {
    SSL_CTX_free(pSSLContext);
    return 0;
  }
  
  return prepare_context(pSSLContext);
}

bool DTLSContext::initialize(const std::string& x509File, const std::string& privateKeyfile, const char* password, bool verifyCerts)
{
  assert(!_pPrivateKey);
  assert(!_pX509);
  assert(!_pSSLContext);
  
  DTLSContext::_verfifyCerts = verifyCerts;
  
  //
  // Note that SSL init functions are not reentrant
  // Make sure that this is done in the same thread as all other components
  // using openssl
  //
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_crypto_strings();
  
  _pSSLContext = generate_context(x509File, privateKeyfile, password);
 
  return _pSSLContext != 0;
}
 
bool DTLSContext::initialize(const std::string& identity, bool verifyCerts)
{
  assert(!_pPrivateKey);
  assert(!_pX509);
  assert(!_pSSLContext);
  
  DTLSContext::_verfifyCerts = verifyCerts;
  
  //
  // Note that SSL init functions are not reentrant
  // Make sure that this is done in the same thread as all other components
  // using openssl
  //
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_crypto_strings();
    
  _pPrivateKey = generate_key();
  if (!_pPrivateKey)
    return false;
  
  _pX509 = generate_cert(identity, _pPrivateKey);
  if (!_pX509)
  {
    EVP_PKEY_free(_pPrivateKey);
    return false;
  }
  
  _pSSLContext = generate_context(_pX509, _pPrivateKey);
  if (!_pSSLContext)
  {
    EVP_PKEY_free(_pPrivateKey);
    X509_free(_pX509);
    return false;
  }
  
  return true;
}

DTLSContext* DTLSContext::instance()
{
  if (!_pInstance)
  {
    _pInstance = new DTLSContext();
  }
  return DTLSContext::_pInstance;
}
 
void DTLSContext::releaseInstance()
{
  delete DTLSContext::_pInstance;
  DTLSContext::_pInstance = 0;
  SSL_CTX_free(DTLSContext::_pSSLContext);
  DTLSContext::_pSSLContext = 0;
  DTLSContext::_pX509 = 0;
  DTLSContext::_pPrivateKey = 0;
}


DTLSContext::DTLSContext()
{
}

DTLSContext::~DTLSContext()
{
}
  
X509& DTLSContext::x509Cert()
{
  if (!_pX509)
  {
    throw OSS::IllegalStateException();
  }
  return *_pX509;
}
  
const X509& DTLSContext::x509Cert() const
{
  if (!_pX509)
  {
    throw OSS::IllegalStateException();
  }
  return *_pX509;
}

EVP_PKEY& DTLSContext::privateKey()
{
  if (!_pPrivateKey)
  {
    throw OSS::IllegalStateException();
  }
  return *_pPrivateKey;
}

const EVP_PKEY& DTLSContext::privateKey() const
{
  if (!_pPrivateKey)
  {
    throw OSS::IllegalStateException();
  }
  return *_pPrivateKey;
}
  
SSL_CTX& DTLSContext::sslContext()
{
  if (!_pSSLContext)
  {
    throw OSS::IllegalStateException();
  }
  return *_pSSLContext;
}

const SSL_CTX& DTLSContext::sslContext() const
{
  if (!_pSSLContext)
  {
    throw OSS::IllegalStateException();
  }
  return *_pSSLContext;
}

} } // OSS::RTP



