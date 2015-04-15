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

#ifndef OSS_SRTPPROFILE_H_INCLUDED
#define	OSS_SRTPPROFILE_H_INCLUDED

#include <srtp/srtp.h>
#include "OSS/Net/DTLSSession.h"


namespace OSS {
namespace RTP {  


class SRTPProfile
{  
  // 
  // DTLS handshake is used to establish keying material,
  // algorithms, and parameters for SRTP
  //
public:
  SRTPProfile();
  
  ~SRTPProfile();
  
  bool create(OSS::Net::DTLSSession& session);
  
  const std::string& getLocalFingerPrint() const;
  
  const std::string& getRemoteFingerPrint() const;
  
  const std::string& getClientMasterKey() const;
  
  const std::string& getServerMasterKey() const;
  
  const std::string& getClientMasterSalt() const;
  
  const std::string& getServerMasterSalt() const;
  
  bool isValid() const;
  
  srtp_policy_t& policy();
  
protected:
  bool _isValid;
  std::string _localFingerPrint;
  std::string _remoteFingerPrint;
  std::string _clientMasterKey;
  std::string _clientMasterSalt;
  std::string _serverMasterKey;
  std::string _serverMasterSalt;
  srtp_policy_t _policy;
};

//
// Inlines
//

inline const std::string& SRTPProfile::getLocalFingerPrint() const
{
  return _localFingerPrint;
}
  
inline const std::string& SRTPProfile::getRemoteFingerPrint() const
{
  return _remoteFingerPrint;
}

inline const std::string& SRTPProfile::getClientMasterKey() const
{
  return _clientMasterKey;
}

inline const std::string& SRTPProfile::getServerMasterKey() const
{
  return _serverMasterKey;
}

inline const std::string& SRTPProfile::getClientMasterSalt() const
{
  return _clientMasterSalt;
}

inline const std::string& SRTPProfile::getServerMasterSalt() const
{
  return _serverMasterSalt;
}

inline bool SRTPProfile::isValid() const
{
  return _isValid;
}

srtp_policy_t& SRTPProfile::policy()
{
  return _policy;
}

  
} } // OSS::RTP 

#endif	/* SRTPPROFILE_H */

