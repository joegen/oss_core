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

#ifndef OSS_SIPDIALOGSET_H_INCLUDED
#define OSS_SIPDIALOGSET_H_INCLUDED

#include "OSS/SIP/SIPDialog.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace SIP {

class SIPDialogSet : boost::noncopyable
{
public:
  typedef boost::shared_ptr<SIPDialogSet> Ptr;
  typedef boost::weak_ptr<SIPDialogSet> WeakPtr;
  typedef std::map<std::string, SIPDialog::Ptr> Dialogs;
  typedef SIPDialog::Direction Direction;
  typedef SIPDialog::Type Type;
  typedef SIPDialog::State State;

  SIPDialogSet(Type type, Direction direction);
  ~SIPDialogSet();
  
  Type getType() const;
  State getState() const;
  Direction getDirection() const;

  SIPDialog::Ptr findDialog(const std::string& dialogId);
  void removeDialog(const std::string& dialogId);

protected:
  SIPDialog::Ptr createDialog(const std::string& dialogId);

private:
  Direction _direction;
  State _state;
  Type _type;
  
  std::string _callId;
  OSS::mutex_critic_sec _dialogsMutex;
  Dialogs _dialogs;
};


//
// Inlines 
//

inline SIPDialogSet::Type SIPDialogSet::getType() const
{
  return _type;
}

inline SIPDialogSet::State SIPDialogSet::getState() const
{
  return _state;
}

inline SIPDialogSet::Direction SIPDialogSet::getDirection() const
{
  return _direction;
}

} }

#endif // OSS_SIPDIALOGSET_H_INCLUDED

