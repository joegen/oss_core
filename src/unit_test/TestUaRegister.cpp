#include "gtest/gtest.h"
#include "OSS/SIP/UA/SIPRegistration.h"
#include "OSS/SIP/UA/SIPEventLoop.h"
#include "OSS/SIP/UA/SIPUserAgent.h"
#include "OSS/SIP/B2BUA/SIPB2BRegisterAgent.h"


using namespace OSS;
using namespace OSS::SIP;
using namespace OSS::SIP::UA;
using namespace OSS::SIP::B2BUA;

static SIPEventLoop* gpEventLoop = 0;
static SIPB2BRegisterAgent* gpUA = 0;
static bool gResponseHandled = false;
static bool gExitHandled = false;
static bool gIsOkResponse = false;

void reg_response_handler(SIPRegistration* pReg, const SIPMessage::Ptr& pMsg, const std::string& error)
{
  //
  // Stop the user-agent
  //
  gpUA->stop();
  gResponseHandled = true;
  
  if (pMsg)
    gIsOkResponse = pMsg->is2xx(200);
}

void exit_handler()
{
  gExitHandled = true;
  gpEventLoop->stop(false, true, true);
}

TEST(UATest, test_ua_register)
{
  //
  // Run the main event loop
  //
  gpEventLoop = new SIPEventLoop();
  gpEventLoop->run(true);
   
  gpUA = new SIPB2BRegisterAgent();
  
  ASSERT_TRUE(gpUA->initialize("oss_core"));
  ASSERT_TRUE(gpUA->run(exit_handler));
  ASSERT_TRUE(gpUA->sendRegister("creytiv.com", "demo", "demo", "secret", "", "", "sip:bridge.ossapp.com", 3600, reg_response_handler));
  
  //
  // Wait until event loop terminates
  //
  OSS::thread_sleep(5000);
  
  delete gpUA;
  delete gpEventLoop;
   
  ASSERT_TRUE(gResponseHandled);
  ASSERT_TRUE(gExitHandled);
}
