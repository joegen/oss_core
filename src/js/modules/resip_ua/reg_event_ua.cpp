#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rutil/Logger.hxx"
#include "resip/stack/SipMessage.hxx"
#include "resip/stack/OctetContents.hxx"
#include "resip/dum/MasterProfile.hxx"
#include "resip/dum/ClientAuthManager.hxx"
#include "resip/dum/ClientSubscription.hxx"

#ifdef USE_SSL
#include "resip/stack/ssl/Security.hxx"
#endif

#include "reg_event_ua.h"

using namespace resip;

#define RESIPROCATE_SUBSYSTEM Subsystem::TEST

static Token regEvent("message-summary");

AddAor::AddAor(RegEventClient& client, const resip::Uri& aor) : 
   mClient(client),
   mAor(aor)
{
}

void
AddAor::executeCommand()
{
   //InfoLog (<< "Execute: " << *this);
   SharedPtr<SipMessage> sub = mClient.mDum.makeSubscription(resip::NameAddr(mAor), regEvent.value());
   mClient.mDum.send(sub);
}

resip::Message* 
AddAor::clone() const
{
   return new AddAor(mClient, mAor);
}

#ifdef RESIP_USE_STL_STREAMS
std::ostream& 
AddAor::encode(std::ostream& strm) const
#else
resip::ResipFastOStream&
AddAor::encode(resip::ResipFastOStream& strm) const
#endif
{
   strm << "Add RegEvent watcher " << mAor;
   return strm;
}

#ifdef RESIP_USE_STL_STREAMS
std::ostream& 
AddAor::encodeBrief(std::ostream& strm) const
#else
resip::ResipFastOStream&
AddAor::encodeBrief(resip::ResipFastOStream& strm) const
#endif
{
   return encode(strm);
}


RegEventClient::RegEventClient(SharedPtr<MasterProfile> profile) :
   mSecurity(0),
   mStack(mSecurity),
   mStackThread(mStack),
   mDum(mStack),
   mDumThread(mDum),
   mProfile(profile)
{
   mDum.addTransport(UDP, 5080);
   mDum.addTransport(TCP, 5081);

   mDum.setMasterProfile(mProfile);
   
   std::auto_ptr<resip::ClientAuthManager> clam(new resip::ClientAuthManager());
   mDum.setClientAuthManager(clam);
   mDum.setClientRegistrationHandler(this);
   
   mDum.addClientSubscriptionHandler(regEvent.value(), this);
}

RegEventClient::~RegEventClient()
{
#ifdef USE_SSL
   delete mSecurity;
   mSecurity = 0;
#endif
}

void
RegEventClient::run()
{
   mStackThread.run();
   mDumThread.run();
}

void
RegEventClient::watchAor(const resip::Uri& aor)
{
   AddAor* add = new AddAor(*this, aor);
   mDum.post(add);
}
 
Data toData(const SipMessage& notify)
{
  const HeaderFieldValue& body = notify.getRawBody();
  Data data(body.getBuffer(), body.getLength());
  return data;
}

// Client must call acceptUpdate or rejectUpdate for any onUpdateFoo
void 
RegEventClient::onNewSubscription(ClientSubscriptionHandle h, const SipMessage& notify)
{
   InfoLog (<< "Got subscription " << notify.brief());
}

void 
RegEventClient::onUpdatePending(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
   h->acceptUpdate();
   Data content = toData(notify);
   onRegEvent(h->getDocumentKey(), content);
}

void 
RegEventClient::onUpdateActive(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
   h->acceptUpdate();
   Data content = toData(notify);
   onRegEvent(h->getDocumentKey(), content);
}

void 
RegEventClient::onUpdateExtension(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
   h->acceptUpdate();
   Data content = toData(notify);
   onRegEvent(h->getDocumentKey(), content);
}

int 
RegEventClient::onRequestRetry(ClientSubscriptionHandle, int retrySeconds, const SipMessage& notify)
{
   return -1;
}
      
void 
RegEventClient::onTerminated(ClientSubscriptionHandle, const SipMessage* msg)
{
   WarningLog (<< "Subscription terminated " << (msg ? "with message " : " with no message"));
   if(msg)
   {
      WarningLog(<< msg->brief());
   }
}

void RegEventClient::onRegEvent(const resip::Data& aor, const resip::Data& reg)
{
  if (_cb)
  {
    _cb(aor, reg);
  }
   WarningLog (<< "Got result from " << aor << " --> " << reg);
}

void RegEventClient::onRegEventError(const resip::Data& aor, const resip::Data& reg)
{
   WarningLog (<< "Got error for " << aor);
}




