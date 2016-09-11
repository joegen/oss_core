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

#ifndef OSS_SIPQ_H_INCLUDED
#define	OSS_SIPQ_H_INCLUDED

#include <string>


enum sip_queue_type
{
  SIPQ_REQ, // A client queue that can send request
  SIPQ_REP, // A server queue that can send responses.  Can server multiple clients
  SIPQ_PULL, // A client queue that can send pull request
  SIPQ_PUSH, // A server queue that can send push request.  Can server mutiple clients
  SIPQ_PUB, // A queue that publishes events to subscribers
  SIPQ_SUB,  // A subscriber queue receiving events from a publisher
  SIPQ_REGS,  // A service location queue
  SIPQ_REGC // A service location client
};

enum sip_queue_transport
{
  SIPQ_INPROC, // In process transport
  SIPQ_NETWORK // Network transport using TCP and UDP
};

enum sipq_ret
{
  SIPQ_OK,
  SIPQ_TIMEOUT,
  SIPQ_NOT_CONNECTED
};


//
// the sipq contxt structure
//
struct sipq_context_t;

//
// Create a new sipq context
// Params:
//    local_uri - the local uri this queue will listen for requests
//        schemes are inproc and net.  Example:  inproc:myqueue or 
//        net:localhost:9000
//    user_data - optional user data
// Return: The newly allocated context.  Must be destroyed using sipq_destroy()
// 
sipq_context_t* sipq_create(sip_queue_type type, const char* local_uri, void* user_data = 0);

//
// Destroy the sipq context created by sipq_create()
// Params:
//    ctx - the sipq context to destroy
//
//
void sipq_destroy(sipq_context_t* ctx);

//
// Connect the local queue to a remote queue
// Params:
//    ctx - The sipq context
//    remote_uri - the address of the remote queue
//        Example:  net:localhost:9002 or inproc:remotequeue
// Return:  SIPQ_OK if successful
//
int sipq_connect(sipq_context_t* ctx, const char* remote_uri);

//
// Read from a sipq context
// Params:
//    ctx - the sipq context
//    buff - buffer containing the data read
// Return:  size of the buffer read
//
std::size_t sipq_read(sipq_context_t* ctx, void* buff);

//
// Write to a sipq context
// Params:
//    ctx - the sipq context
//    buff - buffer containing the data to write
//    buff_len - size of the buffer 
// Return:  size of the data written
//
std::size_t sipq_write(sipq_context_t* ctx, void* buff, std::size_t buff_len);


#endif	// OSS_SIPQ_H_INCLUDED

