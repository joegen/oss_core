#include "gtest/gtest.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPTransportService.h" 

using namespace OSS::SIP;

static void tlsDispatch(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
}
SIPTransportService tlsServer(boost::bind(tlsDispatch, _1, _2));


TEST(TransportTest, test_udp_transport)
{
}

TEST(TransportTest, test_tcp_transport)
{
}

TEST(TransportTest, test_tls_transport)
{
  //
  // Run the following commands in the same directory as the unit-test prior to running test-case
  // openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=sip.mydomain.local" -keyout sip.mydomain.local.key  -out sip.mydomain.local.cert
  // this will generate sip.mydomain.local.cert  sip.mydomain.local.key
  //
  tlsServer.addTLSTransport("127.0.0.1", "35061", "127.0.0.1");
  
  boost::asio::ssl::context& tlsContext = tlsServer.tlsContext();
  
  tlsContext.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
  
  ASSERT_TRUE(boost::filesystem::exists("sip.mydomain.local.cert"));
  ASSERT_TRUE(boost::filesystem::exists("sip.mydomain.local.key"));
  
  tlsContext.use_certificate_chain_file("sip.mydomain.local.cert");
  tlsContext.use_private_key_file("sip.mydomain.local.key", boost::asio::ssl::context::pem);
  
  OSS::Net::IPAddress localAddress("127.0.0.1");
  OSS::Net::IPAddress remoteAddress("127.0.0.1");
  remoteAddress.setPort(35061);
  
  tlsServer.run();

  OSS::thread_sleep(100);
  
  SIPTransportSession::Ptr pClient = tlsServer.createClientTlsTransport(localAddress, remoteAddress);
  
  ASSERT_TRUE(pClient);
  
  OSS::thread_sleep(100);
  
  ASSERT_TRUE(pClient->writeKeepAlive());
  
  OSS::thread_sleep(100);
  
  tlsServer.stop();
}

