#include "OSS/Net/FramedTcpConnection.h"
#include "OSS/Net/FramedTcpListener.h"
#include "OSS/UTL/Logger.h"


namespace OSS {

FramedTcpConnection::FramedTcpConnection( FramedTcpListener& listener) :
  _ioService(listener._ioService),
  _listener(listener),
  _socket(_ioService),
  _resolver(_ioService),
  _moreReadRequired(0),
  _lastExpectedPacketSize(0),
  _localPort(0),
  _remotePort(0),
  _pInactivityTimer(0)
{
  int expires = _listener.getInactivityThreshold() * 1000;
  _pInactivityTimer = new boost::asio::deadline_timer(_ioService, boost::posix_time::milliseconds(expires));
  OSS_LOG_DEBUG( "FramedTcpConnection CREATED.");
}

FramedTcpConnection::~FramedTcpConnection()
{
  delete _pInactivityTimer;
  stop();
  OSS_LOG_DEBUG( "FramedTcpConnection DESTROYED.");
}

bool FramedTcpConnection::write(const std::string& data)
{
  short version = FTCP_VERSION;
  short key = FTCP_KEY;
  short len = (short)data.size();
  std::stringstream strm;
  strm.write((char*)(&version), sizeof(version));
  strm.write((char*)(&key), sizeof(key));
  strm.write((char*)(&len), sizeof(len));
  strm << data;
  std::string packet = strm.str();
  boost::system::error_code ec;
  bool ok = _socket.write_some(boost::asio::buffer(packet.c_str(), packet.size()), ec) > 0;
  return ok;
}

void FramedTcpConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (!e && bytes_transferred)
  {
    try
    {
      if (!_localPort)
      {
        _localPort = _socket.local_endpoint().port();
        _localAddress = _socket.local_endpoint().address().to_string();
      }

      if (!_remotePort)
      {
        _remotePort = _socket.remote_endpoint().port();
        _remoteAddress = _socket.remote_endpoint().address().to_string();
      }
    }
    catch(...)
    {
      //
      //  Exception is non relevant if it is even thrown
      //
    }
    
    //
    // Start the inactivity timer
    //
    startInactivityTimer();

    OSS_LOG_DEBUG( "FramedTcpConnection::handleRead"
            << " BYTES: " << bytes_transferred
            << " SRC: " << _localAddress << ":" << _localPort
            << " DST: " << _remoteAddress << ":" << _remotePort );

    if (_moreReadRequired == 0)
    {
      std::stringstream strm;
      if (!_spillOverBuffer.empty())
        strm << _spillOverBuffer;

 
      strm.write(_buffer.data(), bytes_transferred);

      if (!_spillOverBuffer.empty())
      {
        bytes_transferred += _spillOverBuffer.size();
        OSS_LOG_DEBUG("FramedTcpConnection::handleRead "
                << "Spillover buffer is not empty.  "
                << "Resulting BYTES: " << bytes_transferred);
      }

      short version;
      strm.read((char*)(&version), sizeof(version));
      short key;
      strm.read((char*)(&key), sizeof(key));
      if (version == FTCP_VERSION && key == FTCP_KEY)
      {
        short len;
        strm.read((char*)(&len), sizeof(len));
        
        _lastExpectedPacketSize = len + sizeof(version) + sizeof(key) + (sizeof(len));
        _moreReadRequired =  bytes_transferred - _lastExpectedPacketSize;
        
        if (!_moreReadRequired)
        {
          char buf[FTCP_READ_BUFFER_SIZE];
          strm.read(buf, len);
          _listener.onIncomingRequest(*this, buf, len);
          _spillOverBuffer = std::string();
          if (_lastExpectedPacketSize < bytes_transferred)
          {
            //
            // We have spill over bytes
            //
            std::size_t extraBytes = bytes_transferred - _lastExpectedPacketSize;
            char spillbuf[extraBytes];
            strm.read(spillbuf, extraBytes);
            _spillOverBuffer = std::string(spillbuf, extraBytes);
            OSS_LOG_DEBUG("FramedTcpConnection::handleRead "
                << "Spillover bytes from last read detected.  "
                << "BYTES: " << extraBytes);
          }
        }
        else
        {
          _messageBuffer += _spillOverBuffer;
          _messageBuffer += std::string(_buffer.data(), bytes_transferred);
          _spillOverBuffer = std::string();
          OSS_LOG_DEBUG("FramedTcpConnection::handleRead "
                << "More bytes required to complete message.  "
                << "Required BYTES: " << _moreReadRequired);
        }
      }
    }
    else
    {
      readMore(bytes_transferred);
    }
  }
  else if (e)
  {
    OSS_LOG_WARNING( "FramedTcpConnection::handleRead "
                << "Exception caught while calling read.  "
                << "ERROR: " << e.message());
    boost::system::error_code ignored_ec;
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _listener.destroyConnection(shared_from_this());
    return;
  }

  start();
}

void FramedTcpConnection::readMore(std::size_t bytes_transferred)
{
  _messageBuffer += std::string(_buffer.data(), bytes_transferred);

  std::stringstream strm;
  strm << _messageBuffer;

  short version;
  strm.read((char*)(&version), sizeof(version));
  short key;
  strm.read((char*)(&key), sizeof(key));
  if (version == FTCP_VERSION && key == FTCP_KEY)
  {
    short len;
    strm.read((char*)(&len), sizeof(len));

    _lastExpectedPacketSize = len + sizeof(version) + sizeof(key) + (sizeof(len));
    if (_messageBuffer.size() < _lastExpectedPacketSize)
      _moreReadRequired = _lastExpectedPacketSize - _messageBuffer.size();
    else
      _moreReadRequired = 0;

    if (!_moreReadRequired)
    {
      char buf[FTCP_READ_BUFFER_SIZE];
      strm.read(buf, len);
      _listener.onIncomingRequest(*this, buf, len);
      _spillOverBuffer = std::string();
      if (_lastExpectedPacketSize < _messageBuffer.size())
      {
        //
        // We have spill over bytes
        //
        std::size_t extraBytes = _messageBuffer.size() - _lastExpectedPacketSize;
        char spillbuf[extraBytes];
        strm.read(spillbuf, extraBytes);
        _spillOverBuffer = std::string(spillbuf, extraBytes);
        OSS_LOG_DEBUG( "FramedTcpConnection::readMore "
                << "Spillover bytes from last read detected.  "
                << "BYTES: " << extraBytes);
      }
    }
    else
    {
      OSS_LOG_DEBUG( "FramedTcpConnection::readMore "
                << "More bytes required to complete message.  "
                << "Required BYTES: " << _moreReadRequired);
    }
  }
}

void FramedTcpConnection::start()
{
  OSS_LOG_DEBUG( "FramedTcpConnection::start() INVOKED");


  _socket.async_read_some(boost::asio::buffer(_buffer),
            boost::bind(&FramedTcpConnection::handleRead, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
}

void FramedTcpConnection::stop()
{
  OSS_LOG_DEBUG( "FramedTcpConnection::stop() INVOKED");
  _socket.close();
}

void FramedTcpConnection::onInactivityTimeout(const boost::system::error_code& ec)
{
  if (!ec)
  {
    OSS_LOG_WARNING( "FramedTcpConnection::onInactivityTimeout "
                  << "No activity on this socket for too long." );
    boost::system::error_code ignored_ec;
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _socket.close();
  }
}

void FramedTcpConnection::startInactivityTimer()
{
  boost::system::error_code ignored_ec;
  _pInactivityTimer->cancel(ignored_ec);
  _pInactivityTimer->expires_from_now(boost::posix_time::milliseconds(_listener.getInactivityThreshold() * 1000));
  _pInactivityTimer->async_wait(boost::bind(&FramedTcpConnection::onInactivityTimeout, this, boost::asio::placeholders::error));

  OSS_LOG_DEBUG( "FramedTcpConnection::startInactivityTimer "
          << " Session inactivity timeout set at " << _listener.getInactivityThreshold() << " seconds.");
}

} // OSS


