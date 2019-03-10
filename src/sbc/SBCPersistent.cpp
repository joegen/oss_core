
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


#include "OSS/SIP/SBC/SBCPersistent.h"
#include "OSS/Persistent/ClassType.h"



using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;


namespace OSS {
namespace SIP {
namespace SBC {

void SBCPersistent::persistTransactionState(const OSS::SIP::SIPMessage::Ptr& pUASRequest,
    const std::string& routeFile,
    const boost::filesystem::path& stateDirectory)
{
}

void SBCPersistent::persistToDialogState(const OSS::SIP::SIPMessage::Ptr& pUASRequest,
  const OSS::SIP::SIPMessage::Ptr& pUASResponse,
  const OSS::SIP::SIPMessage::Ptr& pUACRequest,
  const OSS::SIP::SIPMessage::Ptr& pUACResponse,
  const boost::filesystem::path& stateDirectory)
{
}

} } } // OSS::SIP::SBC
