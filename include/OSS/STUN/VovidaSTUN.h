#ifndef STUN_H
#define STUN_H

#include <iostream>
#include <time.h>

// if you change this version, change in makefile too 
#define STUN_VERSION "0.96"

#define STUN_MAX_STRING 256
#define STUN_MAX_UNKNOWN_ATTRIBUTES 8
#define STUN_MAX_MESSAGE_SIZE 2048

#define STUN_PORT 3478

// define some basic types
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
#if defined( WIN32 )
typedef unsigned __int64 UInt64;
#else
typedef unsigned long long UInt64;
#endif
typedef struct { unsigned char octet[16]; }  UInt128;

/// define a structure to hold a vovida_stun_ address 
const UInt8  IPv4Family = 0x01;
const UInt8  IPv6Family = 0x02;

// define  flags  
const UInt32 ChangeIpFlag   = 0x04;
const UInt32 ChangePortFlag = 0x02;

// define  vovida_stun_ attribute
const UInt16 MappedAddress    = 0x0001;
const UInt16 ResponseAddress  = 0x0002;
const UInt16 ChangeRequest    = 0x0003;
const UInt16 SourceAddress    = 0x0004;
const UInt16 ChangedAddress   = 0x0005;
const UInt16 Username         = 0x0006;
const UInt16 Password         = 0x0007;
const UInt16 MessageIntegrity = 0x0008;
const UInt16 ErrorCode        = 0x0009;
const UInt16 UnknownAttribute = 0x000A;
const UInt16 ReflectedFrom    = 0x000B;
const UInt16 XorMappedAddress = 0x8020;
const UInt16 XorOnly          = 0x0021;
const UInt16 ServerName       = 0x8022;
const UInt16 SecondaryAddress = 0x8050; // Non standard extention

// define types for a vovida_stun_ message 
const UInt16 BindRequestMsg               = 0x0001;
const UInt16 BindResponseMsg              = 0x0101;
const UInt16 BindErrorResponseMsg         = 0x0111;
const UInt16 SharedSecretRequestMsg       = 0x0002;
const UInt16 SharedSecretResponseMsg      = 0x0102;
const UInt16 SharedSecretErrorResponseMsg = 0x0112;

typedef struct 
{
      UInt16 msgType;
      UInt16 msgLength;
      UInt128 id;
} VovidaStunMsgHdr;


typedef struct
{
      UInt16 type;
      UInt16 length;
} VovidaStunAtrHdr;

typedef struct
{
      UInt16 port;
      UInt32 addr;
} VovidaStunAddress4;

typedef struct
{
      UInt8 pad;
      UInt8 family;
      VovidaStunAddress4 ipv4;
} VovidaStunAtrAddress4;

typedef struct
{
      UInt32 value;
} VovidaStunAtrChangeRequest;

typedef struct
{
      UInt16 pad; // all 0
      UInt8 errorClass;
      UInt8 number;
      char reason[STUN_MAX_STRING];
      UInt16 sizeReason;
} VovidaStunAtrError;

typedef struct
{
      UInt16 attrType[STUN_MAX_UNKNOWN_ATTRIBUTES];
      UInt16 numAttributes;
} VovidaStunAtrUnknown;

typedef struct
{
      char value[STUN_MAX_STRING];      
      UInt16 sizeValue;
} VovidaStunAtrString;

typedef struct
{
      char hash[20];
} VovidaStunAtrIntegrity;

typedef enum 
{
   HmacUnkown=0,
   HmacOK,
   HmacBadUserName,
   HmacUnkownUserName,
   HmacFailed,
} VovidaStunHmacStatus;

typedef struct
{
      VovidaStunMsgHdr msgHdr;
	
      bool hasMappedAddress;
      VovidaStunAtrAddress4  mappedAddress;
	
      bool hasResponseAddress;
      VovidaStunAtrAddress4  responseAddress;
	
      bool hasChangeRequest;
      VovidaStunAtrChangeRequest changeRequest;
	
      bool hasSourceAddress;
      VovidaStunAtrAddress4 sourceAddress;
	
      bool hasChangedAddress;
      VovidaStunAtrAddress4 changedAddress;
	
      bool hasUsername;
      VovidaStunAtrString username;
	
      bool hasPassword;
      VovidaStunAtrString password;
	
      bool hasMessageIntegrity;
      VovidaStunAtrIntegrity messageIntegrity;
	
      bool hasErrorCode;
      VovidaStunAtrError errorCode;
	
      bool hasUnknownAttributes;
      VovidaStunAtrUnknown unknownAttributes;
	
      bool hasReflectedFrom;
      VovidaStunAtrAddress4 reflectedFrom;

      bool hasXorMappedAddress;
      VovidaStunAtrAddress4  xorMappedAddress;
	
      bool xorOnly;

      bool hasServerName;
      VovidaStunAtrString serverName;
      
      bool hasSecondaryAddress;
      VovidaStunAtrAddress4 secondaryAddress;
} VovidaStunMessage; 


// Define enum with different types of NAT 
typedef enum 
{
   VovidaStunTypeUnknown=0,
   VovidaStunTypeFailure,
   VovidaStunTypeOpen,
   VovidaStunTypeBlocked,

   VovidaStunTypeIndependentFilter,
   VovidaStunTypeDependentFilter,
   VovidaStunTypePortDependedFilter,
   VovidaStunTypeDependentMapping,

   //VovidaStunTypeConeNat,
   //VovidaStunTypeRestrictedNat,
   //VovidaStunTypePortRestrictedNat,
   //VovidaStunTypeSymNat,
   
   VovidaStunTypeFirewall,
} NatType;

#ifdef WIN32
typedef SOCKET Socket;
#else
typedef int Socket;
#endif

#define MAX_MEDIA_RELAYS 500
#define MAX_RTP_MSG_SIZE 1500
#define MEDIA_RELAY_TIMEOUT 3*60

typedef struct 
{
      int relayPort;       // media relay port
      int fd;              // media relay file descriptor
      VovidaStunAddress4 destination; // NAT IP:port
      time_t expireTime;      // if no activity after time, close the socket 
} VovidaStunMediaRelay;

typedef struct
{
      VovidaStunAddress4 myAddr;
      VovidaStunAddress4 altAddr;
      Socket myFd;
      Socket altPortFd;
      Socket altIpFd;
      Socket altIpPortFd;
      bool relay; // true if media relaying is to be done
      VovidaStunMediaRelay relays[MAX_MEDIA_RELAYS];
} VovidaStunServerInfo;

bool
vovida_stun_ParseMessage( char* buf, 
                  unsigned int bufLen, 
                  VovidaStunMessage& message, 
                  bool verbose );

void
vovida_stun_BuildReqSimple( VovidaStunMessage* msg,
                    const VovidaStunAtrString& username,
                    bool changePort, bool changeIp, unsigned int id=0 );

unsigned int
vovida_stun_EncodeMessage( const VovidaStunMessage& message, 
                   char* buf, 
                   unsigned int bufLen, 
                   const VovidaStunAtrString& password,
                   bool verbose);

void
vovida_stun_CreateUserName(const VovidaStunAddress4& addr, VovidaStunAtrString* username);

void 
vovida_stun_GetUserNameAndPassword(  const VovidaStunAddress4& dest, 
                             VovidaStunAtrString* username,
                             VovidaStunAtrString* password);

void
vovida_stun_CreatePassword(const VovidaStunAtrString& username, VovidaStunAtrString* password);

int 
vovida_stun_Rand();

UInt64
vovida_stun_GetSystemTimeSecs();

/// find the IP address of a the specified vovida_stun_ server - return false is fails parse 
bool  
vovida_stun_ParseServerName( const char* serverName, VovidaStunAddress4& vovida_stun_ServerAddr);

bool 
vovida_stun_ParseHostName( const char* peerName,
                   UInt32& ip,
                   UInt16& portVal,
                   UInt16 defaultPort );

/// return true if all is OK
/// Create a media relay and do the STERN thing if startMediaPort is non-zero
bool
vovida_stun_InitServer(VovidaStunServerInfo& info, 
               const VovidaStunAddress4& myAddr, 
               const VovidaStunAddress4& altAddr,
               int startMediaPort,
               bool verbose);

void
vovida_stun_StopServer(VovidaStunServerInfo& info);

/// return true if all is OK 
bool
vovida_stun_ServerProcess(VovidaStunServerInfo& info, bool verbose);

/// returns number of address found - take array or addres 
int 
vovida_stun_FindLocalInterfaces(UInt32* addresses, int maxSize );

void 
vovida_stun_Test( VovidaStunAddress4& dest, int testNum, bool verbose, VovidaStunAddress4* srcAddr=0 );

NatType
vovida_stun_NatType( VovidaStunAddress4& dest, bool verbose, 
             bool* preservePort=0, // if set, is return for if NAT preservers ports or not
             bool* hairpin=0 ,  // if set, is the return for if NAT will hairpin packets
             int port=0, // port to use for the test, 0 to choose random port
             VovidaStunAddress4* sAddr=0 // NIC to use 
   );

/// prints a VovidaStunAddress
std::ostream& 
operator<<( std::ostream& strm, const VovidaStunAddress4& addr);

#if 0
std::ostream& 
operator<< ( std::ostream& strm, const UInt128& );
#endif

bool
vovida_stun_ServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      VovidaStunAddress4& from, 
                      VovidaStunAddress4& myAddr,
                      VovidaStunAddress4& altAddr, 
                      VovidaStunMessage* resp,
                      VovidaStunAddress4* destination,
                      VovidaStunAtrString* hmacPassword,
                      bool* changePort,
                      bool* changeIp,
                      bool verbose);

int
vovida_stun_OpenSocket( VovidaStunAddress4& dest, 
                VovidaStunAddress4* mappedAddr, 
                int port=0, 
                VovidaStunAddress4* srcAddr=0, 
                bool verbose=false );

bool
vovida_stun_OpenSocketPair( VovidaStunAddress4& dest, VovidaStunAddress4* mappedAddr, 
                    int* fd1, int* fd2, 
                    int srcPort=0,  VovidaStunAddress4* srcAddr=0,
                    bool verbose=false);

int
vovida_stun_RandomPort();

#endif


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:

