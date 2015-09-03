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


#include "OSS/SIP/UA/SIPRegistration.h"
#include "OSS/UTL/Logger.h"
#include "re.h"


namespace OSS {
namespace SIP {
namespace UA {


const OSS::UInt32 DEFAULT_REG_EXPIRES = 3600;

static void register_response_handler(int err, const struct sip_msg *msg, void *arg)
{
  SIPRegistration* pRegistration = (SIPRegistration*)arg;
  
  
  if (pRegistration && !pRegistration->getResponseHandlers().empty())
  {
    SIPMessage::Ptr pMsg;
    
    std::string errorMessage(strerror(err));
   
    if (!err && msg && msg->mb->size)
    {
      pMsg = SIPMessage::Ptr(new SIPMessage(msg->mb->buf, msg->mb->size));
      
      std::size_t count = pMsg->hdrGetSize(OSS::SIP::HDR_CONTACT); 
      if (count > 0)
      {
        std::vector<std::string> clist;
        SIPContact::msgGetContacts(pMsg.get(), clist);
        
        if (!clist.empty())
        {
          SIPContact contact(clist);
          pRegistration->setContactList(contact);
        }
      }
    }
    

    const SIPRegistration::ResponseHandlerList& handlers = pRegistration->getResponseHandlers();
    for (SIPRegistration::ResponseHandlerList::const_iterator iter = handlers.begin();
      iter != handlers.end(); iter++)
    {
      (*iter)(pRegistration, pMsg, errorMessage);
    }
  }
  else
  {
    OSS_LOG_ERROR("OSS::SIP::UA::register_response_handler - Unable to determine session or response handler not set.");
    delete pRegistration;
    pRegistration = 0;
  }
} 

/* called when challenged for credentials */
static int register_auth_handler(char **user, char **pass, const char *realm, void *arg)
{
	int err = 0;
	(void)realm;

  SIPRegistration* pRegistration = (SIPRegistration*)arg;
  if (!pRegistration)
    return 1;
  
  if (pRegistration->getAuthUser().empty() || pRegistration->getAuthPassword().empty())
    return -1;
  
	err |= str_dup(user, pRegistration->getAuthUser().c_str());
	err |= str_dup(pass, pRegistration->getAuthPassword().c_str());

	return err;
}
  
  
SIPRegistration::SIPRegistration(SIPUserAgent& ua) :
  _ua(ua),
  _registration_handle(0),
  _expires(DEFAULT_REG_EXPIRES),
  _isRegistered(false),
  _regId(0)
{
}

SIPRegistration::~SIPRegistration()
{
  if (_scheduleTimer)
  {
    OSS::net_io_timer_cancel(_scheduleTimer);
  }
  
  stop();
  
  OSS_LOG_INFO("SIPRegistration " << _toUser << "@" << _domain << " - " << _callId << " DESTROYED");
}

void SIPRegistration::stop()
{
  struct sipreg* pReg = (struct sipreg*)_registration_handle; 
  if (pReg)
  {
    mem_deref(pReg);
    _registration_handle = 0;
  }
}

SIPRegistration* SIPRegistration::clone() const
{
  SIPRegistration* pClone = new SIPRegistration(_ua);

  pClone->_authUser  = _authUser;
  pClone->_authPassword  = _authPassword;
  pClone->_domain  = _domain;
  pClone->_realm  = _realm;
  pClone->_fromUser  = _fromUser;
  pClone->_toUser  = _toUser;
  pClone->_contactUser  = _contactUser;
  pClone->_routeHeader  = _routeHeader;
  pClone->_contactParams  = _contactParams;
  pClone->_expires  = _expires;
  pClone->_extraHeaders  = _extraHeaders;
  pClone->_responseHandlers  = _responseHandlers;
  
  return pClone;
}

void SIPRegistration::schedule(int millis)
{
  //
  // Cancel any previous timer
  //
  if (_scheduleTimer)
  {
    OSS::net_io_timer_cancel(_scheduleTimer);
  }
  
  //
  // Stop any previous run
  //
  stop();
  
  //
  // Schedule a new timer
  //
  OSS_LOG_NOTICE("SIPRegistration::schedule - Next registration attempt after " << millis << " milliseconds.");
  _scheduleTimer = OSS::net_io_timer_create(millis, boost::bind(&SIPRegistration::schedule_handler, this));
}


void SIPRegistration::schedule_handler()
{
  if (!run())
  {
    delete this;
  }
}

bool SIPRegistration::run()
{
  //
  // Stop any previous call to run
  //
  stop();
  
  int err = 0;
  std::ostringstream fromUri;
  std::ostringstream toUri;
  std::ostringstream registrar;
  
  if (_contactUser.empty())
  {
    OSS_LOG_ERROR("SIPRegistration::send - Contact User can't be empty.");
    return false;
  }
  
  if (_domain.empty())
  {
    OSS_LOG_ERROR("SIPRegistration::send - Domain can't be empty.");
    return false;
  }
  
  if (_fromUser.empty())
    _fromUser = _contactUser;
  
  if (_toUser.empty())
    _toUser = _fromUser;
  
  fromUri << "sip:" << _fromUser << "@" << _domain;
  toUri << "sip:" << _toUser << "@" << _domain;
  registrar << "sip:" << _domain;
  
  sipreg* pRegistration = 0;
  struct sip* pSipStack  = (sip*)_ua.sip_stack_handle();
  
  const char* routes[] = {_routeHeader.c_str()};
  
  err = sipreg_register(
    &pRegistration, // @param regp     Pointer to allocated SIP Registration client
    pSipStack, // @param sip      SIP Stack instance
    registrar.str().c_str(), // @param reg_uri  SIP Request URI
    toUri.str().c_str(), // @param to_uri   SIP To-header URI
    fromUri.str().c_str(), // @param from_uri SIP From-header URI
    _expires, // @param expires  Registration interval in [seconds]
    _contactUser.c_str(), // @param cuser    Contact username
		_routeHeader.empty() ? 0 : routes, // @param routev   Optional route vector
    _routeHeader.empty() ? 0 : 1, // @param routec   Number of routes
    _regId, // @param regid    Register identification
    register_auth_handler, // @param authh    Authentication handler
    (OSS_HANDLE)this, // @param aarg     Authentication handler argument
    false, // @param aref     True to ref argument
		register_response_handler, // @param resph    Response handler
    (OSS_HANDLE)this, // @param arg      Response handler argument
    _contactParams.empty() ? 0 : _contactParams.c_str(), // @param params   Optional Contact-header parameters
    _extraHeaders.empty() ? 0 : _extraHeaders.c_str() // @param fmt      Formatted strings with extra SIP Headers
  );
  
  if (err)
  {
    OSS_LOG_ERROR("SIPRegistration::send - Failure sending registration (" << strerror(err) << ")");
    return false;
  }
  
  _registration_handle = (OSS_HANDLE)pRegistration;
  
  return true;
}

bool SIPRegistration::isRegisteredBinding(const OSS::SIP::SIPURI& binding) const
{
  OSS::mutex_critic_sec_lock lock(_contactListMutex);
  std::size_t count = _contactList.getSize();
  for (std::size_t i = 0; i < count; i++)
  {
    ContactURI curi;
    _contactList.getAt(curi, i);
    
    if (curi.getHostPort() == binding.getHostPort())
      return true;
  }
  return false;
}
  
  
} } } // OSS::SIP::UA




