

#include "OSS/SIP/EP/SIPEndpoint.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/LogFile.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPHeaderTokens.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPVia.h"


using OSS::SIP::EP::SIPEndpoint;
using OSS::Net::IPAddress;
using OSS::SIP::SIPMessage;
using OSS::SIP::SIPURI;
using OSS::Net::IPAddress;
using OSS::SIP::ContactList;
using OSS::SIP::SIPContact;
using OSS::SIP::ContactURI;
using OSS::SIP::SIPRecordRoute;
using OSS::SIP::SIPRoute;
using OSS::SIP::SIPCSeq;
using OSS::SIP::SIPFrom;
using OSS::SIP::SIPTo;
using OSS::SIP::SIPVia;


const char* uaAddress = "127.0.0.1";
const unsigned int UAC_PORT = 35060;
const unsigned int UAS_PORT = 35062;
const IPAddress uasHostPort(uaAddress, UAS_PORT);
const IPAddress uacHostPort(uaAddress, UAC_PORT);


void uac_create_contact(SIPURI& contactUri)
{
  contactUri.setUserInfo("uac");
  contactUri.setHostPort(uacHostPort.toIpPortString().c_str());
}

void uac_send_ack_for_invite(SIPEndpoint* pEndpoint, const SIPEndpoint::EndpointEventPtr& pEvent)
{
  //
  // clone the 200 ok request.  It would already have the correct tags
  //
  SIPMessage* pRequest = new SIPMessage(*(pEvent->sipRequest.get()));
  
  OSS::SIP::ContactList contactList;
  OSS::SIP::SIPContact::msgGetContacts(pRequest,  contactList);
  
  if (contactList.empty())
  {
    return;
  }
  
  //
  // Create the start-line from the contact
  //
  ContactURI& curi = contactList.front();
  std::ostringstream startLine;
  startLine << "ACK " << curi.getURI() << " SIP/2.0";
  pRequest->startLine() = startLine.str();
  
  //
  // Replace the contact with our own local address
  //
  SIPURI localContact;
  uac_create_contact(localContact);
  pRequest->hdrListRemove(OSS::SIP::HDR_CONTACT);
  pRequest->hdrSet(OSS::SIP::HDR_CONTACT, localContact.data());
  
  //
  // Now check if there is a route set
  //
  std::list<std::string> recordRoutes;
  SIPRecordRoute::msgGetRecordRoutes(pRequest, recordRoutes);
  if (!recordRoutes.empty())
  {
    pRequest->hdrListRemove(OSS::SIP::HDR_RECORD_ROUTE);
    
    //
    // Prepend the record routes as route headers (prepending inserts it in reverse order)
    //
    for (std::list<std::string>::iterator iter = recordRoutes.begin(); iter != recordRoutes.end(); iter++)
    {
      pRequest->hdrListPrepend(OSS::SIP::HDR_ROUTE, *iter);
    }
  }
  
  //
  // Change the CSEQ method from INVITE to ACK
  //
  SIPCSeq cseq(pRequest->hdrGet(OSS::SIP::HDR_CSEQ));
  cseq.setMethod(OSS::SIP::REQ_ACK);
  pRequest->hdrSet(OSS::SIP::HDR_CSEQ, cseq.data());
  
  //
  // change the via branch.  We preserve the host and protocol
  //
  std::string newBranch = SIPVia::createBranchString();
  std::string topVia;
  SIPVia::msgGetTopVia(pRequest, topVia);
  SIPVia oldVia(topVia);
  SIPVia newVia;
  newVia.setParam("branch", newBranch.c_str());
  newVia.setTransport(oldVia.getTransport().c_str());
  newVia.setSentBy(oldVia.getSentBy().c_str());
  pRequest->hdrListRemove(OSS::SIP::HDR_VIA);
  pRequest->hdrSet(OSS::SIP::HDR_VIA, newVia.data());
  
  
  //
  // Remove the body if there is any
  //
  pRequest->hdrRemove(OSS::SIP::HDR_CONTENT_LENGTH);
  pRequest->hdrRemove(OSS::SIP::HDR_CONTENT_TYPE);
  pRequest->body() = std::string();
  
  SIPMessage::Ptr pAck(pRequest);
  pEndpoint->sendEndpointRequest(pAck, uacHostPort, uasHostPort);
}

void uac_handle_response(SIPEndpoint* pEndpoint, const SIPEndpoint::EndpointEventPtr& pEvent)
{
  if (pEvent->sipRequest->isResponseTo(OSS::SIP::REQ_INVITE) && pEvent->sipRequest->isResponseFamily(OSS::SIP::SIPMessage::CODE_200_Ok))
  {
    uac_send_ack_for_invite(pEndpoint, pEvent);
    return;
  }
}

void uac_thread(SIPEndpoint* pEndpoint)
{
  unsigned int cseq = 0;
  
  SIPURI requestUri;
  requestUri.setUserInfo("uas");
  requestUri.setHostPort(uasHostPort.toIpPortString().c_str());
  
  //
  // Create tags and branches
  //
  std::string callId = SIPMessage::createTagString();
  std::string fromTag = SIPMessage::createTagString();
  std::string branch = SIPMessage::createBranchString();
  
  //
  // Create the from uri
  //
  SIPURI fromUri;
  fromUri.setUserInfo("uac");
  fromUri.setHostPort("127.0.0.1");
  
  //
  // Create the to uri
  //
  SIPURI toUri;
  toUri.setUserInfo("uas");
  toUri.setHostPort("127.0.0.1");
  
  //
  // Create the contact uri
  //
  SIPURI contactUri;
  uac_create_contact(contactUri);
  
  SIPMessage::Ptr pInvite1 = SIPMessage::createRequest(
      SIPMessage::REQUEST_INVITE,
      requestUri,
      callId,
      ++cseq,
      fromUri,
      "UAC",
      fromTag,
      toUri,
      "UAS",
      "",
      contactUri,
      "UAC",
      uacHostPort,
      branch,
      "",
      "");
  
  if (pInvite1)
  {
    pEndpoint->sendEndpointRequest(pInvite1, uacHostPort, uasHostPort);
  }
  
  bool terminated = false;
  while (!terminated)
  {
    SIPEndpoint::EndpointEventPtr pEvent;
    pEndpoint->receiveEndpointEvent(pEvent);
    if (pEvent)
    {
      switch(pEvent->eventType)
      {
        case SIPEndpoint::IncomingRequest:
          break;
        case SIPEndpoint::IncomingResponse:
          uac_handle_response(pEndpoint, pEvent);
          break;
        case SIPEndpoint::TransactionError:
          break;
        case SIPEndpoint::TransactionTermination:
          break;
        case SIPEndpoint::Ackfor2xx:
          break;
        case SIPEndpoint::EndpointTerminated:
          terminated = true;
          break;
      }
    }
  }
}

void uas_thread(SIPEndpoint* pEndpoint)
{
  //
  // Create the contact uri
  //
  SIPURI contactUri;
  contactUri.setUserInfo("uas");
  contactUri.setHostPort(uasHostPort.toIpPortString().c_str());
  
  std::string localTag = SIPMessage::createTagString();
  
  bool terminated = false;
  while (!terminated)
  {
    SIPEndpoint::EndpointEventPtr pEvent;
    pEndpoint->receiveEndpointEvent(pEvent);
    if (pEvent)
    {
      switch(pEvent->eventType)
      {
        case SIPEndpoint::IncomingRequest:
        {
          SIPMessage::Ptr pResponse = pEvent->sipRequest->createResponse(200, "Hell Yeah!", localTag, contactUri.data());
          pEndpoint->sendEndpointResponse(pResponse, pEvent->transaction, uacHostPort);
          break;
        }
        case SIPEndpoint::IncomingResponse:
          break;
        case SIPEndpoint::TransactionError:
          break;
        case SIPEndpoint::TransactionTermination:
          break;
        case SIPEndpoint::Ackfor2xx:
          break;
        case SIPEndpoint::EndpointTerminated:
          terminated = true;
          break;
      }
    }
  }
}

int main(int argc, char** argv) 
{
  OSS::OSS_init();
  
  OSS::log_reset_level(OSS::PRIO_DEBUG);
  
  SIPEndpoint uac;
  SIPEndpoint uas;
  
  IPAddress uacAddress("127.0.0.1");
  uacAddress.setPort(UAC_PORT);
  
  IPAddress uasAddress("127.0.0.1");
  uasAddress.setPort(UAS_PORT);
  
  uac.addTransport(uacAddress);
  uas.addTransport(uasAddress);
  
  uac.runEndpoint();
  uas.runEndpoint();
  
  OSS::thread_sleep(1000);
  
  boost::thread uacThread(boost::bind(uac_thread, &uac));
  boost::thread uasThread(boost::bind(uas_thread, &uas));
  
  uacThread.join();
  uasThread.join();
 
  return 0;
}

