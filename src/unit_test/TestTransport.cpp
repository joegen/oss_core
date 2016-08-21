#include "gtest/gtest.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPTransportService.h" 
#include "OSS/Net/Net.h"

using namespace OSS::SIP;

static bool  tlsDispatched = false;

#define KEYS_DIR "../../../src/unit_test/test-data/tls/keys"
static const char * TLS_CA_FILE = KEYS_DIR "/ca.crt";
static const char * SERVER_CERT_FILE = KEYS_DIR "/karoo.crt";
static const char * SERVER_KEY_FILE = KEYS_DIR "/karoo.key";


static void tlsDispatch(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  ASSERT_TRUE(pMsg->isRequest("OPTIONS"));
  
  std::cout << pMsg->data() << std::endl;
  tlsDispatched = true;
}

SIPTransportService tlsServer(boost::bind(tlsDispatch, _1, _2));

// return pass phrase for tls cert
std::string password_callback(
    std::size_t max_length,  // the maximum length for a password
    boost::asio::ssl::context::password_purpose purpose ) // for_reading or for_writing
{
  return "admin";
}

TEST(TransportTest, test_tls_transport)
{
  if (boost::filesystem::exists(SERVER_KEY_FILE))
  {
    boost::asio::ssl::context& tlsServerContext = tlsServer.tlsServerContext();
    boost::asio::ssl::context& tlsClientContext = tlsServer.tlsClientContext();


    tlsClientContext.set_verify_mode( boost::asio::ssl::context::verify_peer);
    tlsServerContext.set_verify_mode(boost::asio::ssl::context::verify_peer | boost::asio::ssl::context::verify_fail_if_no_peer_cert);


    ASSERT_TRUE(boost::filesystem::exists(SERVER_KEY_FILE));
    ASSERT_TRUE(boost::filesystem::exists(SERVER_CERT_FILE));
    ASSERT_TRUE(boost::filesystem::exists(TLS_CA_FILE));


    tlsServerContext.set_password_callback(password_callback);
    tlsServerContext.use_certificate_file(SERVER_CERT_FILE, boost::asio::ssl::context::pem);
    tlsServerContext.use_private_key_file(SERVER_KEY_FILE, boost::asio::ssl::context::pem);
    tlsServerContext.load_verify_file(TLS_CA_FILE);

    tlsClientContext.set_password_callback(password_callback);
    tlsClientContext.use_certificate_file(SERVER_CERT_FILE, boost::asio::ssl::context::pem);
    tlsClientContext.use_private_key_file(SERVER_KEY_FILE, boost::asio::ssl::context::pem);
    tlsClientContext.load_verify_file(TLS_CA_FILE);


    OSS::Net::IPAddress localAddress("127.0.0.1");
    OSS::Net::IPAddress remoteAddress("127.0.0.1");
    remoteAddress.setPort(35061);


    SIPListener::SubNets subnets;
    subnets.push_back("0.0.0.0/0");
    tlsServer.addTLSTransport("127.0.0.1", "35061", "127.0.0.1", subnets);

    tlsServer.run();

    OSS::thread_sleep(100);

    SIPTransportSession::Ptr pClient = tlsServer.createClientTlsTransport(localAddress, remoteAddress);

    ASSERT_TRUE(!!pClient);

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

    pClient->writeKeepAlive();
    pClient->writeMessage(pMsg);


    OSS::thread_sleep(100);
    tlsServer.stop();

    ASSERT_TRUE(tlsDispatched);
  }
}

TEST(TransportTest, test_get_default_address)
{
  std::string iface;
  ASSERT_TRUE(OSS::net_get_default_interface_name(iface));
  ASSERT_FALSE(iface.empty());
  std::cout << "TransportTest::test_get_default_address result: interface-name=" << iface << std::endl;
  std::string address;
  ASSERT_TRUE(OSS::net_get_interface_address(iface, address));
  ASSERT_FALSE(address.empty());
  std::cout << "TransportTest::test_get_default_address result: address=" << address << std::endl;
}
