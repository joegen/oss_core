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

#define SIPQ_INTERNAL 1

#include "OSS/SIP/SIPQ/sipq.h"
#include "OSS/SIP/SIPQ/SIPQEndpoint.h"
#include "OSS/SIP/SIPURI.h"


using namespace OSS;
using namespace OSS::SIP;
using namespace OSS::SIP::EP;


struct sipq_context_t
{
  sip_queue_type type; // Queue type
  sip_queue_transport transport; // Transport (INPROC or NETWORK)
  std::string local_uri; // the local address URI
  std::string remote_uri;
  SIPQEndpoint* endpoint;
  void* user_data;
};

static bool sipq_bind(sipq_context_t* ctx)
{
  return ctx->endpoint->bindToAddress(ctx->local_uri.c_str());
}

sipq_context_t* sipq_create(sip_queue_type type, const char* local_uri, void* user_data)
{
  sipq_context_t* ctx = new sipq_context_t();
  ctx->type = type;
  ctx->endpoint = new SIPQEndpoint(ctx);
  ctx->user_data = user_data;
  ctx->local_uri = local_uri;
  
  if (!sipq_bind(ctx))
  {
    sipq_destroy(ctx);
    ctx = 0;
  }
  return ctx;
}

void sipq_destroy(sipq_context_t* ctx)
{
  delete ctx->endpoint;
  delete ctx;
}

int sipq_connect(sipq_context_t* ctx, const char* remote_uri)
{
  return SIPQ_OK;
}

std::size_t sipq_read(sipq_context_t* ctx, void* buff)
{
  return ctx->endpoint->q_read(buff);
}

std::size_t sipq_write(sipq_context_t* ctx, void* buff, std::size_t buff_len)
{
  return ctx->endpoint->q_write(buff, buff_len);
}




