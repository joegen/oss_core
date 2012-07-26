
#ifndef FramedTcpClient_H
#define	FramedTcpClient_H

#include <cassert>
#include <zmq.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "OSS/Logger.h"
#include <boost/lexical_cast.hpp>


namespace OSS {

class FramedTcpClient
{
public:
  typedef boost::shared_ptr<FramedTcpClient> Ptr;
  FramedTcpClient(boost::asio::io_service& ioService);
  ~FramedTcpClient();
  bool connect(const std::string& serviceAddress, const std::string& servicePort);
  bool sendAndReceive(const std::string& data, std::string& response);
  bool isConnected() const;
private:
  short getNextReadSize();
  boost::asio::io_service& _ioService;
  boost::asio::ip::tcp::resolver _resolver;
  boost::asio::ip::tcp::socket *_pSocket;
  std::string _serviceAddress;
  std::string _servicePort;
  bool _isConnected;
};

} // OSS


#endif	/* FramedTcpClient_H */

