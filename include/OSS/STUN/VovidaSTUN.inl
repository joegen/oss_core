#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   
#include <errno.h>

#ifdef WIN32
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


#if defined(__sparc__) || defined(WIN32)
#define NOSSL
#endif
#define NOSSL

#include "OSS/STUN/VovidaUDP.h"
#include "OSS/STUN/VovidaSTUN.h"


using namespace std;


static void
computeHmac(char* hmac, const char* input, int length, const char* key, int keySize);

static bool 
vovida_stun_ParseAtrAddress( char* body, unsigned int hdrLen,  VovidaStunAtrAddress4& result )
{
   if ( hdrLen != 8 )
   {
      clog << "hdrLen wrong for Address" <<endl;
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
      clog << "ipv6 not supported" << endl;
   }
   else
   {
      clog << "bad address family: " << result.family << endl;
   }
	
   return false;
}

static bool 
vovida_stun_ParseAtrChangeRequest( char* body, unsigned int hdrLen,  VovidaStunAtrChangeRequest& result )
{
   if ( hdrLen != 4 )
   {
      clog << "hdr length = " << hdrLen << " expecting " << sizeof(result) << endl;
		
      clog << "Incorrect size for ChangeRequest" << endl;
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
vovida_stun_ParseAtrError( char* body, unsigned int hdrLen,  VovidaStunAtrError& result )
{
   if ( hdrLen >= sizeof(result) )
   {
      clog << "head on Error too large" << endl;
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
vovida_stun_ParseAtrUnknown( char* body, unsigned int hdrLen,  VovidaStunAtrUnknown& result )
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
vovida_stun_ParseAtrString( char* body, unsigned int hdrLen,  VovidaStunAtrString& result )
{
   if ( hdrLen >= STUN_MAX_STRING )
   {
      clog << "String is too large" << endl;
      return false;
   }
   else
   {
      if (hdrLen % 4 != 0)
      {
         clog << "Bad length string " << hdrLen << endl;
         return false;
      }
		
      result.sizeValue = hdrLen;
      memcpy(&result.value, body, hdrLen);
      result.value[hdrLen] = 0;
      return true;
   }
}


static bool 
vovida_stun_ParseAtrIntegrity( char* body, unsigned int hdrLen,  VovidaStunAtrIntegrity& result )
{
   if ( hdrLen != 20)
   {
      clog << "MessageIntegrity must be 20 bytes" << endl;
      return false;
   }
   else
   {
      memcpy(&result.hash, body, hdrLen);
      return true;
   }
}


bool
vovida_stun_ParseMessage( char* buf, unsigned int bufLen, VovidaStunMessage& msg, bool verbose)
{
   if (verbose) clog << "Received vovida_stun_ message: " << bufLen << " bytes" << endl;
   memset(&msg, 0, sizeof(msg));
	
   if (sizeof(VovidaStunMsgHdr) > bufLen)
   {
      clog << "Bad message" << endl;
      return false;
   }
	
   memcpy(&msg.msgHdr, buf, sizeof(VovidaStunMsgHdr));
   msg.msgHdr.msgType = ntohs(msg.msgHdr.msgType);
   msg.msgHdr.msgLength = ntohs(msg.msgHdr.msgLength);
	
   if (msg.msgHdr.msgLength + sizeof(VovidaStunMsgHdr) != bufLen)
   {
      clog << "Message header length doesn't match message size: "
           << msg.msgHdr.msgLength << " - " << bufLen << endl;
      return false;
   }
	
   char* body = buf + sizeof(VovidaStunMsgHdr);
   unsigned int size = msg.msgHdr.msgLength;
	
   //clog << "bytes after header = " << size << endl;
	
   while ( size > 0 )
   {
      // !jf! should check that there are enough bytes left in the buffer
		
      VovidaStunAtrHdr* attr = reinterpret_cast<VovidaStunAtrHdr*>(body);
		
      unsigned int attrLen = ntohs(attr->length);
      int atrType = ntohs(attr->type);
		
      //if (verbose) clog << "Found attribute type=" << AttrNames[atrType] << " length=" << attrLen << endl;
      if ( attrLen+4 > size ) 
      {
         clog << "claims attribute is larger than size of message " 
              <<"(attribute type="<<atrType<<")"<< endl;
         return false;
      }
		
      body += 4; // skip the length and type in attribute header
      size -= 4;
		
      switch ( atrType )
      {
         case MappedAddress:
            msg.hasMappedAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.mappedAddress )== false )
            {
               clog << "problem parsing MappedAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "MappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
					
            break;  

         case ResponseAddress:
            msg.hasResponseAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.responseAddress )== false )
            {
               clog << "problem parsing ResponseAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "ResponseAddress = " << msg.responseAddress.ipv4 << endl;
            }
            break;  
				
         case ChangeRequest:
            msg.hasChangeRequest = true;
            if (vovida_stun_ParseAtrChangeRequest( body, attrLen, msg.changeRequest) == false)
            {
               clog << "problem parsing ChangeRequest" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "ChangeRequest = " << msg.changeRequest.value << endl;
            }
            break;
				
         case SourceAddress:
            msg.hasSourceAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.sourceAddress )== false )
            {
               clog << "problem parsing SourceAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "SourceAddress = " << msg.sourceAddress.ipv4 << endl;
            }
            break;  
				
         case ChangedAddress:
            msg.hasChangedAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.changedAddress )== false )
            {
               clog << "problem parsing ChangedAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "ChangedAddress = " << msg.changedAddress.ipv4 << endl;
            }
            break;  
				
         case Username: 
            msg.hasUsername = true;
            if (vovida_stun_ParseAtrString( body, attrLen, msg.username) == false)
            {
               clog << "problem parsing Username" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "Username = " << msg.username.value << endl;
            }
					
            break;
				
         case Password: 
            msg.hasPassword = true;
            if (vovida_stun_ParseAtrString( body, attrLen, msg.password) == false)
            {
               clog << "problem parsing Password" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "Password = " << msg.password.value << endl;
            }
            break;
				
         case MessageIntegrity:
            msg.hasMessageIntegrity = true;
            if (vovida_stun_ParseAtrIntegrity( body, attrLen, msg.messageIntegrity) == false)
            {
               clog << "problem parsing MessageIntegrity" << endl;
               return false;
            }
            else
            {
               //if (verbose) clog << "MessageIntegrity = " << msg.messageIntegrity.hash << endl;
            }
					
            // read the current HMAC
            // look up the password given the user of given the transaction id 
            // compute the HMAC on the buffer
            // decide if they match or not
            break;
				
         case ErrorCode:
            msg.hasErrorCode = true;
            if (vovida_stun_ParseAtrError(body, attrLen, msg.errorCode) == false)
            {
               clog << "problem parsing ErrorCode" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "ErrorCode = " << int(msg.errorCode.errorClass) 
                                 << " " << int(msg.errorCode.number) 
                                 << " " << msg.errorCode.reason << endl;
            }
					
            break;
				
         case UnknownAttribute:
            msg.hasUnknownAttributes = true;
            if (vovida_stun_ParseAtrUnknown(body, attrLen, msg.unknownAttributes) == false)
            {
               clog << "problem parsing UnknownAttribute" << endl;
               return false;
            }
            break;
				
         case ReflectedFrom:
            msg.hasReflectedFrom = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.reflectedFrom ) == false )
            {
               clog << "problem parsing ReflectedFrom" << endl;
               return false;
            }
            break;  
				
         case XorMappedAddress:
            msg.hasXorMappedAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.xorMappedAddress ) == false )
            {
               clog << "problem parsing XorMappedAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "XorMappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
            break;  

         case XorOnly:
            msg.xorOnly = true;
            if (verbose) 
            {
               clog << "xorOnly = true" << endl;
            }
            break;  
				
         case ServerName: 
            msg.hasServerName = true;
            if (vovida_stun_ParseAtrString( body, attrLen, msg.serverName) == false)
            {
               clog << "problem parsing ServerName" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "ServerName = " << msg.serverName.value << endl;
            }
            break;
				
         case SecondaryAddress:
            msg.hasSecondaryAddress = true;
            if ( vovida_stun_ParseAtrAddress(  body,  attrLen,  msg.secondaryAddress ) == false )
            {
               clog << "problem parsing secondaryAddress" << endl;
               return false;
            }
            else
            {
               if (verbose) clog << "SecondaryAddress = " << msg.secondaryAddress.ipv4 << endl;
            }
            break;  
					
         default:
            if (verbose) clog << "Unknown attribute: " << atrType << endl;
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
encodeAtrAddress4(char* ptr, UInt16 type, const VovidaStunAtrAddress4& atr)
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
encodeAtrChangeRequest(char* ptr, const VovidaStunAtrChangeRequest& atr)
{
   ptr = encode16(ptr, ChangeRequest);
   ptr = encode16(ptr, 4);
   ptr = encode32(ptr, atr.value);
   return ptr;
}

static char* 
encodeAtrError(char* ptr, const VovidaStunAtrError& atr)
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
encodeAtrUnknown(char* ptr, const VovidaStunAtrUnknown& atr)
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
encodeAtrString(char* ptr, UInt16 type, const VovidaStunAtrString& atr)
{
   assert(atr.sizeValue % 4 == 0);
	
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, atr.sizeValue);
   ptr = encode(ptr, atr.value, atr.sizeValue);
   return ptr;
}


static char* 
encodeAtrIntegrity(char* ptr, const VovidaStunAtrIntegrity& atr)
{
   ptr = encode16(ptr, MessageIntegrity);
   ptr = encode16(ptr, 20);
   ptr = encode(ptr, atr.hash, sizeof(atr.hash));
   return ptr;
}


unsigned int
vovida_stun_EncodeMessage( const VovidaStunMessage& msg, 
                   char* buf, 
                   unsigned int bufLen, 
                   const VovidaStunAtrString& password, 
                   bool verbose)
{
   assert(bufLen >= sizeof(VovidaStunMsgHdr));
   char* ptr = buf;
	
   ptr = encode16(ptr, msg.msgHdr.msgType);
   char* lengthp = ptr;
   ptr = encode16(ptr, 0);
   ptr = encode(ptr, reinterpret_cast<const char*>(msg.msgHdr.id.octet), sizeof(msg.msgHdr.id));
	
   if (verbose) clog << "Encoding vovida_stun_ message: " << endl;
   if (msg.hasMappedAddress)
   {
      if (verbose) clog << "Encoding MappedAddress: " << msg.mappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, MappedAddress, msg.mappedAddress);
   }
   if (msg.hasResponseAddress)
   {
      if (verbose) clog << "Encoding ResponseAddress: " << msg.responseAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ResponseAddress, msg.responseAddress);
   }
   if (msg.hasChangeRequest)
   {
      if (verbose) clog << "Encoding ChangeRequest: " << msg.changeRequest.value << endl;
      ptr = encodeAtrChangeRequest(ptr, msg.changeRequest);
   }
   if (msg.hasSourceAddress)
   {
      if (verbose) clog << "Encoding SourceAddress: " << msg.sourceAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, SourceAddress, msg.sourceAddress);
   }
   if (msg.hasChangedAddress)
   {
      if (verbose) clog << "Encoding ChangedAddress: " << msg.changedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ChangedAddress, msg.changedAddress);
   }
   if (msg.hasUsername)
   {
      if (verbose) clog << "Encoding Username: " << msg.username.value << endl;
      ptr = encodeAtrString(ptr, Username, msg.username);
   }
   if (msg.hasPassword)
   {
      if (verbose) clog << "Encoding Password: " << msg.password.value << endl;
      ptr = encodeAtrString(ptr, Password, msg.password);
   }
   if (msg.hasErrorCode)
   {
      if (verbose) clog << "Encoding ErrorCode: class=" 
			<< int(msg.errorCode.errorClass)  
			<< " number=" << int(msg.errorCode.number) 
			<< " reason=" 
			<< msg.errorCode.reason 
			<< endl;
		
      ptr = encodeAtrError(ptr, msg.errorCode);
   }
   if (msg.hasUnknownAttributes)
   {
      if (verbose) clog << "Encoding UnknownAttribute: ???" << endl;
      ptr = encodeAtrUnknown(ptr, msg.unknownAttributes);
   }
   if (msg.hasReflectedFrom)
   {
      if (verbose) clog << "Encoding ReflectedFrom: " << msg.reflectedFrom.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ReflectedFrom, msg.reflectedFrom);
   }
   if (msg.hasXorMappedAddress)
   {
      if (verbose) clog << "Encoding XorMappedAddress: " << msg.xorMappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, XorMappedAddress, msg.xorMappedAddress);
   }
   if (msg.xorOnly)
   {
      if (verbose) clog << "Encoding xorOnly: " << endl;
      ptr = encodeXorOnly( ptr );
   }
   if (msg.hasServerName)
   {
      if (verbose) clog << "Encoding ServerName: " << msg.serverName.value << endl;
      ptr = encodeAtrString(ptr, ServerName, msg.serverName);
   }
   if (msg.hasSecondaryAddress)
   {
      if (verbose) clog << "Encoding SecondaryAddress: " << msg.secondaryAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, SecondaryAddress, msg.secondaryAddress);
   }

   if (password.sizeValue > 0)
   {
      if (verbose) clog << "HMAC with password: " << password.value << endl;
		
      VovidaStunAtrIntegrity integrity;
      computeHmac(integrity.hash, buf, int(ptr-buf) , password.value, password.sizeValue);
      ptr = encodeAtrIntegrity(ptr, integrity);
   }
   if (verbose) clog << endl;
	
   encode16(lengthp, UInt16(ptr - buf - sizeof(VovidaStunMsgHdr)));
   return int(ptr - buf);
}

int 
vovida_stun_Rand()
{
   // return 32 bits of random stuff
   assert( sizeof(int) == 4 );
   static bool init=false;
   if ( !init )
   { 
      init = true;
		
      UInt64 tick;
		
#if defined(WIN32) 
      volatile unsigned int lowtick=0,hightick=0;
      __asm
         {
            rdtsc 
               mov lowtick, eax
               mov hightick, edx
               }
      tick = hightick;
      tick <<= 32;
      tick |= lowtick;
#elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )
      asm("rdtsc" : "=A" (tick));
#elif defined (__SUNPRO_CC) || defined( __sparc__ )	
      tick = gethrtime();
#elif defined(__MACH__) 
      int fd=open("/dev/random",O_RDONLY);
      read(fd,&tick,sizeof(tick));
      closesocket(fd);
#else
      tick=time(0);
#endif 
      int seed = int(tick);
#ifdef WIN32
      srand(seed);
#else
      srandom(seed);
#endif
   }
	
#ifdef WIN32
   assert( RAND_MAX == 0x7fff );
   int r1 = rand();
   int r2 = rand();
	
   int ret = (r1<<16) + r2;
	
   return ret;
#else
   return random(); 
#endif
}


/// return a random number to use as a port 
int
vovida_stun_RandomPort()
{
   int min=0x4000;
   int max=0x7FFF;
	
   int ret = vovida_stun_Rand();
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
vovida_stun_CreateUserName(const VovidaStunAddress4& source, VovidaStunAtrString* username)
{
   UInt64 time = vovida_stun_GetSystemTimeSecs();
   time -= (time % 20*60);
   //UInt64 hitime = time >> 32;
   UInt64 lotime = time & 0xFFFFFFFF;
	
   char buffer[1024];
   sprintf(buffer,
           "%08x:%08x:%08x:", 
           UInt32(source.addr),
           UInt32(vovida_stun_Rand()),
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
	
   //if (verbose) clog << "computed username=" << username.value << endl;
}

void
vovida_stun_CreatePassword(const VovidaStunAtrString& username, VovidaStunAtrString* password)
{
   char hmac[20];
   char key[] = "Fluffy";
   //char buffer[STUN_MAX_STRING];
   computeHmac(hmac, username.value, strlen(username.value), key, strlen(key));
   toHex(hmac, 20, password->value);
   password->sizeValue = 40;
   password->value[40]=0;
	
   //clog << "password=" << password->value << endl;
}


UInt64
vovida_stun_GetSystemTimeSecs()
{
   UInt64 time=0;
#if defined(WIN32)  
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

#if 0
ostream& operator<< ( ostream& strm, const UInt128& r )
{
   strm << int(r.octet[0]);
   for ( int i=1; i<16; i++ )
   {
      strm << ':' << int(r.octet[i]);
   }
    
   return strm;
}
#endif

ostream& 
operator<<( ostream& strm, const VovidaStunAddress4& addr)
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
vovida_stun_ParseHostName( const char* peerName,
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
	
#ifdef WIN32
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
      int err = getErrno();
      std::cerr << "error was " << err << std::endl;
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
vovida_stun_ParseServerName( const char* name, VovidaStunAddress4& addr)
{
   assert(name);
	
   // TODO - put in DNS SRV stuff.
	
   bool ret = vovida_stun_ParseHostName( name, addr.addr, addr.port, 3478); 
   if ( ret != true ) 
   {
       addr.port=0xFFFF;
   }	
   return ret;
}


static void
vovida_stun_CreateErrorResponse(VovidaStunMessage& response, int cl, int number, const char* msg)
{
   response.msgHdr.msgType = BindErrorResponseMsg;
   response.hasErrorCode = true;
   response.errorCode.errorClass = cl;
   response.errorCode.number = number;
   strcpy(response.errorCode.reason, msg);
}

#if 0
static void
vovida_stun_CreateSharedSecretErrorResponse(VovidaStunMessage& response, int cl, int number, const char* msg)
{
   response.msgHdr.msgType = SharedSecretErrorResponseMsg;
   response.hasErrorCode = true;
   response.errorCode.errorClass = cl;
   response.errorCode.number = number;
   strcpy(response.errorCode.reason, msg);
}
#endif

static void
vovida_stun_CreateSharedSecretResponse(const VovidaStunMessage& request, const VovidaStunAddress4& source, VovidaStunMessage& response)
{
   response.msgHdr.msgType = SharedSecretResponseMsg;
   response.msgHdr.id = request.msgHdr.id;
	
   response.hasUsername = true;
   vovida_stun_CreateUserName( source, &response.username);
	
   response.hasPassword = true;
   vovida_stun_CreatePassword( response.username, &response.password);
}


// This funtion takes a single message sent to a vovida_stun_ server, parses
// and constructs an apropriate repsonse - returns true if message is
// valid
bool
vovida_stun_ServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      VovidaStunAddress4& from, 
                      VovidaStunAddress4& secondary,
                      VovidaStunAddress4& myAddr,
                      VovidaStunAddress4& altAddr, 
                      VovidaStunMessage* resp,
                      VovidaStunAddress4* destination,
                      VovidaStunAtrString* hmacPassword,
                      bool* changePort,
                      bool* changeIp,
                      bool verbose)
{
    
   // set up information for default response 
	
   memset( resp, 0 , sizeof(*resp) );
	
   *changeIp = false;
   *changePort = false;
	
   VovidaStunMessage req;
   bool ok = vovida_stun_ParseMessage( buf,bufLen, req, verbose);
	
   if (!ok)      // Complete garbage, drop it on the floor
   {
      if (verbose) clog << "Request did not parse" << endl;
      return false;
   }
   if (verbose) clog << "Request parsed ok" << endl;
	
   VovidaStunAddress4 mapped = req.mappedAddress.ipv4;
   VovidaStunAddress4 respondTo = req.responseAddress.ipv4;
   UInt32 flags = req.changeRequest.value;
	
   switch (req.msgHdr.msgType)
   {
      case SharedSecretRequestMsg:
         if(verbose) clog << "Received SharedSecretRequestMsg on udp. send error 433." << endl;
         // !cj! - should fix so you know if this came over TLS or UDP
         vovida_stun_CreateSharedSecretResponse(req, from, *resp);
         //vovida_stun_CreateSharedSecretErrorResponse(*resp, 4, 33, "this request must be over TLS");
         return true;
			
      case BindRequestMsg:
         if (!req.hasMessageIntegrity)
         {
            if (verbose) clog << "BindRequest does not contain MessageIntegrity" << endl;
				
            if (0) // !jf! mustAuthenticate
            {
               if(verbose) clog << "Received BindRequest with no MessageIntegrity. Sending 401." << endl;
               vovida_stun_CreateErrorResponse(*resp, 4, 1, "Missing MessageIntegrity");
               return true;
            }
         }
         else
         {
            if (!req.hasUsername)
            {
               if (verbose) clog << "No UserName. Send 432." << endl;
               vovida_stun_CreateErrorResponse(*resp, 4, 32, "No UserName and contains MessageIntegrity");
               return true;
            }
            else
            {
               if (verbose) clog << "Validating username: " << req.username.value << endl;
               // !jf! could retrieve associated password from provisioning here
               if (strcmp(req.username.value, "test") == 0)
               {
                  if (0)
                  {
                     // !jf! if the credentials are stale 
                     vovida_stun_CreateErrorResponse(*resp, 4, 30, "Stale credentials on BindRequest");
                     return true;
                  }
                  else
                  {
                     if (verbose) clog << "Validating MessageIntegrity" << endl;
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
                        if (verbose) clog << "MessageIntegrity is bad. Sending " << endl;
                        vovida_stun_CreateErrorResponse(*resp, 4, 3, "Unknown username. Try test with password 1234");
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
                  if (verbose) clog << "Invalid username: " << req.username.value << "Send 430." << endl; 
               }
            }
         }
			
         // TODO !jf! should check for unknown attributes here and send 420 listing the
         // unknown attributes. 
			
         if ( respondTo.port == 0 ) respondTo = from;
         if ( mapped.port == 0 ) mapped = from;
				
         *changeIp   = ( flags & ChangeIpFlag )?true:false;
         *changePort = ( flags & ChangePortFlag )?true:false;
			
         if (verbose)
         {
            clog << "Request is valid:" << endl;
            clog << "\t flags=" << flags << endl;
            clog << "\t changeIp=" << *changeIp << endl;
            clog << "\t changePort=" << *changePort << endl;
            clog << "\t from = " << from << endl;
            clog << "\t respond to = " << respondTo << endl;
            clog << "\t mapped = " << mapped << endl;
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
            const char serverName[] = "Vovida.org " STUN_VERSION; // must pad to mult of 4
            
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
            vovida_stun_CreatePassword( req.username, hmacPassword );
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
         if (verbose) clog << "Unknown or unsupported request " << endl;
         return false;
   }
	
   assert(0);
   return false;
}

bool
vovida_stun_InitServer(VovidaStunServerInfo& info, const VovidaStunAddress4& myAddr,
               const VovidaStunAddress4& altAddr, int startMediaPort, bool verbose )
{
   assert( myAddr.port != 0 );
   assert( altAddr.port!= 0 );
   assert( myAddr.addr  != 0 );
   //assert( altAddr.addr != 0 );
	
   info.myAddr = myAddr;
   info.altAddr = altAddr;
	
   info.myFd = INVALID_SOCKET;
   info.altPortFd = INVALID_SOCKET;
   info.altIpFd = INVALID_SOCKET;
   info.altIpPortFd = INVALID_SOCKET;

   memset(info.relays, 0, sizeof(info.relays));
   if (startMediaPort > 0)
   {
      info.relay = true;

      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         VovidaStunMediaRelay* relay = &info.relays[i];
         relay->relayPort = startMediaPort+i;
         relay->fd = 0;
         relay->expireTime = 0;
      }
   }
   else
   {
      info.relay = false;
   }
   
   if ((info.myFd = udp_openPort(myAddr.port, myAddr.addr,verbose)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      vovida_stun_StopServer(info);

      return false;
   }
   //if (verbose) clog << "Opened " << myAddr.addr << ":" << myAddr.port << " --> " << info.myFd << endl;

   if ((info.altPortFd = udp_openPort(altAddr.port,myAddr.addr,verbose)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      vovida_stun_StopServer(info);
      return false;
   }
   //if (verbose) clog << "Opened " << myAddr.addr << ":" << altAddr.port << " --> " << info.altPortFd << endl;
   
   
   info.altIpFd = INVALID_SOCKET;
   if (  altAddr.addr != 0 )
   {
      if ((info.altIpFd = udp_openPort( myAddr.port, altAddr.addr,verbose)) == INVALID_SOCKET)
      {
         clog << "Can't open " << altAddr << endl;
         vovida_stun_StopServer(info);
         return false;
      }
      //if (verbose) clog << "Opened " << altAddr.addr << ":" << myAddr.port << " --> " << info.altIpFd << endl;;
   }
   
   info.altIpPortFd = INVALID_SOCKET;
   if (  altAddr.addr != 0 )
   {  if ((info.altIpPortFd = udp_openPort(altAddr.port, altAddr.addr,verbose)) == INVALID_SOCKET)
      {
         clog << "Can't open " << altAddr << endl;
         vovida_stun_StopServer(info);
         return false;
      }
      //if (verbose) clog << "Opened " << altAddr.addr << ":" << altAddr.port << " --> " << info.altIpPortFd << endl;;
   }
   
   return true;
}

void
vovida_stun_StopServer(VovidaStunServerInfo& info)
{
   if (info.myFd > 0) closesocket(info.myFd);
   if (info.altPortFd > 0) closesocket(info.altPortFd);
   if (info.altIpFd > 0) closesocket(info.altIpFd);
   if (info.altIpPortFd > 0) closesocket(info.altIpPortFd);
   
   if (info.relay)
   {
      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         VovidaStunMediaRelay* relay = &info.relays[i];
         if (relay->fd)
         {
            closesocket(relay->fd);
            relay->fd = 0;
         }
      }
   }
}


bool
vovida_stun_ServerProcess(VovidaStunServerInfo& info, bool verbose)
{
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = sizeof(msg);
   	
   bool ok = false;
   bool recvAltIp =false;
   bool recvAltPort = false;
	
   fd_set fdSet; 
   Socket maxFd=0;

   FD_ZERO(&fdSet); 
   FD_SET(info.myFd,&fdSet); 
   if ( info.myFd >= maxFd ) maxFd=info.myFd+1;
   FD_SET(info.altPortFd,&fdSet); 
   if ( info.altPortFd >= maxFd ) maxFd=info.altPortFd+1;

   if ( info.altIpFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpFd,&fdSet);
      if (info.altIpFd>=maxFd) maxFd=info.altIpFd+1;
   }
   if ( info.altIpPortFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpPortFd,&fdSet);
      if (info.altIpPortFd>=maxFd) maxFd=info.altIpPortFd+1;
   }

   if (info.relay)
   {
      for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
      {
         VovidaStunMediaRelay* relay = &info.relays[i];
         if (relay->fd)
         {
            FD_SET(relay->fd, &fdSet);
            if (relay->fd >= maxFd) 
			{
				maxFd=relay->fd+1;
			}
         }
      }
   }
   
   if ( info.altIpFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpFd,&fdSet);
      if (info.altIpFd>=maxFd) maxFd=info.altIpFd+1;
   }
   if ( info.altIpPortFd != INVALID_SOCKET )
   {
      FD_SET(info.altIpPortFd,&fdSet);
      if (info.altIpPortFd>=maxFd) maxFd=info.altIpPortFd+1;
   }
   
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 1000;
	
   int e = select( maxFd, &fdSet, NULL,NULL, &tv );
   if (e < 0)
   {
      int err = getErrno();
      clog << "Error on select: " << strerror(err) << endl;
   }
   else if (e >= 0)
   {
      VovidaStunAddress4 from;

      // do the media relaying
      if (info.relay)
      {
         time_t now = time(0);
         for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
         {
            VovidaStunMediaRelay* relay = &info.relays[i];
            if (relay->fd)
            {
               if (FD_ISSET(relay->fd, &fdSet))
               {
                  char msg[MAX_RTP_MSG_SIZE];
                  int msgLen = sizeof(msg);
                  
                  VovidaStunAddress4 rtpFrom;
                  ok = udp_getMessage( relay->fd, msg, &msgLen, &rtpFrom.addr, &rtpFrom.port ,verbose);
                  if (ok)
                  {
                     udp_sendMessage(info.myFd, msg, msgLen, relay->destination.addr, relay->destination.port, verbose);
                     relay->expireTime = now + MEDIA_RELAY_TIMEOUT;
                     if ( verbose ) clog << "Relay packet on " 
                                         << relay->fd 
                                         << " from " << rtpFrom 
                                         << " -> " << relay->destination 
                                         << endl;
                  }
               }
               else if (now > relay->expireTime)
               {
                  closesocket(relay->fd);
                  relay->fd = 0;
               }
            }
         }
      }
      
     
      if (FD_ISSET(info.myFd,&fdSet))
      {
         if (verbose) clog << "received on A1:P1" << endl;
         recvAltIp = false;
         recvAltPort = false;
         ok = udp_getMessage( info.myFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else if (FD_ISSET(info.altPortFd, &fdSet))
      {
         if (verbose) clog << "received on A1:P2" << endl;
         recvAltIp = false;
         recvAltPort = true;
         ok = udp_getMessage( info.altPortFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else if ( (info.altIpFd!=INVALID_SOCKET) && FD_ISSET(info.altIpFd,&fdSet))
      {
         if (verbose) clog << "received on A2:P1" << endl;
         recvAltIp = true;
         recvAltPort = false;
         ok = udp_getMessage( info.altIpFd, msg, &msgLen, &from.addr, &from.port ,verbose);
      }
      else if ( (info.altIpPortFd!=INVALID_SOCKET) && FD_ISSET(info.altIpPortFd, &fdSet))
      {
         if (verbose) clog << "received on A2:P2" << endl;
         recvAltIp = true;
         recvAltPort = true;
         ok = udp_getMessage( info.altIpPortFd, msg, &msgLen, &from.addr, &from.port,verbose );
      }
      else
      {
         return true;
      }

      int relayPort = 0;
      if (info.relay)
      {
         for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
         {
            VovidaStunMediaRelay* relay = &info.relays[i];
            if (relay->destination.addr == from.addr && 
                relay->destination.port == from.port)
            {
               relayPort = relay->relayPort;
               relay->expireTime = time(0) + MEDIA_RELAY_TIMEOUT;
               break;
            }
         }

         if (relayPort == 0)
         {
            for (int i=0; i<MAX_MEDIA_RELAYS; ++i)
            {
               VovidaStunMediaRelay* relay = &info.relays[i];
               if (relay->fd == 0)
               {
                  if ( verbose ) clog << "Open relay port " << relay->relayPort << endl;
                  
                  relay->fd = udp_openPort(relay->relayPort, info.myAddr.addr, verbose);
                  relay->destination.addr = from.addr;
                  relay->destination.port = from.port;
                  relay->expireTime = time(0) + MEDIA_RELAY_TIMEOUT;
                  relayPort = relay->relayPort;
                  break;
               }
            }
         }
      }
         
      if ( !ok ) 
      {
         if ( verbose ) clog << "Get message did not return a valid message" <<endl;
         return true;
      }
		
      if ( verbose ) clog << "Got a request (len=" << msgLen << ") from " << from << endl;
		
      if ( msgLen <= 0 )
      {
         return true;
      }
		
      bool changePort = false;
      bool changeIp = false;
		
      VovidaStunMessage resp;
      VovidaStunAddress4 dest;
      VovidaStunAtrString hmacPassword;  
      hmacPassword.sizeValue = 0;

      VovidaStunAddress4 secondary;
      secondary.port = 0;
      secondary.addr = 0;
               
      if (info.relay && relayPort)
      {
         secondary = from;
         
         from.addr = info.myAddr.addr;
         from.port = relayPort;
      }
      
      ok = vovida_stun_ServerProcessMsg( msg, msgLen, from, secondary,
                                 recvAltIp ? info.altAddr : info.myAddr,
                                 recvAltIp ? info.myAddr : info.altAddr, 
                                 &resp,
                                 &dest,
                                 &hmacPassword,
                                 &changePort,
                                 &changeIp,
                                 verbose );
		
      if ( !ok )
      {
         if ( verbose ) clog << "Failed to parse message" << endl;
         return true;
      }
		
      char buf[STUN_MAX_MESSAGE_SIZE];
      int len = sizeof(buf);
      		
      len = vovida_stun_EncodeMessage( resp, buf, len, hmacPassword,verbose );
		
      if ( dest.addr == 0 )  ok=false;
      if ( dest.port == 0 ) ok=false;
		
      if ( ok )
      {
         assert( dest.addr != 0 );
         assert( dest.port != 0 );
			
         Socket sendFd;
			
         bool sendAltIp   = recvAltIp;   // send on the received IP address 
         bool sendAltPort = recvAltPort; // send on the received port
			
         if ( changeIp )   sendAltIp   = !sendAltIp;   // if need to change IP, then flip logic 
         if ( changePort ) sendAltPort = !sendAltPort; // if need to change port, then flip logic 
			
         if ( !sendAltPort )
         {
            if ( !sendAltIp )
            {
               sendFd = info.myFd;
            }
            else
            {
               sendFd = info.altIpFd;
            }
         }
         else
         {
            if ( !sendAltIp )
            {
               sendFd = info.altPortFd;
            }
            else
            {
               sendFd = info.altIpPortFd;
            }
         }
	
         if ( sendFd != INVALID_SOCKET )
         {
            udp_sendMessage( sendFd, buf, len, dest.addr, dest.port, verbose );
         }
      }
   }
	
   return true;
}

int 
vovida_stun_FindLocalInterfaces(UInt32* addresses,int maxRet)
{
#if defined(WIN32) || defined(__sparc__)
   return 0;
#else
   struct ifconf ifc;
	
   int s = socket( AF_INET, SOCK_DGRAM, 0 );
   int len = 100 * sizeof(struct ifreq);
	
   char buf[ len ];
	
   ifc.ifc_len = len;
   ifc.ifc_buf = buf;
	
   int e = ioctl(s,SIOCGIFCONF,&ifc);
   char *ptr = buf;
   int tl = ifc.ifc_len;
   int count=0;
	
   while ( (tl > 0) && ( count < maxRet) )
   {
      struct ifreq* ifr = (struct ifreq *)ptr;
		
      int si = sizeof(ifr->ifr_name) + sizeof(struct sockaddr);
      tl -= si;
      ptr += si;
      //char* name = ifr->ifr_ifrn.ifrn_name;
      //cerr << "name = " << name << endl;
		
      struct ifreq ifr2;
      ifr2 = *ifr;
		
      e = ioctl(s,SIOCGIFADDR,&ifr2);
      if ( e == -1 )
      {
         break;
      }
		
      //cerr << "ioctl addr e = " << e << endl;
		
      struct sockaddr a = ifr2.ifr_addr;
      struct sockaddr_in* addr = (struct sockaddr_in*) &a;
		
      UInt32 ai = ntohl( addr->sin_addr.s_addr );
      if (int((ai>>24)&0xFF) != 127)
      {
         addresses[count++] = ai;
      }
		
#if 0
      cerr << "Detected interface "
           << int((ai>>24)&0xFF) << "." 
           << int((ai>>16)&0xFF) << "." 
           << int((ai>> 8)&0xFF) << "." 
           << int((ai    )&0xFF) << endl;
#endif
   }
	
   closesocket(s);
	
   return count;
#endif
}


void
vovida_stun_BuildReqSimple( VovidaStunMessage* msg,
                    const VovidaStunAtrString& username,
                    bool changePort, bool changeIp, unsigned int id )
{
   assert( msg );
   memset( msg , 0 , sizeof(*msg) );
	
   msg->msgHdr.msgType = BindRequestMsg;
	
   for ( int i=0; i<16; i=i+4 )
   {
      assert(i+3<16);
      int r = vovida_stun_Rand();
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


static void 
vovida_stun_SendTest( Socket myFd, VovidaStunAddress4& dest, 
              const VovidaStunAtrString& username, const VovidaStunAtrString& password, 
              int testNum, bool verbose )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );
	
   bool changePort=false;
   bool changeIP=false;
	
   switch (testNum)
   {
      case 1:
      case 10:
      case 11:
         break;
      case 2:
         //changePort=true;
         changeIP=true;
         break;
      case 3:
         changePort=true;
         break;
      case 4:
         changeIP=true;
         break;
      case 5:
         break;
      default:
         cerr << "Test " << testNum <<" is unkown\n";
         assert(0);
   }
	
   VovidaStunMessage req;
   memset(&req, 0, sizeof(VovidaStunMessage));
	
   vovida_stun_BuildReqSimple( &req, username, 
                       changePort , changeIP , 
                       testNum );
	
   char buf[STUN_MAX_MESSAGE_SIZE];
   int len = STUN_MAX_MESSAGE_SIZE;
	
   len = vovida_stun_EncodeMessage( req, buf, len, password,verbose );
	
   if ( verbose )
   {
      clog << "About to send msg of len " << len << " to " << dest << endl;
   }
	
   udp_sendMessage( myFd, buf, len, dest.addr, dest.port, verbose );
	
   // add some delay so the packets don't get sent too quickly 
#ifdef WIN32 // !cj! TODO - should fix this up in windows
		 clock_t now = clock();
		 assert( CLOCKS_PER_SEC == 1000 );
		 while ( clock() <= now+10 ) { };
#else
		 usleep(10*1000);
#endif

}


void 
vovida_stun_GetUserNameAndPassword(  const VovidaStunAddress4& dest, 
                             VovidaStunAtrString* username,
                             VovidaStunAtrString* password)
{ 
   // !cj! This is totally bogus - need to make TLS connection to dest and get a
   // username and password to use 
   vovida_stun_CreateUserName(dest, username);
   vovida_stun_CreatePassword(*username, password);
}


void 
vovida_stun_Test( VovidaStunAddress4& dest, int testNum, bool verbose, VovidaStunAddress4* sAddr )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );
	
   int port = vovida_stun_RandomPort();
   UInt32 interfaceIp=0;
   if (sAddr)
   {
      interfaceIp = sAddr->addr;
      if ( sAddr->port != 0 )
      {
        port = sAddr->port;
      }
   }
   Socket myFd = udp_openPort(port,interfaceIp,verbose);
	
   VovidaStunAtrString username;
   VovidaStunAtrString password;
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   vovida_stun_GetUserNameAndPassword( dest, username, password );
#endif
	
   vovida_stun_SendTest( myFd, dest, username, password, testNum, verbose );
    
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = STUN_MAX_MESSAGE_SIZE;
	
   VovidaStunAddress4 from;
   udp_getMessage( myFd,
               msg,
               &msgLen,
               &from.addr,
               &from.port,verbose );
	
   VovidaStunMessage resp;
   memset(&resp, 0, sizeof(VovidaStunMessage));
	
   if ( verbose ) clog << "Got a response" << endl;
   vovida_stun_ParseMessage( msg,msgLen, resp,verbose );
#if 0
   if ( verbose )
   {
      clog << "\t ok=" << ok << endl;
      clog << "\t id=" << resp.msgHdr.id << endl;
      clog << "\t mappedAddr=" << resp.mappedAddress.ipv4 << endl;
      clog << "\t changedAddr=" << resp.changedAddress.ipv4 << endl;
      clog << endl;
   }
#endif	
   if (sAddr)
   {
      sAddr->port = resp.mappedAddress.ipv4.port;
      sAddr->addr = resp.mappedAddress.ipv4.addr;
   }
}


NatType
vovida_stun_NatType( VovidaStunAddress4& dest, 
             bool verbose,
             bool* preservePort, // if set, is return for if NAT preservers ports or not
             bool* hairpin,  // if set, is the return for if NAT will hairpin packets
             int port, // port to use for the test, 0 to choose random port
             VovidaStunAddress4* sAddr // NIC to use 
   )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );
	
   if ( hairpin ) 
   {
      *hairpin = false;
   }
	
   if ( port == 0 )
   {
      port = vovida_stun_RandomPort();
   }
   UInt32 interfaceIp=0;
   if (sAddr)
   {
      interfaceIp = sAddr->addr;
   }
   Socket myFd1 = udp_openPort(port,interfaceIp,verbose);
   Socket myFd2 = udp_openPort(port+1,interfaceIp,verbose);

   if ( ( myFd1 == INVALID_SOCKET) || ( myFd2 == INVALID_SOCKET) )
   {
        cerr << "Some problem opening port/interface to send on" << endl;
       return VovidaStunTypeFailure; 
   }

   assert( myFd1 != INVALID_SOCKET );
   assert( myFd2 != INVALID_SOCKET );
    
   bool respTestI=false;
   bool isNat=true;
   VovidaStunAddress4 testImappedAddr;
   bool respTestI2=false; 
   bool mappedIpSame = true;
   VovidaStunAddress4 testI2mappedAddr;
   VovidaStunAddress4 testI2dest=dest;
   bool respTestII=false;
   bool respTestIII=false;

   bool respTestHairpin=false;
   bool respTestPreservePort=false;
	
   memset(&testImappedAddr,0,sizeof(testImappedAddr));
	
   VovidaStunAtrString username;
   VovidaStunAtrString password;
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS 
   vovida_stun_GetUserNameAndPassword( dest, username, password );
#endif
	
   int count=0;
   while ( count < 7 )
   {
      struct timeval tv;
      fd_set fdSet; 
#ifdef WIN32
      unsigned int fdSetSize;
#else
      int fdSetSize;
#endif
      FD_ZERO(&fdSet); fdSetSize=0;
      FD_SET(myFd1,&fdSet); fdSetSize = (myFd1+1>fdSetSize) ? myFd1+1 : fdSetSize;
      FD_SET(myFd2,&fdSet); fdSetSize = (myFd2+1>fdSetSize) ? myFd2+1 : fdSetSize;
      tv.tv_sec=0;
      tv.tv_usec=150*1000; // 150 ms 
      if ( count == 0 ) tv.tv_usec=0;
		
      int  err = select(fdSetSize, &fdSet, NULL, NULL, &tv);
      int e = getErrno();
      if ( err == SOCKET_ERROR )
      {
         // error occured
         cerr << "Error " << e << " " << strerror(e) << " in select" << endl;
        return VovidaStunTypeFailure; 
     }
      else if ( err == 0 )
      {
         // timeout occured 
         count++;
			
         if ( !respTestI ) 
         {
            vovida_stun_SendTest( myFd1, dest, username, password, 1 ,verbose );
         }         
			
         if ( (!respTestI2) && respTestI ) 
         {
            // check the address to send to if valid 
            if (  ( testI2dest.addr != 0 ) &&
                  ( testI2dest.port != 0 ) )
            {
               vovida_stun_SendTest( myFd1, testI2dest, username, password, 10  ,verbose);
            }
         }
			
         if ( !respTestII )
         {
            vovida_stun_SendTest( myFd2, dest, username, password, 2 ,verbose );
         }
			
         if ( !respTestIII )
         {
            vovida_stun_SendTest( myFd2, dest, username, password, 3 ,verbose );
         }
			
         if ( respTestI && (!respTestHairpin) )
         {
            if (  ( testImappedAddr.addr != 0 ) &&
                  ( testImappedAddr.port != 0 ) )
            {
               vovida_stun_SendTest( myFd1, testImappedAddr, username, password, 11 ,verbose );
            }
         }
      }
      else
      {
         //if (verbose) clog << "-----------------------------------------" << endl;
         assert( err>0 );
         // data is avialbe on some fd 
			
         for ( int i=0; i<2; i++)
         {
            Socket myFd;
            if ( i==0 ) 
            {
               myFd=myFd1;
            }
            else
            {
               myFd=myFd2;
            }
				
            if ( myFd!=INVALID_SOCKET ) 
            {					
               if ( FD_ISSET(myFd,&fdSet) )
               {
                  char msg[STUN_MAX_MESSAGE_SIZE];
                  int msgLen = sizeof(msg);
                  						
                  VovidaStunAddress4 from;
						
                  udp_getMessage( myFd,
                              msg,
                              &msgLen,
                              &from.addr,
                              &from.port,verbose );
						
                  VovidaStunMessage resp;
                  memset(&resp, 0, sizeof(VovidaStunMessage));
						
                  vovida_stun_ParseMessage( msg,msgLen, resp,verbose );
						
                  if ( verbose )
                  {
                     clog << "Received message of type " << resp.msgHdr.msgType 
                          << "  id=" << (int)(resp.msgHdr.id.octet[0]) << endl;
                  }
						
                  switch( resp.msgHdr.id.octet[0] )
                  {
                     case 1:
                     {
                        if ( !respTestI )
                        {
								
                           testImappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testImappedAddr.port = resp.mappedAddress.ipv4.port;
			
                           respTestPreservePort = ( testImappedAddr.port == port ); 
                           if ( preservePort )
                           {
                              *preservePort = respTestPreservePort;
                           }								
									
                           testI2dest.addr = resp.changedAddress.ipv4.addr;
									
                           if (sAddr)
                           {
                              sAddr->port = testImappedAddr.port;
                              sAddr->addr = testImappedAddr.addr;
                           }
									
                           count = 0;
                        }		
                        respTestI=true;
                     }
                     break;
                     case 2:
                     {  
                        respTestII=true;
                     }
                     break;
                     case 3:
                     {
                        respTestIII=true;
                     }
                     break;
                     case 10:
                     {
                        if ( !respTestI2 )
                        {
                           testI2mappedAddr.addr = resp.mappedAddress.ipv4.addr;
                           testI2mappedAddr.port = resp.mappedAddress.ipv4.port;
								
                           mappedIpSame = false;
                           if ( (testI2mappedAddr.addr  == testImappedAddr.addr ) &&
                                (testI2mappedAddr.port == testImappedAddr.port ))
                           { 
                              mappedIpSame = true;
                           }
								
							
                        }
                        respTestI2=true;
                     }
                     break;
                     case 11:
                     {
							
                        if ( hairpin ) 
                        {
                           *hairpin = true;
                        }
                        respTestHairpin = true;
                     }
                     break;
                  }
               }
            }
         }
      }
   }
	
   // see if we can bind to this address 
   //cerr << "try binding to " << testImappedAddr << endl;
   Socket s = udp_openPort( 0/*use ephemeral*/, testImappedAddr.addr, false );
   if ( s != INVALID_SOCKET )
   {
      closesocket(s);
      isNat = false;
      //cerr << "binding worked" << endl;
   }
   else
   {
      isNat = true;
      //cerr << "binding failed" << endl;
   }
	
   if (verbose)
   {
      clog << "test I = " << respTestI << endl;
      clog << "test II = " << respTestII << endl;
      clog << "test III = " << respTestIII << endl;
      clog << "test I(2) = " << respTestI2 << endl;
      clog << "is nat  = " << isNat <<endl;
      clog << "mapped IP same = " << mappedIpSame << endl;
      clog << "hairpin = " << respTestHairpin << endl;
      clog << "preserver port = " << respTestPreservePort << endl;
   }
	
#if 0
   // implement logic flow chart from draft RFC
   if ( respTestI )
   {
      if ( isNat )
      {
         if (respTestII)
         {
            return VovidaStunTypeConeNat;
         }
         else
         {
            if ( mappedIpSame )
            {
               if ( respTestIII )
               {
                  return VovidaStunTypeRestrictedNat;
               }
               else
               {
                  return VovidaStunTypePortRestrictedNat;
               }
            }
            else
            {
               return VovidaStunTypeSymNat;
            }
         }
      }
      else
      {
         if (respTestII)
         {
            return VovidaStunTypeOpen;
         }
         else
         {
            return VovidaStunTypeSymFirewall;
         }
      }
   }
   else
   {
      return VovidaStunTypeBlocked;
   }
#else
   if ( respTestI ) // not blocked 
   {
      if ( isNat )
      {
         if ( mappedIpSame )
         {
            if (respTestII)
            {
               return VovidaStunTypeIndependentFilter;
            }
            else
            {
               if ( respTestIII )
               {
                  return VovidaStunTypeDependentFilter;
               }
               else
               {
                  return VovidaStunTypePortDependedFilter;
               }
            }
         }
         else // mappedIp is not same 
         {
            return VovidaStunTypeDependentMapping;
         }
      }
      else  // isNat is false
      {
         if (respTestII)
         {
            return VovidaStunTypeOpen;
         }
         else
         {
            return VovidaStunTypeFirewall;
         }
      }
   }
   else
   {
      return VovidaStunTypeBlocked;
   }
#endif
	
   return VovidaStunTypeUnknown;
}


int
vovida_stun_OpenSocket( VovidaStunAddress4& dest, VovidaStunAddress4* mapAddr, 
                int port, VovidaStunAddress4* srcAddr, 
                bool verbose )
{
   assert( dest.addr != 0 );
   assert( dest.port != 0 );
   assert( mapAddr );
   
   if ( port == 0 )
   {
      port = vovida_stun_RandomPort();
   }
   unsigned int interfaceIp = 0;
   if ( srcAddr )
   {
      interfaceIp = srcAddr->addr;
   }
   
   Socket myFd = udp_openPort(port,interfaceIp,verbose);
   if (myFd == INVALID_SOCKET)
   {
      return myFd;
   }
   
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = sizeof(msg);
	
   VovidaStunAtrString username;
   VovidaStunAtrString password;
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   vovida_stun_GetUserNameAndPassword( dest, username, password );
#endif
	
   vovida_stun_SendTest(myFd, dest, username, password, 1, 0/*false*/ );
	
   VovidaStunAddress4 from;
	
   udp_getMessage( myFd, msg, &msgLen, &from.addr, &from.port,verbose );
	
   VovidaStunMessage resp;
   memset(&resp, 0, sizeof(VovidaStunMessage));
	
   bool ok = vovida_stun_ParseMessage( msg, msgLen, resp,verbose );
   if (!ok)
   {
      return -1;
   }
	
   VovidaStunAddress4 mappedAddr = resp.mappedAddress.ipv4;

   //clog << "--- vovida_stun_OpenSocket --- " << endl;
   //clog << "\treq  id=" << req.id << endl;
   //clog << "\tresp id=" << id << endl;
   //clog << "\tmappedAddr=" << mappedAddr << endl;
	
   *mapAddr = mappedAddr;
	
   return myFd;
}


bool
vovida_stun_OpenSocketPair( VovidaStunAddress4& dest, VovidaStunAddress4* mapAddr, 
                    int* fd1, int* fd2, 
                    int port, VovidaStunAddress4* srcAddr, 
                    bool verbose )
{
   assert( dest.addr!= 0 );
   assert( dest.port != 0 );
   assert( mapAddr );
   
   const int NUM=3;
	
   if ( port == 0 )
   {
      port = vovida_stun_RandomPort();
   }
	
   *fd1=-1;
   *fd2=-1;
	
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen =sizeof(msg);
	
   VovidaStunAddress4 from;
   int fd[NUM];
   int i;
	
   unsigned int interfaceIp = 0;
   if ( srcAddr )
   {
      interfaceIp = srcAddr->addr;
   }

   for( i=0; i<NUM; i++)
   {
      fd[i] = udp_openPort( (port == 0) ? 0 : (port + i), 
                        interfaceIp, verbose);
      if (fd[i] < 0) 
      {
         while (i > 0)
         {
            closesocket(fd[--i]);
         }
         return false;
      }
   }
	
   VovidaStunAtrString username;
   VovidaStunAtrString password;
	
   username.sizeValue = 0;
   password.sizeValue = 0;
	
#ifdef USE_TLS
   vovida_stun_GetUserNameAndPassword( dest, username, password );
#endif
	
   for( i=0; i<NUM; i++)
   {
      vovida_stun_SendTest(fd[i], dest, username, password, 1/*testNum*/, verbose );
   }
	
   VovidaStunAddress4 mappedAddr[NUM];
   for( i=0; i<NUM; i++)
   {
      msgLen = sizeof(msg)/sizeof(*msg);
      udp_getMessage( fd[i],
                  msg,
                  &msgLen,
                  &from.addr,
                  &from.port ,verbose);
		
      VovidaStunMessage resp;
      memset(&resp, 0, sizeof(VovidaStunMessage));
		
      bool ok = vovida_stun_ParseMessage( msg, msgLen, resp, verbose );
      if (!ok) 
      {
         return false;
      }
		
      mappedAddr[i] = resp.mappedAddress.ipv4;
   }
	
   if (verbose)
   {               
      clog << "--- vovida_stun_OpenSocketPair --- " << endl;
      for( i=0; i<NUM; i++)
      {
         clog << "\t mappedAddr=" << mappedAddr[i] << endl;
      }
   }
	
   if ( mappedAddr[0].port %2 == 0 )
   {
      if (  mappedAddr[0].port+1 ==  mappedAddr[1].port )
      {
         *mapAddr = mappedAddr[0];
         *fd1 = fd[0];
         *fd2 = fd[1];
         closesocket( fd[2] );
         return true;
      }
   }
   else
   {
      if (( mappedAddr[1].port %2 == 0 )
          && (  mappedAddr[1].port+1 ==  mappedAddr[2].port ))
      {
         *mapAddr = mappedAddr[1];
         *fd1 = fd[1];
         *fd2 = fd[2];
         closesocket( fd[0] );
         return true;
      }
   }

   // something failed, close all and return error
   for( i=0; i<NUM; i++)
   {
      closesocket( fd[i] );
   }
	
   return false;
}

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


