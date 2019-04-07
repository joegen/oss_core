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

#include "OSS/SIP/SIPDialogSetPool.h"

namespace OSS {
namespace SIP {


SIPDialogSetPool::SIPDialogSetPool(Type type, Direction direction) :
  _direction(direction),
  _type(type)
{
}

SIPDialogSetPool::~SIPDialogSetPool()
{
}

SIPDialogSet::Ptr SIPDialogSetPool::findDialogSet(const std::string& dialogSetId)
{
  OSS::mutex_critic_sec_lock lock(_dialogsMutex);
  DialogSets::const_iterator iter = _dialogs.find(dialogSetId);
  if (iter == _dialogs.end())
  {
    return SIPDialogSet::Ptr();
  }
  return iter->second;
}

void SIPDialogSetPool::removeDialogSet(const std::string& dialogSetId)
{
  OSS::mutex_critic_sec_lock lock(_dialogsMutex);
  _dialogs.erase(dialogSetId);
}

SIPDialogSet::Ptr SIPDialogSetPool::createDialogSet(const std::string& dialogSetId)
{
  OSS::mutex_critic_sec_lock lock(_dialogsMutex);
  DialogSets::const_iterator iter = _dialogs.find(dialogSetId);
  assert(iter == _dialogs.end());
  SIPDialogSet::Ptr dialog = SIPDialogSet::Ptr(new SIPDialogSet(_type, _direction));
  _dialogs[dialogSetId] = dialog;
  return dialog;
}
  
} }