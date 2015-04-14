#include "gtest/gtest.h"

#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPTransportService.h" 

#include "OSS/Net/DTLSContext.h"
#include "OSS/Net/DTLSSession.h"

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
  
  pClient->writeKeepAlive();
  pClient->writeMessage(pMsg);

  
  OSS::thread_sleep(100);
  tlsServer.stop();
  
  ASSERT_TRUE(tlsDispatched);
}

void handle_dtls_server(OSS::Net::DTLSSession* pServer)
{
  //int fd = pServer->getFd();
 
  //
  // Poll for new connections
  //
  
  OSS::Net::IPAddress remotePeer;
  ASSERT_TRUE(pServer->accept(remotePeer));
  
  std::string remoteAddress = remotePeer.toIpPortString();
  ASSERT_STREQ(remoteAddress.c_str(), "127.0.0.1:30000");
  
  char buf[1024];
  int len = 0;
  while (true)
  {
    OSS::Net::DTLSSession::PacketType packetType = pServer->peek();
    if (packetType == OSS::Net::DTLSSession::DTLS)
    {
      len = pServer->read(buf, sizeof(buf));
      std::string msg(buf, len);
      if (msg == "exit")
      {
        break;
      }
      pServer->write(buf, len);
    }
    else if (packetType == OSS::Net::DTLSSession::RTP)
    {
      len = pServer->readRaw(buf, sizeof(buf));
      std::string rtpResponse("TypeRTP");
      ASSERT_EQ(pServer->write(rtpResponse.c_str(), rtpResponse.size()), rtpResponse.size());
    }
  }
}

TEST(TransportTest, test_dtls_transport)
{
  ASSERT_TRUE(OSS::Net::DTLSContext::initialize("ossapp.com", true));
  ASSERT_TRUE(OSS::Net::DTLSContext::instance());
  ASSERT_TRUE(OSS::Net::DTLSContext::willVerifyCerts());
  
  //
  // Create the client and server sockets
  //
  int client, server;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in s4;
		struct sockaddr_in6 s6;
	} server_addr, client_addr;
  
  memset((void *) &server_addr, 0, sizeof(struct sockaddr_storage));
	memset((void *) &client_addr, 0, sizeof(struct sockaddr_storage));
  
  inet_pton(AF_INET, "127.0.0.1", &client_addr.s4.sin_addr);
  client_addr.s4.sin_family = AF_INET;
	client_addr.s4.sin_port = htons(30000);
  
  inet_pton(AF_INET, "127.0.0.1", &server_addr.s4.sin_addr);
  server_addr.s4.sin_family = AF_INET;
	server_addr.s4.sin_port = htons(30002);
  
  client = socket(server_addr.ss.ss_family, SOCK_DGRAM, 0);
  server = socket(server_addr.ss.ss_family, SOCK_DGRAM, 0);
  
  bind(client, (const struct sockaddr *) &client_addr, sizeof(struct sockaddr_in));
  bind(server, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
  
  OSS::Net::DTLSSession* clientSession = new OSS::Net::DTLSSession(OSS::Net::DTLSSession::CLIENT);
  OSS::Net::DTLSSession* serverSession = new OSS::Net::DTLSSession(OSS::Net::DTLSSession::SERVER);
  
  clientSession->attachSocket(client);
  serverSession->attachSocket(server);
  
  boost::thread t(boost::bind(handle_dtls_server, serverSession));
  
  OSS::thread_sleep(500);
 

  OSS::Net::IPAddress connectAddress("127.0.0.1");
  connectAddress.setPort(30002);
  
  ASSERT_TRUE(clientSession->connect(connectAddress, false));


  std::string hello("hello");
  ASSERT_TRUE(clientSession->write(hello.c_str(), hello.size()) == hello.size());
  
  char buf[1024];
  int len = clientSession->read(buf, sizeof(buf));
  ASSERT_TRUE(len == hello.size());
  
  std::string response(buf, len);
  ASSERT_STREQ(response.c_str(), hello.c_str());
  
  //
  // Test sending RTP
  //
  unsigned char rtp_pkt[] =
  {
    0x80, 0x12, 0x00, 0xb5, 0x00, 0x2c, 0xcb, 0x6c,
    0x00, 0x00, 0x3a, 0x87, 0x22, 0xb3, 0x40, 0x77,
    0x02, 0x6d, 0x21, 0x37, 0xc3, 0x82, 0x26, 0xda,
    0x7f, 0xe4, 0xe8, 0x58, 0xd6, 0xa2, 0x3c, 0x5a
  };
  
  ASSERT_TRUE(clientSession->writeRaw((const char*)rtp_pkt, sizeof(rtp_pkt)) == sizeof(rtp_pkt));
  len = clientSession->read(buf, sizeof(buf));
  std::string rtpResponse(buf, len);
  ASSERT_STREQ(rtpResponse.c_str(), "TypeRTP");
  
  std::string exit("exit");
  ASSERT_TRUE(clientSession->write(exit.c_str(), exit.size()) == exit.size());
  
  
  
  t.join();
  
  delete clientSession;
  delete serverSession;
  
  ::close(client);
  ::close(server);
  
  OSS::Net::DTLSContext::releaseInstance();
}

