// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCExecBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

SBCExecBehavior::SBCExecBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_EXEC, "SBC EXEC Request Handler")
{
  setName("SBC EXEC Request Handler");
}

SBCExecBehavior::~SBCExecBehavior()
 {

}


SIPMessage::Ptr SBCExecBehavior::onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  std::string cmd = pRequest->hdrGet("cmd");
  std::string args = pRequest->hdrGet("args");
  std::ostringstream output;

  bool ok = false;
  ok = dynamic_cast<SBCManager*>(_pManager)->execProc(cmd, args, output);

  std::string content = output.str();
  SIPMessage::Ptr response;
  if (ok)
  {
    response = pRequest->createResponse(200);
    if (!content.empty())
    {
      response->hdrSet("Content-Length", OSS::string_from_number<size_t>(content.size()));
      response->hdrSet("Content-Type", "text/plain");
      response->body() = content;
      response->commitData();
    }
  }
  else
  {
    response = pRequest->createResponse(500, "EXEC Server Error");
  }
  return response;
}

} } } // OSS::SIP::SBC




