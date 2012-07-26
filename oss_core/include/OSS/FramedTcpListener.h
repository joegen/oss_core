
#ifndef OSS_STATEQUEUELISTENER_H
#define	OSS_STATEQUEUELISTENER_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "OSS/FramedTcpConnection.h"

namespace OSS {

class FramedTcpListener : boost::noncopyable
{
public:
  typedef boost::recursive_mutex mutex;
  typedef boost::lock_guard<mutex> mutex_lock;
  FramedTcpListener();
  ~FramedTcpListener();
  void run(const std::string& address, const std::string& port);
  void handleAccept(const boost::system::error_code& e);
  
  void addConnection(FramedTcpConnection::Ptr conn);
  void destroyConnection(FramedTcpConnection::Ptr conn);

  void setInactivityThreshold(int threshold);
  int getInactivityThreshold() const;

  void onIncomingRequest(FramedTcpConnection& connection, const char* data, std::size_t len);

protected:
  boost::asio::io_service _ioService;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::resolver _resolver;
  FramedTcpConnection::Ptr _pNewConnection;
  mutex _mutex;
  std::map<FramedTcpConnection*, FramedTcpConnection::Ptr> _connections;
  int _inactivityThreashold;
  friend class FramedTcpConnection;
};

//
// Inlines
//
inline void FramedTcpListener::setInactivityThreshold(int threshold)
{
  _inactivityThreashold = threshold;
}

inline int FramedTcpListener::getInactivityThreshold() const
{
  return _inactivityThreashold;
}

} // OSS

#endif	/* STATEQUEUELISTENER_H */

