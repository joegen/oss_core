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

#ifndef OSS_SIPDIALOGSETPOOL_H_INCLUDED
#define OSS_SIPDIALOGSETPOOL_H_INCLUDED

#include "OSS/SIP/SIPDialogSet.h"

namespace OSS {
namespace SIP {
  
class SIPDialogSetPool : boost::noncopyable
{
public:
  typedef SIPDialog::Direction Direction;
  typedef SIPDialog::Type Type;
  typedef std::map<std::string, SIPDialogSet::Ptr> DialogSets;
  
  SIPDialogSetPool(Type type, Direction direction);
  ~SIPDialogSetPool();
  
  Type getType() const;
  Direction getDirection() const;
  
  SIPDialogSet::Ptr findDialogSet(const std::string& dialogSetId);
  void removeDialogSet(const std::string& dialogSetId);

protected:
  SIPDialogSet::Ptr createDialogSet(const std::string& dialogSetId);
private:
  Direction _direction;
  Type _type;
  OSS::mutex_critic_sec _dialogsMutex;
  DialogSets _dialogs;
};

//
// Inlines 
//

inline SIPDialogSetPool::Type SIPDialogSetPool::getType() const
{
  return _type;
}


inline SIPDialogSetPool::Direction SIPDialogSetPool::getDirection() const
{
  return _direction;
}

} } // OSS::SIP

#endif // OSS_SIPDIALOGSETPOOL_H_INCLUDED

