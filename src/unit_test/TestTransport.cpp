#include "gtest/gtest.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPTransportService.h" 

using namespace OSS::SIP;

static bool  tlsDispatched = false;

static void tlsDispatch(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  ASSERT_TRUE(pMsg->isRequest("OPTIONS"));
  tlsDispatched = true;
}

SIPTransportService tlsServer(boost::bind(tlsDispatch, _1, _2));

// return pass phrase for tls cert
std::string password_callback(
    std::size_t max_length,  // the maximum length for a password
    boost::asio::ssl::context::password_purpose purpose ) // for_reading or for_writing
{
  return "pass";
}

TEST(TransportTest, test_tls_transport)
{
  boost::asio::ssl::context& tlsServerContext = tlsServer.tlsServerContext();
  boost::asio::ssl::context& tlsClientContext = tlsServer.tlsClientContext();

  tlsClientContext.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::verify_none);
  tlsServerContext.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::verify_none);
  
  const char* KEY_FILE = "/home/joegen/certs/key.pem";
  const char* CERT_FILE = "/home/joegen/certs/cert.pem";
  
  ASSERT_TRUE(boost::filesystem::exists(KEY_FILE));
  ASSERT_TRUE(boost::filesystem::exists(CERT_FILE));
  
  //
  // Run the following commands in the same directory as the unit-test prior to running test-case
  // openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=sip.mydomain.local" -keyout sip.mydomain.local.key  -out sip.mydomain.local.cert
  // this will generate sip.mydomain.local.cert  sip.mydomain.local.key
  //
  
  tlsServerContext.set_password_callback(password_callback);
  tlsServerContext.use_private_key_file(KEY_FILE, boost::asio::ssl::context::pem);
  tlsServerContext.use_certificate_file(CERT_FILE, boost::asio::ssl::context::pem);
  tlsServerContext.use_certificate_chain_file(CERT_FILE);
  
  
  OSS::Net::IPAddress localAddress("127.0.0.1");
  OSS::Net::IPAddress remoteAddress("127.0.0.1");
  remoteAddress.setPort(35061);
  

  tlsServer.addTLSTransport("127.0.0.1", "35061", "127.0.0.1");
  
  tlsServer.run();

  OSS::thread_sleep(100);
  
  SIPTransportSession::Ptr pClient = tlsServer.createClientTlsTransport(localAddress, remoteAddress);
  
  ASSERT_TRUE(pClient);
  
  OSS::thread_sleep(100);
  
  std::ostringstream msg;
  msg << "OPTIONS sips:900@127.0.0.1:35061 SIP/2.0" << CRLF;
  msg << "To: <sips:900@127.0.0.1>" << CRLF;
  msg << "From: 9011 <sips:9011@127.0.0.1>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/TLS 127.0.0.1:5061;branch=z9hG4bK-d87543-419889160-1--d87543-;rport" << CRLF;
  msg << "Call-ID: 885e5e180c04c509" << CRLF;
  msg << "CSeq: 1 OPTIONS" << CRLF;
  msg << "Contact: <sips:9011@127.0.0.1:5061>" << CRLF << CRLF;
  
  SIPMessage::Ptr pMsg(new SIPMessage(msg.str()));
  
  ASSERT_TRUE(pClient->writeKeepAlive());
  pClient->writeMessage(pMsg);
  
  OSS::thread_sleep(100);
  
  ASSERT_TRUE(tlsDispatched);
  
  tlsServer.stop();
}

