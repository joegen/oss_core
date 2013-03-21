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

#include "OSS/OSS.h"

#include <iostream>
#include <time.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <errno.h>

#ifdef OSS_OS_FAMILY_WINDOWS
#include <winsock2.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#else
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>
#endif


#include "OSS/STUN/STUNProto.h"



namespace OSS {
namespace STUN {
namespace Proto {

#define NOSSL


#ifdef OSS_OS_FAMILY_WINDOWS
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
      StunAddress4 destination; // NAT IP:port
      time_t expireTime;      // if no activity after time, close the socket
} StunMediaRelay;

typedef struct
{
      StunAddress4 myAddr;
      StunAddress4 altAddr;
      Socket myFd;
      Socket altPortFd;
      Socket altIpFd;
      Socket altIpPortFd;
      bool relay; // true if media relaying is to be done
      StunMediaRelay relays[MAX_MEDIA_RELAYS];
} StunServerInfo;

/// return true if all is OK
/// Create a media relay and do the STERN thing if startMediaPort is non-zero
bool
stunInitServer(StunServerInfo& info,
               const StunAddress4& myAddr,
               const StunAddress4& altAddr,
               int startMediaPort,
               bool verbose);

void
stunStopServer(StunServerInfo& info);

/// return true if all is OK
bool
stunServerProcess(StunServerInfo& info, bool verbose);

/// returns number of address found - take array or addres
int
stunFindLocalInterfaces(UInt32* addresses, int maxSize );

void
stunTest( StunAddress4& dest, int testNum, bool verbose, StunAddress4* srcAddr=0 );

NatType
stunNatType( StunAddress4& dest, bool verbose,
             bool* preservePort=0, // if set, is return for if NAT preservers ports or not
             bool* hairpin=0 ,  // if set, is the return for if NAT will hairpin packets
             int port=0, // port to use for the test, 0 to choose random port
             StunAddress4* sAddr=0 // NIC to use
   );

/// prints a StunAddress
std::ostream&
operator<<( std::ostream& strm, const StunAddress4& addr);

std::ostream&
operator<< ( std::ostream& strm, const UInt128& );


bool
stunServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      StunAddress4& from,
                      StunAddress4& myAddr,
                      StunAddress4& altAddr,
                      StunMessage* resp,
                      StunAddress4* destination,
                      StunAtrString* hmacPassword,
                      bool* changePort,
                      bool* changeIp,
                      bool verbose);

int
stunOpenSocket( StunAddress4& dest,
                StunAddress4* mappedAddr,
                int port=0,
                StunAddress4* srcAddr=0,
                bool verbose=false );

bool
stunOpenSocketPair( StunAddress4& dest, StunAddress4* mappedAddr,
                    int* fd1, int* fd2,
                    int srcPort=0,  StunAddress4* srcAddr=0,
                    bool verbose=false);

static void
computeHmac(char* hmac, const char* input, int length, const char* key, int keySize);

static bool
stunParseAtrAddress( char* body, unsigned int hdrLen,  StunAtrAddress4& result )
{
   if ( hdrLen != 8 )
   {
      //clog << "hdrLen wrong for Address" <<endl;
      return false;
   }
   result.pad = *body++;
   result.family = *body++;
   if (result.family == IPv4Family)
   {
      UInt16 nport;
      memcpy(&nport, body, 2); body+=2;
      result.ipv4.port = ntohs(nport);

      UInt32 naddr;
      memcpy(&naddr, body, 4); body+=4;
      result.ipv4.addr = ntohl(naddr);
      return true;
   }
   else if (result.family == IPv6Family)
   {
      //clog << "ipv6 not supported" << endl;
   }
   else
   {
      //clog << "bad address family: " << result.family << endl;
   }

   return false;
}

static bool
stunParseAtrChangeRequest( char* body, unsigned int hdrLen,  StunAtrChangeRequest& result )
{
   if ( hdrLen != 4 )
   {
      //clog << "hdr length = " << hdrLen << " expecting " << sizeof(result) << endl;

      //clog << "Incorrect size for ChangeRequest" << endl;
      return false;
   }
   else
   {
      memcpy(&result.value, body, 4);
      result.value = ntohl(result.value);
      return true;
   }
}

static bool
stunParseAtrError( char* body, unsigned int hdrLen,  StunAtrError& result )
{
   if ( hdrLen >= sizeof(result) )
   {
      //clog << "head on Error too large" << endl;
      return false;
   }
   else
   {
      memcpy(&result.pad, body, 2); body+=2;
      result.pad = ntohs(result.pad);
      result.errorClass = *body++;
      result.number = *body++;

      result.sizeReason = hdrLen - 4;
      memcpy(&result.reason, body, result.sizeReason);
      result.reason[result.sizeReason] = 0;
      return true;
   }
}

static bool
stunParseAtrUnknown( char* body, unsigned int hdrLen,  StunAtrUnknown& result )
{
   if ( hdrLen >= sizeof(result) )
   {
      return false;
   }
   else
   {
      if (hdrLen % 4 != 0) return false;
      result.numAttributes = hdrLen / 4;
      for (int i=0; i<result.numAttributes; i++)
      {
         memcpy(&result.attrType[i], body, 2); body+=2;
         result.attrType[i] = ntohs(result.attrType[i]);
      }
      return true;
   }
}


static bool
stunParseAtrString( char* body, unsigned int hdrLen,  StunAtrString& result )
{
   if ( hdrLen >= STUN_MAX_STRING )
   {
      //clog << "String is too large" << endl;
      return false;
   }
   else
   {
      if (hdrLen % 4 != 0)
      {
         //clog << "Bad length string " << hdrLen << endl;
         return false;
      }

      result.sizeValue = hdrLen;
      memcpy(&result.value, body, hdrLen);
      result.value[hdrLen] = 0;
      return true;
   }
}


static bool
stunParseAtrIntegrity( char* body, unsigned int hdrLen,  StunAtrIntegrity& result )
{
   if ( hdrLen != 20)
   {
      //clog << "MessageIntegrity must be 20 bytes" << endl;
      return false;
   }
   else
   {
      memcpy(&result.hash, body, hdrLen);
      return true;
   }
}


bool
stunParseMessage( char* buf, unsigned int bufLen, StunMessage& msg, bool verbose)
{
   //if (verbose) //clog << "Received stun message: " << bufLen << " bytes" << endl;
   memset(&msg, 0, sizeof(msg));

   if (sizeof(StunMsgHdr) > bufLen)
   {
      //clog << "Bad message" << endl;
      return false;
   }

   memcpy(&msg.msgHdr, buf, sizeof(StunMsgHdr));
   msg.msgHdr.msgType = ntohs(msg.msgHdr.msgType);
   msg.msgHdr.msgLength = ntohs(msg.msgHdr.msgLength);

   if (msg.msgHdr.msgLength + sizeof(StunMsgHdr) != bufLen)
   {
      //clog << "Message header length doesn't match message size: "
      //     << msg.msgHdr.msgLength << " - " << bufLen << endl;
      return false;
   }

   char* body = buf + sizeof(StunMsgHdr);
   unsigned int size = msg.msgHdr.msgLength;

   ////clog << "bytes after header = " << size << endl;

   while ( size > 0 )
   {
      // !jf! should check that there are enough bytes left in the buffer

      StunAtrHdr* attr = reinterpret_cast<StunAtrHdr*>(body);

      unsigned int attrLen = ntohs(attr->length);
      int atrType = ntohs(attr->type);

      ////if (verbose) //clog << "Found attribute type=" << AttrNames[atrType] << " length=" << attrLen << endl;
      if ( attrLen+4 > size )
      {
         //clog << "claims attribute is larger than size of message "
         //     <<"(attribute type="<<atrType<<")"<< endl;
         return false;
      }

      body += 4; // skip the length and type in attribute header
      size -= 4;

      switch ( atrType )
      {
         case MappedAddress:
            msg.hasMappedAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.mappedAddress )== false )
            {
               //clog << "problem parsing MappedAddress" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "MappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }

            break;

         case ResponseAddress:
            msg.hasResponseAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.responseAddress )== false )
            {
               //clog << "problem parsing ResponseAddress" << endl;
               return false;
            }
            else
            {
               ////if (verbose) //clog << "ResponseAddress = " << msg.responseAddress.ipv4 << endl;
            }
            break;

         case ChangeRequest:
            msg.hasChangeRequest = true;
            if (stunParseAtrChangeRequest( body, attrLen, msg.changeRequest) == false)
            {
               //clog << "problem parsing ChangeRequest" << endl;
               return false;
            }
            else
            {
               ////if (verbose) //clog << "ChangeRequest = " << msg.changeRequest.value << endl;
            }
            break;

         case SourceAddress:
            msg.hasSourceAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.sourceAddress )== false )
            {
               //clog << "problem parsing SourceAddress" << endl;
               return false;
            }
            else
            {
               ////if (verbose) //clog << "SourceAddress = " << msg.sourceAddress.ipv4 << endl;
            }
            break;

         case ChangedAddress:
            msg.hasChangedAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.changedAddress )== false )
            {
               //clog << "problem parsing ChangedAddress" << endl;
               return false;
            }
            else
            {
               ////if (verbose) //clog << "ChangedAddress = " << msg.changedAddress.ipv4 << endl;
            }
            break;

         case Username:
            msg.hasUsername = true;
            if (stunParseAtrString( body, attrLen, msg.username) == false)
            {
               //clog << "problem parsing Username" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "Username = " << msg.username.value << endl;
            }

            break;

         case Password:
            msg.hasPassword = true;
            if (stunParseAtrString( body, attrLen, msg.password) == false)
            {
               //clog << "problem parsing Password" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "Password = " << msg.password.value << endl;
            }
            break;

         case MessageIntegrity:
            msg.hasMessageIntegrity = true;
            if (stunParseAtrIntegrity( body, attrLen, msg.messageIntegrity) == false)
            {
               //clog << "problem parsing MessageIntegrity" << endl;
               return false;
            }
            else
            {
               ////if (verbose) //clog << "MessageIntegrity = " << msg.messageIntegrity.hash << endl;
            }

            // read the current HMAC
            // look up the password given the user of given the transaction id
            // compute the HMAC on the buffer
            // decide if they match or not
            break;

         case ErrorCode:
            msg.hasErrorCode = true;
            if (stunParseAtrError(body, attrLen, msg.errorCode) == false)
            {
               //clog << "problem parsing ErrorCode" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "ErrorCode = " << int(msg.errorCode.errorClass)
                //                 << " " << int(msg.errorCode.number)
                //                 << " " << msg.errorCode.reason << endl;
            }

            break;

         case UnknownAttribute:
            msg.hasUnknownAttributes = true;
            if (stunParseAtrUnknown(body, attrLen, msg.unknownAttributes) == false)
            {
               //clog << "problem parsing UnknownAttribute" << endl;
               return false;
            }
            break;

         case ReflectedFrom:
            msg.hasReflectedFrom = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.reflectedFrom ) == false )
            {
               //clog << "problem parsing ReflectedFrom" << endl;
               return false;
            }
            break;

         case XorMappedAddress:
            msg.hasXorMappedAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.xorMappedAddress ) == false )
            {
               //clog << "problem parsing XorMappedAddress" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "XorMappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
            break;

         case XorOnly:
            msg.xorOnly = true;
            //if (verbose)
            {
               //clog << "xorOnly = true" << endl;
            }
            break;

         case ServerName:
            msg.hasServerName = true;
            if (stunParseAtrString( body, attrLen, msg.serverName) == false)
            {
               //clog << "problem parsing ServerName" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "ServerName = " << msg.serverName.value << endl;
            }
            break;

         case SecondaryAddress:
            msg.hasSecondaryAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.secondaryAddress ) == false )
            {
               //clog << "problem parsing secondaryAddress" << endl;
               return false;
            }
            else
            {
               //if (verbose) //clog << "SecondaryAddress = " << msg.secondaryAddress.ipv4 << endl;
            }
            break;

         default:
            //if (verbose) //clog << "Unknown attribute: " << atrType << endl;
            if ( atrType <= 0x7FFF )
            {
               return false;
            }
      }

      body += attrLen;
      size -= attrLen;
   }

   return true;
}


static char*
encode16(char* buf, UInt16 data)
{
   UInt16 ndata = htons(data);
   memcpy(buf, reinterpret_cast<void*>(&ndata), sizeof(UInt16));
   return buf + sizeof(UInt16);
}

static char*
encode32(char* buf, UInt32 data)
{
   UInt32 ndata = htonl(data);
   memcpy(buf, reinterpret_cast<void*>(&ndata), sizeof(UInt32));
   return buf + sizeof(UInt32);
}


static char*
encode(char* buf, const char* data, unsigned int length)
{
   memcpy(buf, data, length);
   return buf + length;
}


static char*
encodeAtrAddress4(char* ptr, UInt16 type, const StunAtrAddress4& atr)
{
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, 8);
   *ptr++ = atr.pad;
   *ptr++ = IPv4Family;
   ptr = encode16(ptr, atr.ipv4.port);
   ptr = encode32(ptr, atr.ipv4.addr);

   return ptr;
}

static char*
encodeAtrChangeRequest(char* ptr, const StunAtrChangeRequest& atr)
{
   ptr = encode16(ptr, ChangeRequest);
   ptr = encode16(ptr, 4);
   ptr = encode32(ptr, atr.value);
   return ptr;
}

static char*
encodeAtrError(char* ptr, const StunAtrError& atr)
{
   ptr = encode16(ptr, ErrorCode);
   ptr = encode16(ptr, 6 + atr.sizeReason);
   ptr = encode16(ptr, atr.pad);
   *ptr++ = atr.errorClass;
   *ptr++ = atr.number;
   ptr = encode(ptr, atr.reason, atr.sizeReason);
   return ptr;
}


static char*
encodeAtrUnknown(char* ptr, const StunAtrUnknown& atr)
{
   ptr = encode16(ptr, UnknownAttribute);
   ptr = encode16(ptr, 2+2*atr.numAttributes);
   for (int i=0; i<atr.numAttributes; i++)
   {
      ptr = encode16(ptr, atr.attrType[i]);
   }
   return ptr;
}


static char*
encodeXorOnly(char* ptr)
{
   ptr = encode16(ptr, XorOnly );
   return ptr;
}


static char*
encodeAtrString(char* ptr, UInt16 type, const StunAtrString& atr)
{
   assert(atr.sizeValue % 4 == 0);

   ptr = encode16(ptr, type);
   ptr = encode16(ptr, atr.sizeValue);
   ptr = encode(ptr, atr.value, atr.sizeValue);
   return ptr;
}


static char*
encodeAtrIntegrity(char* ptr, const StunAtrIntegrity& atr)
{
   ptr = encode16(ptr, MessageIntegrity);
   ptr = encode16(ptr, 20);
   ptr = encode(ptr, atr.hash, sizeof(atr.hash));
   return ptr;
}


unsigned int
stunEncodeMessage( const StunMessage& msg,
                   char* buf,
                   unsigned int bufLen,
                   const StunAtrString& password,
                   bool verbose)
{
   assert(bufLen >= sizeof(StunMsgHdr));
   char* ptr = buf;

   ptr = encode16(ptr, msg.msgHdr.msgType);
   char* lengthp = ptr;
   ptr = encode16(ptr, 0);
   ptr = encode(ptr, reinterpret_cast<const char*>(msg.msgHdr.id.octet), sizeof(msg.msgHdr.id));

   //if (verbose) //clog << "Encoding stun message: " << endl;
   if (msg.hasMappedAddress)
   {
      //if (verbose) //clog << "Encoding MappedAddress: " << msg.mappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, MappedAddress, msg.mappedAddress);
   }
   if (msg.hasResponseAddress)
   {
      //if (verbose) //clog << "Encoding ResponseAddress: " << msg.responseAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ResponseAddress, msg.responseAddress);
   }
   if (msg.hasChangeRequest)
   {
      //if (verbose) //clog << "Encoding ChangeRequest: " << msg.changeRequest.value << endl;
      ptr = encodeAtrChangeRequest(ptr, msg.changeRequest);
   }
   if (msg.hasSourceAddress)
   {
      //if (verbose) //clog << "Encoding SourceAddress: " << msg.sourceAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, SourceAddress, msg.sourceAddress);
   }
   if (msg.hasChangedAddress)
   {
      //if (verbose) //clog << "Encoding ChangedAddress: " << msg.changedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ChangedAddress, msg.changedAddress);
   }
   if (msg.hasUsername)
   {
      //if (verbose) //clog << "Encoding Username: " << msg.username.value << endl;
      ptr = encodeAtrString(ptr, Username, msg.username);
   }
   if (msg.hasPassword)
   {
      //if (verbose) //clog << "Encoding Password: " << msg.password.value << endl;
      ptr = encodeAtrString(ptr, Password, msg.password);
   }
   if (msg.hasErrorCode)
   {
      //if (verbose) //clog << "Encoding ErrorCode: class="
			//<< int(msg.errorCode.errorClass)
			//<< " number=" << int(msg.errorCode.number)
			//<< " reason="
			//<< msg.errorCode.reason
			//<< endl;

      ptr = encodeAtrError(ptr, msg.errorCode);
   }
   if (msg.hasUnknownAttributes)
   {
      //if (verbose) //clog << "Encoding UnknownAttribute: ???" << endl;
      ptr = encodeAtrUnknown(ptr, msg.unknownAttributes);
   }
   if (msg.hasReflectedFrom)
   {
      //if (verbose) //clog << "Encoding ReflectedFrom: " << msg.reflectedFrom.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ReflectedFrom, msg.reflectedFrom);
   }
   if (msg.hasXorMappedAddress)
   {
      //if (verbose) //clog << "Encoding XorMappedAddress: " << msg.xorMappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, XorMappedAddress, msg.xorMappedAddress);
   }
   if (msg.xorOnly)
   {
      //if (verbose) //clog << "Encoding xorOnly: " << endl;
      ptr = encodeXorOnly( ptr );
   }
   if (msg.hasServerName)
   {
      //if (verbose) //clog << "Encoding ServerName: " << msg.serverName.value << endl;
      ptr = encodeAtrString(ptr, ServerName, msg.serverName);
   }
   if (msg.hasSecondaryAddress)
   {
      //if (verbose) //clog << "Encoding SecondaryAddress: " << msg.secondaryAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, SecondaryAddress, msg.secondaryAddress);
   }

   if (password.sizeValue > 0)
   {
      //if (verbose) //clog << "HMAC with password: " << password.value << endl;

      StunAtrIntegrity integrity;
      computeHmac(integrity.hash, buf, int(ptr-buf) , password.value, password.sizeValue);
      ptr = encodeAtrIntegrity(ptr, integrity);
   }
   //if (verbose) //clog << endl;

   encode16(lengthp, UInt16(ptr - buf - sizeof(StunMsgHdr)));
   return int(ptr - buf);
}


/// return a random number to use as a port
int
stunRandomPort()
{
   int min=0x4000;
   int max=0x7FFF;

   int ret = stunRand();
   ret = ret|min;
   ret = ret&max;

   return ret;
}


#ifdef NOSSL
static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
   strncpy(hmac,"hmac-not-implemented",20);
}
#else
#include <openssl/hmac.h>

static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
   unsigned int resultSize=0;
   HMAC(EVP_sha1(),
        key, sizeKey,
        reinterpret_cast<const unsigned char*>(input), length,
        reinterpret_cast<unsigned char*>(hmac), &resultSize);
   assert(resultSize == 20);
}
#endif


static void
toHex(const char* buffer, int bufferSize, char* output)
{
   static char hexmap[] = "0123456789abcdef";

   const char* p = buffer;
   char* r = output;
   for (int i=0; i < bufferSize; i++)
   {
      unsigned char temp = *p++;

      int hi = (temp & 0xf0)>>4;
      int low = (temp & 0xf);

      *r++ = hexmap[hi];
      *r++ = hexmap[low];
   }
   *r = 0;
}

void
stunCreateUserName(const StunAddress4& source, StunAtrString* username)
{
   UInt64 time = stunGetSystemTimeSecs();
   time -= (time % 20*60);
   //UInt64 hitime = time >> 32;
   UInt64 lotime = time & 0xFFFFFFFF;

   char buffer[1024];
   sprintf(buffer,
           "%08x:%08x:%08x:",
           UInt32(source.addr),
           UInt32(stunRand()),
           UInt32(lotime));
   assert( strlen(buffer) < 1024 );

   assert(strlen(buffer) + 41 < STUN_MAX_STRING);

   char hmac[20];
   char key[] = "Jason";
   computeHmac(hmac, buffer, strlen(buffer), key, strlen(key) );
   char hmacHex[41];
   toHex(hmac, 20, hmacHex );
   hmacHex[40] =0;

   strcat(buffer,hmacHex);

   int l = strlen(buffer);
   assert( l+1 < STUN_MAX_STRING );
   assert( l%4 == 0 );

   username->sizeValue = l;
   memcpy(username->value,buffer,l);
   username->value[l]=0;

   ////if (verbose) //clog << "computed username=" << username.value << endl;
}

void
stunCreatePassword(const StunAtrString& username, StunAtrString* password)
{
   char hmac[20];
   char key[] = "Fluffy";
   //char buffer[STUN_MAX_STRING];
   computeHmac(hmac, username.value, strlen(username.value), key, strlen(key));
   toHex(hmac, 20, password->value);
   password->sizeValue = 40;
   password->value[40]=0;

   ////clog << "password=" << password->value << endl;
}


UInt64
stunGetSystemTimeSecs()
{
   UInt64 time=0;
#if defined(OSS_OS_FAMILY_WINDOWS)
   SYSTEMTIME t;
   // CJ TODO - this probably has bug on wrap around every 24 hours
   GetSystemTime( &t );
   time = (t.wHour*60+t.wMinute)*60+t.wSecond;
#else
   struct timeval now;
   gettimeofday( &now , NULL );
   //assert( now );
   time = now.tv_sec;
#endif
   return time;
}


std::ostream& operator<< ( std::ostream& strm, const UInt128& r )
{
   strm << int(r.octet[0]);
   for ( int i=1; i<16; i++ )
   {
      strm << ':' << int(r.octet[i]);
   }

   return strm;
}

std::ostream&
operator<<( std::ostream& strm, const StunAddress4& addr)
{
   UInt32 ip = addr.addr;
   strm << ((int)(ip>>24)&0xFF) << ".";
   strm << ((int)(ip>>16)&0xFF) << ".";
   strm << ((int)(ip>> 8)&0xFF) << ".";
   strm << ((int)(ip>> 0)&0xFF) ;

   strm << ":" << addr.port;

   return strm;
}


// returns true if it scucceeded
bool
stunParseHostName( char* peerName,
               UInt32& ip,
               UInt16& portVal,
               UInt16 defaultPort )
{
   in_addr sin_addr;

   char host[512];
   strncpy(host,peerName,512);
   host[512-1]='\0';
   char* port = NULL;

   int portNum = defaultPort;

   // pull out the port part if present.
   char* sep = strchr(host,':');

   if ( sep == NULL )
   {
      portNum = defaultPort;
   }
   else
   {
      *sep = '\0';
      port = sep + 1;
      // set port part

      char* endPtr=NULL;

      portNum = strtol(port,&endPtr,10);

      if ( endPtr != NULL )
      {
         if ( *endPtr != '\0' )
         {
            portNum = defaultPort;
         }
      }
   }

   if ( portNum < 1024 ) return false;
   if ( portNum >= 0xFFFF ) return false;

   // figure out the host part
   struct hostent* h;

#ifdef OSS_OS_FAMILY_WINDOWS
   assert( strlen(host) >= 1 );
   if ( isdigit( host[0] ) )
   {
      // assume it is a ip address
      unsigned long a = inet_addr(host);
      //cerr << "a=0x" << hex << a << dec << endl;

      ip = ntohl( a );
   }
   else
   {
      // assume it is a host name
      h = gethostbyname( host );

      if ( h == NULL )
      {
         int err = getErrno();
         std::cerr << "error was " << err << std::endl;
         assert( err != WSANOTINITIALISED );

         ip = ntohl( 0x7F000001L );

         return false;
      }
      else
      {
         sin_addr = *(struct in_addr*)h->h_addr;
         ip = ntohl( sin_addr.s_addr );
      }
   }

#else
   h = gethostbyname( host );
   if ( h == NULL )
   {
      //int err = getErrno();
      //std::cerr << "error was " << err << std::endl;
      ip = ntohl( 0x7F000001L );
      return false;
   }
   else
   {
      sin_addr = *(struct in_addr*)h->h_addr;
      ip = ntohl( sin_addr.s_addr );
   }
#endif

   portVal = portNum;

   return true;
}


bool
stunParseServerName( char* name, StunAddress4& addr)
{
   assert(name);

   // TODO - put in DNS SRV stuff.

   bool ret = stunParseHostName( name, addr.addr, addr.port, 3478);
   if ( ret != true )
   {
       addr.port=0xFFFF;
   }
   return ret;
}


static void
stunCreateErrorResponse(StunMessage& response, int cl, int number, const char* msg)
{
   response.msgHdr.msgType = BindErrorResponseMsg;
   response.hasErrorCode = true;
   response.errorCode.errorClass = cl;
   response.errorCode.number = number;
   strcpy(response.errorCode.reason, msg);
}

static void
stunCreateSharedSecretResponse(const StunMessage& request, const StunAddress4& source, StunMessage& response)
{
   response.msgHdr.msgType = SharedSecretResponseMsg;
   response.msgHdr.id = request.msgHdr.id;

   response.hasUsername = true;
   stunCreateUserName( source, &response.username);

   response.hasPassword = true;
   stunCreatePassword( response.username, &response.password);
}


// This funtion takes a single message sent to a stun server, parses
// and constructs an apropriate repsonse - returns true if message is
// valid
bool
stunServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      StunAddress4& from,
                      StunAddress4& secondary,
                      StunAddress4& myAddr,
                      StunAddress4& altAddr,
                      StunMessage* resp,
                      StunAddress4* destination,
                      StunAtrString* hmacPassword,
                      bool* changePort,
                      bool* changeIp,
                      bool verbose)
{

   // set up information for default response

   memset( resp, 0 , sizeof(*resp) );

   *changeIp = false;
   *changePort = false;

   StunMessage req;
   bool ok = stunParseMessage( buf,bufLen, req, verbose);

   if (!ok)      // Complete garbage, drop it on the floor
   {
      //if (verbose) //clog << "Request did not parse" << endl;
      return false;
   }
   //if (verbose) //clog << "Request parsed ok" << endl;

   StunAddress4 mapped = req.mappedAddress.ipv4;
   StunAddress4 respondTo = req.responseAddress.ipv4;
   UInt32 flags = req.changeRequest.value;

   switch (req.msgHdr.msgType)
   {
      case SharedSecretRequestMsg:
         if(verbose) //clog << "Received SharedSecretRequestMsg on udp. send error 433." << endl;
         // !cj! - should fix so you know if this came over TLS or UDP
         stunCreateSharedSecretResponse(req, from, *resp);
         //stunCreateSharedSecretErrorResponse(*resp, 4, 33, "this request must be over TLS");
         return true;

      case BindRequestMsg:
         if (!req.hasMessageIntegrity)
         {
            //if (verbose) //clog << "BindRequest does not contain MessageIntegrity" << endl;

            if (0) // !jf! mustAuthenticate
            {
               if(verbose) //clog << "Received BindRequest with no MessageIntegrity. Sending 401." << endl;
               stunCreateErrorResponse(*resp, 4, 1, "Missing MessageIntegrity");
               return true;
            }
         }
         else
         {
            if (!req.hasUsername)
            {
               //if (verbose) //clog << "No UserName. Send 432." << endl;
               stunCreateErrorResponse(*resp, 4, 32, "No UserName and contains MessageIntegrity");
               return true;
            }
            else
            {
               //if (verbose) //clog << "Validating username: " << req.username.value << endl;
               // !jf! could retrieve associated password from provisioning here
               if (strcmp(req.username.value, "test") == 0)
               {
                  if (0)
                  {
                     // !jf! if the credentials are stale
                     stunCreateErrorResponse(*resp, 4, 30, "Stale credentials on BindRequest");
                     return true;
                  }
                  else
                  {
                     //if (verbose) //clog << "Validating MessageIntegrity" << endl;
                     // need access to shared secret

                     unsigned char hmac[20];
#ifndef NOSSL
                     unsigned int hmacSize=20;

                     HMAC(EVP_sha1(),
                          "1234", 4,
                          reinterpret_cast<const unsigned char*>(buf), bufLen-20-4,
                          hmac, &hmacSize);
                     assert(hmacSize == 20);
#endif

                     if (memcmp(buf, hmac, 20) != 0)
                     {
                        //if (verbose) //clog << "MessageIntegrity is bad. Sending " << endl;
                        stunCreateErrorResponse(*resp, 4, 3, "Unknown username. Try test with password 1234");
                        return true;
                     }

                     // need to compute this later after message is filled in
                     resp->hasMessageIntegrity = true;
                     assert(req.hasUsername);
                     resp->hasUsername = true;
                     resp->username = req.username; // copy username in
                  }
               }
               else
               {
                  //if (verbose) //clog << "Invalid username: " << req.username.value << "Send 430." << endl;
               }
            }
         }

         // TODO !jf! should check for unknown attributes here and send 420 listing the
         // unknown attributes.

         if ( respondTo.port == 0 ) respondTo = from;
         if ( mapped.port == 0 ) mapped = from;

         *changeIp   = ( flags & ChangeIpFlag )?true:false;
         *changePort = ( flags & ChangePortFlag )?true:false;

         //if (verbose)
         {
            //clog << "Request is valid:" << endl;
            //clog << "\t flags=" << flags << endl;
            //clog << "\t changeIp=" << *changeIp << endl;
            //clog << "\t changePort=" << *changePort << endl;
            //clog << "\t from = " << from << endl;
            //clog << "\t respond to = " << respondTo << endl;
            //clog << "\t mapped = " << mapped << endl;
         }

         // form the outgoing message
         resp->msgHdr.msgType = BindResponseMsg;
         for ( int i=0; i<16; i++ )
         {
            resp->msgHdr.id.octet[i] = req.msgHdr.id.octet[i];
         }

         if ( req.xorOnly == false )
         {
            resp->hasMappedAddress = true;
            resp->mappedAddress.ipv4.port = mapped.port;
            resp->mappedAddress.ipv4.addr = mapped.addr;
         }

         if (1) // do xorMapped address or not
         {
            resp->hasXorMappedAddress = true;
            UInt16 id16 = req.msgHdr.id.octet[0]<<8
               | req.msgHdr.id.octet[1];
            UInt32 id32 = req.msgHdr.id.octet[0]<<24
               | req.msgHdr.id.octet[1]<<16
               | req.msgHdr.id.octet[2]<<8
               | req.msgHdr.id.octet[3];
            resp->xorMappedAddress.ipv4.port = mapped.port^id16;
            resp->xorMappedAddress.ipv4.addr = mapped.addr^id32;
         }

         resp->hasSourceAddress = true;
         resp->sourceAddress.ipv4.port = (*changePort) ? altAddr.port : myAddr.port;
         resp->sourceAddress.ipv4.addr = (*changeIp)   ? altAddr.addr : myAddr.addr;

         resp->hasChangedAddress = true;
         resp->changedAddress.ipv4.port = altAddr.port;
         resp->changedAddress.ipv4.addr = altAddr.addr;

         if ( secondary.port != 0 )
         {
            resp->hasSecondaryAddress = true;
            resp->secondaryAddress.ipv4.port = secondary.port;
            resp->secondaryAddress.ipv4.addr = secondary.addr;
         }

         if ( req.hasUsername && req.username.sizeValue > 0 )
         {
            // copy username in
            resp->hasUsername = true;
            assert( req.username.sizeValue % 4 == 0 );
            assert( req.username.sizeValue < STUN_MAX_STRING );
            memcpy( resp->username.value, req.username.value, req.username.sizeValue );
            resp->username.sizeValue = req.username.sizeValue;
         }

         if (1) // add ServerName
         {
            resp->hasServerName = true;
            const char serverName[] = "OSS STUN API"; // must pad to mult of 4

            assert( sizeof(serverName) < STUN_MAX_STRING );
            //cerr << "sizeof serverName is "  << sizeof(serverName) << endl;
            assert( sizeof(serverName)%4 == 0 );
            memcpy( resp->serverName.value, serverName, sizeof(serverName));
            resp->serverName.sizeValue = sizeof(serverName);
         }

         if ( req.hasMessageIntegrity & req.hasUsername )
         {
            // this creates the password that will be used in the HMAC when then
            // messages is sent
            stunCreatePassword( req.username, hmacPassword );
         }

         if (req.hasUsername && (req.username.sizeValue > 64 ) )
         {
            UInt32 source;
            assert( sizeof(int) == sizeof(UInt32) );

            sscanf(req.username.value, "%x", &source);
            resp->hasReflectedFrom = true;
            resp->reflectedFrom.ipv4.port = 0;
            resp->reflectedFrom.ipv4.addr = source;
         }

         destination->port = respondTo.port;
         destination->addr = respondTo.addr;

         return true;

      default:
         //if (verbose) //clog << "Unknown or unsupported request " << endl;
         return false;
   }

   assert(0);
   return false;
}

void
stunBuildReqSimple( StunMessage* msg,
                    const StunAtrString& username,
                    bool changePort, bool changeIp, unsigned int id )
{
   assert( msg );
   memset( msg , 0 , sizeof(*msg) );

   msg->msgHdr.msgType = BindRequestMsg;

   for ( int i=0; i<16; i=i+4 )
   {
      assert(i+3<16);
      int r = stunRand();
      msg->msgHdr.id.octet[i+0]= r>>0;
      msg->msgHdr.id.octet[i+1]= r>>8;
      msg->msgHdr.id.octet[i+2]= r>>16;
      msg->msgHdr.id.octet[i+3]= r>>24;
   }

   if ( id != 0 )
   {
      msg->msgHdr.id.octet[0] = id;
   }

   msg->hasChangeRequest = true;
   msg->changeRequest.value =(changeIp?ChangeIpFlag:0) |
      (changePort?ChangePortFlag:0);

   if ( username.sizeValue > 0 )
   {
      msg->hasUsername = true;
      msg->username = username;
   }
}


} } }// OSS::STUN::Proto



////////////////////////////////////////////////////////////////////////////////



