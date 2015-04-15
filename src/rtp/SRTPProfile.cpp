
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


#include "OSS/RTP/SRTPProfile.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace RTP {  

  
static const int SRTP_MASTER_KEY_KEY_LEN = 16;
static const int SRTP_MASTER_KEY_SALT_LEN = 14;  
static const char* SRTP_LABEL = "EXTRACTOR-dtls_srtp";

SRTPProfile::SRTPProfile() :
  _isValid(false)
{
  
}
  
SRTPProfile::~SRTPProfile()
{
}

static bool generate_fingerprint(X509* pCert, std::string& fingerprint)
{
  if (!pCert)
  {
    OSS_LOG_ERROR("SRTPProfile::generate_fingerprint - Unable to create profile.  Remote Certificate is NULL.");
    return false;
  }
  
  int ret = 0;
  unsigned char sha[EVP_MAX_MD_SIZE];
  unsigned int shaLen = 0;
  ret = X509_digest(pCert, EVP_sha256(), sha, &shaLen);
  
  if (!ret)
  {
    OSS_LOG_ERROR("SRTPProfile::generate_fingerprint - Unable to create profile.  Unable to retrieve SHA fingerprint.");
    return false;
  }
  
  std::ostringstream rstrm;
  
  for (unsigned int i = 0; i < shaLen; i++)
  {
    rstrm << std::setfill('0') << std::hex << std::setw(2)
       << (unsigned int)sha[i]
       << std::setfill(' ') << std::dec;
    
    if(i < (shaLen - 1))
    {
      rstrm << ":";
    }
  }
  
  fingerprint = rstrm.str();
  return true;
}

bool SRTPProfile::create(OSS::Net::DTLSSession& session)
{
  if (!session.isConnected())
  {
    OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Session is not connected.");
    return false;
  }
  
  if (!session.ssl())
  {
    OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  SSL handle is NULL.");
    return false;
  }
  
  //
  // Generate local fingerprint
  //
  if (!generate_fingerprint(&OSS::Net::DTLSContext::instance()->x509Cert(), _localFingerPrint))
  {
    OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Unknown local fingerprint.");
    return false;
  }
  
  //
  // Generate the remote fingerprint
  //
  if (!generate_fingerprint(SSL_get_peer_certificate(session.ssl()), _remoteFingerPrint))
  {
    OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Unknown remote fingerprint.");
    return false;
  }
  
  //
  // Generate the key and salt
  //
  unsigned char dtls_buffer[SRTP_MASTER_KEY_KEY_LEN * 2 + SRTP_MASTER_KEY_SALT_LEN * 2];
  int ret = SSL_export_keying_material(session.ssl(), 
                                    dtls_buffer, 
                                    sizeof(dtls_buffer),
                                    SRTP_LABEL, 
                                    strlen(SRTP_LABEL),
                                    NULL,
                                    0, 
                                    0);
  
  _isValid = !!ret; // double negation to covert to bool
  
  if (_isValid)
  {
    char client_write_key_key[SRTP_MASTER_KEY_KEY_LEN];
    char server_write_key_key[SRTP_MASTER_KEY_KEY_LEN];
    char client_write_key_salt[SRTP_MASTER_KEY_SALT_LEN];
    char server_write_key_salt[SRTP_MASTER_KEY_SALT_LEN];
    size_t offset = 0;

    memcpy(&client_write_key_key[0], &dtls_buffer[offset], SRTP_MASTER_KEY_KEY_LEN);
    offset += SRTP_MASTER_KEY_KEY_LEN;
    memcpy(&server_write_key_key[0], &dtls_buffer[offset], SRTP_MASTER_KEY_KEY_LEN);
    offset += SRTP_MASTER_KEY_KEY_LEN;
    memcpy(&client_write_key_salt[0], &dtls_buffer[offset], SRTP_MASTER_KEY_SALT_LEN);
    offset += SRTP_MASTER_KEY_SALT_LEN;
    memcpy(&server_write_key_salt[0], &dtls_buffer[offset], SRTP_MASTER_KEY_SALT_LEN); 

    _clientMasterKey = std::string(&client_write_key_key[0], SRTP_MASTER_KEY_KEY_LEN);
    _clientMasterSalt = std::string(&client_write_key_salt[0], SRTP_MASTER_KEY_SALT_LEN);
    _serverMasterKey = std::string(&server_write_key_key[0], SRTP_MASTER_KEY_KEY_LEN);
    _serverMasterSalt = std::string(&server_write_key_salt[0], SRTP_MASTER_KEY_SALT_LEN);
    
    //
    // Initialize the policy
    //
    SRTP_PROTECTION_PROFILE* pProfile = SSL_get_selected_srtp_profile(session.ssl());
    if (!pProfile)
    {
      OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Unable to retrieve SRTP profile.");
      _isValid = false;
      return _isValid;
    }
    
    switch( pProfile->id )
    {
    case SRTP_AES128_CM_SHA1_80:
      crypto_policy_set_aes_cm_128_hmac_sha1_80(&_policy.rtp);
      crypto_policy_set_aes_cm_128_hmac_sha1_80(&_policy.rtcp);
      break;
    case SRTP_AES128_CM_SHA1_32:
      crypto_policy_set_aes_cm_128_hmac_sha1_32(&_policy.rtp);   
      crypto_policy_set_aes_cm_128_hmac_sha1_80(&_policy.rtcp);  
      break;
    default:
      OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Unable to determine crypto policy.");
      _isValid = false;
      return false;
    }
    
    _policy.ssrc.value = 0;
    _policy.next = 0;
    _policy.ssrc.type = ssrc_any_outbound;
    
    unsigned char policy_key[SRTP_MASTER_KEY_KEY_LEN +SRTP_MASTER_KEY_SALT_LEN];
    memcpy(&policy_key[0], &client_write_key_key[0], SRTP_MASTER_KEY_SALT_LEN); 
    memcpy(&policy_key[SRTP_MASTER_KEY_SALT_LEN], &client_write_key_salt[0], SRTP_MASTER_KEY_SALT_LEN);
    
    _policy.key = policy_key;    
  }
  else
  {
    OSS_LOG_ERROR("SRTPProfile::create - Unable to create profile.  Unable to export keying material.");
  }
  
  return _isValid;
}

} } // OSS::RTP

