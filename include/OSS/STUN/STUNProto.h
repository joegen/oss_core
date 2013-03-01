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


#ifndef OSS_STUNPROTO_H
#define	OSS_STUNPROTO_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "OSS/OSS.h"
#include "OSS/IPAddress.h"
#include "OSS/Thread.h"
#include "OSS/Core.h"
#include "OSS/STUN/STUN.h"


#define STUN_MAX_STRING 256
#define STUN_MAX_UNKNOWN_ATTRIBUTES 8
#define STUN_MAX_MESSAGE_SIZE 2048
#define STUN_PORT 3478
#define stunRand OSS::getRandom

namespace OSS {
namespace STUN {
namespace Proto {

  typedef struct { unsigned char octet[16]; }  UInt128;

  /// define a structure to hold a stun address
  const UInt8  IPv4Family = 0x01;
  const UInt8  IPv6Family = 0x02;

  // define  flags
  const UInt32 ChangeIpFlag   = 0x04;
  const UInt32 ChangePortFlag = 0x02;

  // define  stun attribute
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

  // define types for a stun message
  const UInt16 BindRequestMsg               = 0x0001;
  const UInt16 BindResponseMsg              = 0x0101;
  const UInt16 BindErrorResponseMsg         = 0x0111;
  const UInt16 SharedSecretRequestMsg       = 0x0002;
  const UInt16 SharedSecretResponseMsg      = 0x0102;
  const UInt16 SharedSecretErrorResponseMsg = 0x0112;

  // Define enum with different types of NAT
  typedef enum
  {
     StunTypeUnknown=0,
     StunTypeFailure,
     StunTypeOpen,
     StunTypeBlocked,

     StunTypeIndependentFilter,
     StunTypeDependentFilter,
     StunTypePortDependedFilter,
     StunTypeDependentMapping,

     //StunTypeConeNat,
     //StunTypeRestrictedNat,
     //StunTypePortRestrictedNat,
     //StunTypeSymNat,

     StunTypeFirewall,
  } NatType;
  
  typedef enum
  {
     HmacUnkown=0,
     HmacOK,
     HmacBadUserName,
     HmacUnkownUserName,
     HmacFailed,
  } StunHmacStatus;

  typedef struct
  {
    UInt16 msgType;
    UInt16 msgLength;
    UInt128 id;
  } StunMsgHdr;


  typedef struct
  {
    UInt16 type;
    UInt16 length;
  } StunAtrHdr;

  typedef struct
  {
    UInt16 port;
    UInt32 addr;
  } StunAddress4;

  typedef struct
  {
    UInt8 pad;
    UInt8 family;
    StunAddress4 ipv4;
  } StunAtrAddress4;

  typedef struct
  {
    UInt32 value;
  } StunAtrChangeRequest;

  typedef struct
  {
    UInt16 pad; // all 0
    UInt8 errorClass;
    UInt8 number;
    char reason[STUN_MAX_STRING];
    UInt16 sizeReason;
  } StunAtrError;

  typedef struct
  {
    UInt16 attrType[STUN_MAX_UNKNOWN_ATTRIBUTES];
    UInt16 numAttributes;
  } StunAtrUnknown;

  typedef struct
  {
    char value[STUN_MAX_STRING];
    UInt16 sizeValue;
  } StunAtrString;

  typedef struct
  {
    char hash[20];
  } StunAtrIntegrity;

  typedef struct
  {
    StunMsgHdr msgHdr;

    bool hasMappedAddress;
    StunAtrAddress4  mappedAddress;

    bool hasResponseAddress;
    StunAtrAddress4  responseAddress;

    bool hasChangeRequest;
    StunAtrChangeRequest changeRequest;

    bool hasSourceAddress;
    StunAtrAddress4 sourceAddress;

    bool hasChangedAddress;
    StunAtrAddress4 changedAddress;

    bool hasUsername;
    StunAtrString username;

    bool hasPassword;
    StunAtrString password;

    bool hasMessageIntegrity;
    StunAtrIntegrity messageIntegrity;

    bool hasErrorCode;
    StunAtrError errorCode;

    bool hasUnknownAttributes;
    StunAtrUnknown unknownAttributes;

    bool hasReflectedFrom;
    StunAtrAddress4 reflectedFrom;

    bool hasXorMappedAddress;
    StunAtrAddress4  xorMappedAddress;

    bool xorOnly;

    bool hasServerName;
    StunAtrString serverName;

    bool hasSecondaryAddress;
    StunAtrAddress4 secondaryAddress;
  } StunMessage;

  bool OSS_API stunParseMessage( char* buf,
                  unsigned int bufLen,
                  StunMessage& message,
                  bool verbose );

  void OSS_API stunBuildReqSimple( StunMessage* msg,
                      const StunAtrString& username,
                      bool changePort, bool changeIp, unsigned int id=0 );

  unsigned int OSS_API stunEncodeMessage( const StunMessage& message,
                     char* buf,
                     unsigned int bufLen,
                     const StunAtrString& password,
                     bool verbose);

  void OSS_API stunCreateUserName(const StunAddress4& addr, StunAtrString* username);

  void OSS_API stunGetUserNameAndPassword(  const StunAddress4& dest,
                               StunAtrString* username,
                               StunAtrString* password);

  void OSS_API stunCreatePassword(const StunAtrString& username, StunAtrString* password);

  UInt64 OSS_API stunGetSystemTimeSecs();

  /// find the IP address of a the specified stun server - return false is fails parse
  bool OSS_API stunParseServerName( char* serverName, StunAddress4& stunServerAddr);

  bool OSS_API stunParseHostName( char* peerName,
                     UInt32& ip,
                     UInt16& portVal,
                     UInt16 defaultPort );

  int stunRandomPort();

} } } // OSS::STUN::Proto


#endif	// OSS_STUNPROTO_H


//
// This library uses code derived from the original work of Vovida Networks
//

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


