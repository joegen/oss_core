// OSS Software Solutions Application Programmer Interface
// Package: SBC
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

#ifndef SBCACCOUNTS_H_INCLUDED
#define	SBCACCOUNTS_H_INCLUDED


#include "OSS/SIP/SBC/SBCWorkSpaceManager.h"
#include "OSS/SIP/SBC/SBCAccountRecord.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
class SBCAccounts
{
public:
  typedef std::map<std::string, SBCAccountRecord> VolatileAccounts;
  typedef std::set<std::string> Realms;
  SBCAccounts();
  
  ~SBCAccounts();
  
  void initialize(const SBCWorkSpaceManager::WorkSpace& workspace);
  
  const SBCWorkSpaceManager::WorkSpace& workspace() const;
  
  bool findAccount(const std::string& identity, SBCAccountRecord& account) const;
  
  bool addAccount(SBCAccountRecord& account);
  
  bool addVolatileAccount(SBCAccountRecord& account);
  
  bool isKnownRealm(const std::string& realm) const;
  
  bool isKnownIdentity(const std::string& identity) const;
  
  void addRealm(const std::string& realm);
protected:
  void determineRealms();
private:
  SBCWorkSpaceManager::WorkSpace _workspace;
  VolatileAccounts _volatileAccounts;
  mutable OSS::mutex_critic_sec _volatileAccountsMutex;
  Realms _realms;
  bool _hasDeterminedRealms;
  
};
  

//
// Inlines
//

inline const SBCWorkSpaceManager::WorkSpace& SBCAccounts::workspace() const
{
  return _workspace;
}

} } }  // OSS::SIP::SBC

#endif	// SBCACCOUNTS_H_INCLUDED

