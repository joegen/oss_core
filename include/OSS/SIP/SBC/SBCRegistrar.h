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

#ifndef SBCREGISTRAR_H_INCLUDED
#define	SBCREGISTRAR_H_INCLUDED

#include "OSS/EP/EndpointListener.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/SBC/SBCWorkSpaceManager.h"
#include "OSS/SIP/SBC/SBCRegistrationRecord.h"


namespace OSS {
namespace SIP {
namespace SBC {

  
using OSS::EP::EndpointListener;
using OSS::EP::EndpointConnection;
using OSS::SIP::B2BUA::SIPB2BTransaction;

class SBCManager;

class SBCRegistrar : public EndpointListener
{
public:
  typedef std::set<SBCRegistrationRecord> Registrations;
  
  enum EventType
  {
    REG_UPDATE,
    REG_QUERY,
    REG_UNREG,
    REG_INVALID
  };
  
  SBCRegistrar();
  
  virtual ~SBCRegistrar();
  
  virtual void handleStart();
    /// handle a start request.  This should not block. 
  
  virtual void handleStop();
    /// handle a stop request.  This should not block

  void setDatabase(const SBCWorkSpaceManager::WorkSpace& regDb);
    /// Set the database/workspace to be used by registrar
  
  bool getBindings(const SIPURI& aor, ContactList& bindings);
    /// return the current bindings for the aor
  
  bool getRegistrations(const std::string& identity, Registrations& registrations);
    /// return the current registrations for the identity
  
  bool onRouteLocalReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
  
  SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
  
  void attachSBCManager(SBCManager* pManager);
  
protected:
  virtual void onHandleEvent(const SIPMessage::Ptr& pRequest);
    /// handle an incoming SIP event
  
  void handleQuery(const SIPMessage::Ptr& pRequest, const SIPTo& to);
  void handleUnreg(const SIPMessage::Ptr& pRequest, const SIPTo& to, ContactList& contactList);
  void handleUpdate(const SIPMessage::Ptr& pRequest, const SIPTo& to, const ContactList& contactList, const std::string& expires);
  
  void updateBinding(const SIPMessage::Ptr& pRequest, const SIPTo& to, const ContactURI& binding, const std::string& expires);
  void removeBinding(const SIPMessage::Ptr& pRequest, const SIPTo& to, const std::string& binding);
  
  bool storeBinding(const std::string& key, const SBCRegistrationRecord& binding);
  
  void dispatchContacts(const SIPMessage::Ptr& pRequest, const SIPURI& aor);
private:
  SBCWorkSpaceManager::WorkSpace _regDb;
  SBCManager* _pManager;
};
  
//
// Inlines
//

inline void SBCRegistrar::setDatabase(const SBCWorkSpaceManager::WorkSpace& regDb)
{
  _regDb = regDb;
}

} } }  // OSS::SIP::SBC


#endif	// SBCREGISTRAR_H_INCLUDED

