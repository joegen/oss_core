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

#ifndef TLSMANAGER_H_INCLUDED
#define	TLSMANAGER_H_INCLUDED

#include "OSS/build.h"

#if ENABLE_FEATURE_NET_EXTRA

#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"


namespace OSS {
namespace Net {  
  
  
class TLSManager : boost::noncopyable
{
public:
  enum VerificationMode 
	{
		VERIFY_NONE, 
			/// Server: The server will not send a client certificate 
			/// request to the client, so the client will not send a certificate. 
			///
			/// Client: If not using an anonymous cipher (by default disabled), 
			/// the server will send a certificate which will be checked, but
			/// the result of the check will be ignored.
			
		VERIFY_RELAXED, 
			/// Server: The server sends a client certificate request to the 
			/// client. The certificate returned (if any) is checked. 
			/// If the verification process fails, the TLS/SSL handshake is 
			/// immediately terminated with an alert message containing the 
			/// reason for the verification failure. 
			///
			/// Client: The server certificate is verified, if one is provided. 
			/// If the verification process fails, the TLS/SSL handshake is
			/// immediately terminated with an alert message containing the 
			/// reason for the verification failure. 
			
		VERIFY_STRICT,
			/// Server: If the client did not return a certificate, the TLS/SSL 
			/// handshake is immediately terminated with a handshake failure
			/// alert. 
			///
			/// Client: Same as VERIFY_RELAXED. 
			
		VERIFY_ONCE
			/// Server: Only request a client certificate on the initial 
			/// TLS/SSL handshake. Do not ask for a client certificate 
			/// again in case of a renegotiation.
			///
			/// Client: Same as VERIFY_RELAXED.	
	};
  
  static TLSManager& instance();
  
  void initialize(
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode
    );
  
  void initializeServer(
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode);
  
  void initializeClient(
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode);
  
  void initialize(
    const std::string& privateKeyFile,
		const std::string& certificateFile,
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode);
  
  void initializeServer(
    const std::string& privateKeyFile,
		const std::string& certificateFile,
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode);
  
  void initializeClient(
    const std::string& privateKeyFile,
		const std::string& certificateFile,
    const std::string& caLocation,
    const std::string& passPhrase, 
    bool allowInvalidCert, 
    VerificationMode verificationMode);
  
protected:
  TLSManager(){};
  ~TLSManager(){};
  
};
  
//
// Inlines
//

inline void TLSManager::initialize(
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode
)
{
  initialize("", "", caLocation, passPhrase, allowInvalidCert, verificationMode);
}

inline void TLSManager::initialize(
  const std::string& privateKeyFile,
  const std::string& certificateFile,
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode)
{
  initializeClient("", "", caLocation, passPhrase, allowInvalidCert, verificationMode);
  initializeServer(privateKeyFile, certificateFile, caLocation, passPhrase, allowInvalidCert, verificationMode);
}

inline void TLSManager::initializeServer(
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode)
{
  initializeServer("", "", caLocation, passPhrase, allowInvalidCert, verificationMode);
}

inline void TLSManager::initializeClient(
  const std::string& caLocation,
  const std::string& passPhrase, 
  bool allowInvalidCert, 
  VerificationMode verificationMode)
{ 
  initializeClient("", "", caLocation, passPhrase, allowInvalidCert, verificationMode);
}
  

} }


#endif // ENABLE_FEATURE_NET_EXTRA
#endif	// TLSMANAGER_H_INCLUDED 

