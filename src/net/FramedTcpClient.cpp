
#include "OSS/Net/FramedTcpConnection.h"
#include "OSS/Net/FramedTcpClient.h"


namespace OSS {


FramedTcpClient::FramedTcpClient(boost::asio::io_service& ioService) :
  _ioService(ioService),
  _resolver(_ioService),
  _pSocket(0),
  _isConnected(false)
{
}

FramedTcpClient::~FramedTcpClient()
{
  delete _pSocket;
}

bool FramedTcpClient::connect(const std::string& serviceAddress, const std::string& servicePort)
{
  if (_pSocket && _isConnected)
  {
    return true;
  }
  else if (_pSocket && !_isConnected)
  {
    boost::system::error_code ignored_ec;
   _pSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  delete _pSocket;
  _pSocket = new boost::asio::ip::tcp::socket(_ioService);

  _serviceAddress = serviceAddress;
  _servicePort = servicePort;

  try
  {
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), serviceAddress.c_str(), servicePort.c_str());
    boost::asio::ip::tcp::resolver::iterator hosts = _resolver.resolve(query);

    //////////////////////////////////////////////////////////////////////////
    // Only works in 1.47 version of asio.  1.46 doesnt have this utility func
    // boost::asio::connect(*_pSocket, hosts);
       _pSocket->connect(hosts->endpoint()); // so we use the connect member
    //////////////////////////////////////////////////////////////////////////

    _isConnected = true;
  }
  catch(...)
  {
    _isConnected = false;
  }

  return _isConnected;
}

bool FramedTcpClient::sendAndReceive(const std::string& data, std::string& response)
{
  assert(_pSocket);
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

  if (packet.size() > FTCP_READ_BUFFER_SIZE)
  {
    OSS_LOG_DEBUG( "FramedTcpClient::sendAndReceive "
                << "Packet exceeds allowable frame size " << FTCP_READ_BUFFER_SIZE);
    return false;
  }


  bool ok = _pSocket->write_some(boost::asio::buffer(packet.c_str(), packet.size()), ec) > 0;
  if (!ok)
  {
    _isConnected = false;
    return false;
  }

  len = getNextReadSize();
  if (!len)
    return false;

  char responseBuff[len];
  _pSocket->read_some(boost::asio::buffer((char*)responseBuff, len), ec);
  if (ec)
  {
    _isConnected = false;
    return false;
  }

  response = std::string(responseBuff, len);
  return true;
}

short FramedTcpClient::getNextReadSize()
{
  short version = FTCP_VERSION;
  short key = FTCP_KEY;
  bool hasVersion = false;
  bool hasKey = false;
  short remoteLen = 0;
  while (!hasVersion || !hasKey)
  {
    short remoteVersion;
    short remoteKey;

    //
    // Read the version (must be 1)
    //
    while (true)
    {

      boost::system::error_code ec;
      _pSocket->read_some(boost::asio::buffer((char*)&remoteVersion, sizeof(remoteVersion)), ec);
      if (ec)
      {
        OSS_LOG_DEBUG( "FramedTcpClient::getNextReadSize "
                << "Unable to read version "
                << "ERROR: " << ec.message());
        _isConnected = false;
        return 0;
      }
      else
      {
        if (remoteVersion == version)
        {
          hasVersion = true;
          break;
        }
      }
    }

    //
    // Read the key (must be 22172)
    //
    while (true)
    {

      boost::system::error_code ec;
      _pSocket->read_some(boost::asio::buffer((char*)&remoteKey, sizeof(remoteKey)), ec);
      if (ec)
      {
        OSS_LOG_DEBUG( "FramedTcpClient::getNextReadSize "
                << "Unable to read secret key "
                << "ERROR: " << ec.message());
        _isConnected = false;
        return 0;
      }
      else
      {
        if (remoteKey == key)
        {
          hasKey = true;
          break;
        }
      }
    }
  }

  boost::system::error_code ec;
  _pSocket->read_some(boost::asio::buffer((char*)&remoteLen, sizeof(remoteLen)), ec);
  if (ec)
  {
    OSS_LOG_DEBUG( "FramedTcpClient::getNextReadSize "
            << "Unable to read secret packet length "
            << "ERROR: " << ec.message());
    _isConnected = false;
    return 0;
  }

  return remoteLen;
}

bool FramedTcpClient::isConnected() const
{
  return _isConnected;
}


} // OSS





