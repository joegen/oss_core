
#ifndef FramedTcpCONNECTION_H
#define	FramedTcpCONNECTION_H

#include "OSS/build.h"

#if ENABLE_FEATURE_NET_EXTRA

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace OSS {

class FramedTcpListener;

#define FTCP_VERSION 1
#define FTCP_KEY 22172
#define FTCP_READ_BUFFER_SIZE 8192

class FramedTcpConnection : public boost::enable_shared_from_this<FramedTcpConnection>, boost::noncopyable
{
public:
  typedef boost::asio::ip::tcp::socket::endpoint_type EndPoint;
  typedef boost::shared_ptr<FramedTcpConnection> Ptr;
  struct Packet
  {
    short version; // Expecting version 1
    short size; // size of the data buffer
    char* data;
  };

  explicit FramedTcpConnection(FramedTcpListener& listener);

  ~FramedTcpConnection();

  void start();
  void stop();

  void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred);

  bool write(const std::string& data);
  boost::asio::ip::tcp::socket& socket();

  const std::string& getLocalAddress() const;
  unsigned short getLocalPort() const;
  const std::string& getRemoteAddress() const;
  unsigned short getRemotePort() const;
  const std::string& getApplicationId() const;
  void setApplicationId(const std::string& id);
protected:
  void readMore(std::size_t bytes_transferred);
  void startInactivityTimer();
  void onInactivityTimeout(const boost::system::error_code&);
  boost::asio::io_service& _ioService;
  FramedTcpListener& _listener;
  boost::asio::ip::tcp::socket _socket;
  boost::asio::ip::tcp::resolver _resolver;
  boost::array<char, 8192> _buffer;

  std::string _messageBuffer;
  std::string _spillOverBuffer;
  std::size_t _moreReadRequired;
  std::size_t _lastExpectedPacketSize;
  std::string _localAddress;
  std::string _remoteAddress;
  unsigned short _localPort;
  unsigned short _remotePort;
  boost::asio::deadline_timer* _pInactivityTimer;
  std::string _applicationId;
};


//
// Inline
//

inline boost::asio::ip::tcp::socket& FramedTcpConnection::socket()
{
  return _socket;
}

inline const std::string& FramedTcpConnection::getLocalAddress() const
{
  return _localAddress;
}

inline unsigned short FramedTcpConnection::getLocalPort() const
{
  return _localPort;
}

inline const std::string& FramedTcpConnection::getRemoteAddress() const
{
  return _remoteAddress;
}

inline unsigned short FramedTcpConnection::getRemotePort() const
{
  return _remotePort;
}

inline const std::string& FramedTcpConnection::getApplicationId() const
{
  return _applicationId;
}

inline void FramedTcpConnection::setApplicationId(const std::string& id)
{
  _applicationId = id;
}

} // OSS


#endif // ENABLE_FEATURE_NET_EXTRA
#endif	/* FramedTcpCONNECTION_H */

