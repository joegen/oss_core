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

#include "OSS/SIP/SIPDialogSet.h"

namespace OSS {
namespace SIP {

 
SIPDialogSet::SIPDialogSet(Type type, Direction direction) :
  _direction(direction),
  _state(SIPDialog::INIT),
  _type(type)
{
}

SIPDialogSet::~SIPDialogSet()
{
}

 SIPDialog::Ptr SIPDialogSet::findDialog(const std::string& dialogId)
 {
   OSS::mutex_critic_sec_lock lock(_dialogsMutex);
   Dialogs::const_iterator iter = _dialogs.find(dialogId);
   if (iter == _dialogs.end())
   {
     return SIPDialog::Ptr();
   }
   return iter->second;
}
 
void SIPDialogSet::removeDialog(const std::string& dialogId)
{
  OSS::mutex_critic_sec_lock lock(_dialogsMutex);
  _dialogs.erase(dialogId);
}

SIPDialog::Ptr SIPDialogSet::createDialog(const std::string& dialogId)
{
   OSS::mutex_critic_sec_lock lock(_dialogsMutex);
   Dialogs::const_iterator iter = _dialogs.find(dialogId);
   assert(iter == _dialogs.end());
   SIPDialog::Ptr dialog = SIPDialog::Ptr(new SIPDialog(_direction));
   _dialogs[dialogId] = dialog;
   return dialog;
}
  
} } // OSS::SIP
 