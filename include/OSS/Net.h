// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef OSS_NET_H_INCLUDED
#define OSS_NET_H_INCLUDED

#include "OSS/OSS.h"
#include "OSS/IPAddress.h"

#include <list>
#include <vector>

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "OSS/OSS.h"


namespace OSS {

namespace Private {
  void net_init();

  void net_deinit();
} // Private
//
// Type definitions
//

typedef std::vector<OSS::IPAddress> socket_address_list; /// Represents a list of socket addresses

struct OSS_API network_interface
{
  std::string _ifName;
  std::string _ipAddress;
  std::string _netMask;
  std::string _broadcastAddress;
  std::string _destAddr;
  unsigned int _flags;
  typedef std::vector<network_interface> interface_address_list;
  static interface_address_list _table;
};


bool OSS_API socket_address_cidr_verify(const std::string& ip, const std::string& cidr);
  /// Determine if an IP address exists in a CIDR IP range.  
  /// Eg check ip="192.168.0.100" if it belongs in cidr="192.168.0.1/24"
  ///
  /// This function only works with IPV4 address


//
// Common Socket Functions
//

enum ModeSelect
{
	MODE_SELECT_READ  = 1,
	MODE_SELECT_WRITE = 2,
	MODE_SELECT_ERROR = 4
};

typedef void* socket_handle;

void OSS_API socket_free(socket_handle handle);
  /// Free the socket.  All calls to socket_create() must call this function to avoid resource leak.

bool OSS_API socket_poll(socket_handle handle, const OSS::UInt32 timeout, ModeSelect mode) ;
	/// Determines the status of the socket, using a 
	/// call to poll() or select().
	/// 
	/// The mode argument is constructed by combining the values
	/// of the SelectMode enumeration.
	///
	/// Returns true if the next operation corresponding to
	/// mode will not block, false otherwise.

int OSS_API socket_available(socket_handle handle) ;
	/// Returns the number of bytes available that can be read
	/// without causing the socket to block.

void OSS_API socket_set_send_buffer_size(socket_handle handle, int size);
	/// Sets the size of the send buffer.
	
int OSS_API socket_get_send_buffer_size(socket_handle handle) ;
	/// Returns the size of the send buffer.
	///
	/// The returned value may be different than the
	/// value previously set with setSendBufferSize(),
	/// as the system is free to adjust the value.

void OSS_API socket_set_receive_buffer_size(socket_handle handle, int size);
	/// Sets the size of the receive buffer.
	
int OSS_API socket_get_receive_buffer_size(socket_handle handle) ;
	/// Returns the size of the receive buffer.
	///
	/// The returned value may be different than the
	/// value previously set with setReceiveBufferSize(),
	/// as the system is free to adjust the value.

void OSS_API socket_set_send_timeout(socket_handle handle, OSS::UInt32 timeout);
	/// Sets the send timeout for the socket.

OSS::UInt32 OSS_API socket_get_send_timeout(socket_handle handle) ;
	/// Returns the send timeout for the socket.
	///
	/// The returned timeout may be different than the
	/// timeout previously set with setSendTimeout(),
	/// as the system is free to adjust the value.

void OSS_API socket_set_receive_timeout(socket_handle handle, OSS::UInt32 timeout);
	/// Sets the send timeout for the socket.
	///
	/// On systems that do not support SO_RCVTIMEO, a
	/// workaround using poll() is provided.

OSS::UInt32 OSS_API socket_get_receive_timeout(socket_handle handle) ;
	/// Returns the receive timeout for the socket.
	///
	/// The returned timeout may be different than the
	/// timeout previously set with getReceiveTimeout(),
	/// as the system is free to adjust the value.

void OSS_API socket_set_linger(socket_handle handle, bool on, int seconds);
	/// Sets the value of the SO_LINGER socket option.
	
void OSS_API socket_get_linger(socket_handle handle, bool& on, int& seconds) ;
	/// Returns the value of the SO_LINGER socket option.

void OSS_API socket_set_no_delay(socket_handle handle, bool flag);
	/// Sets the value of the TCP_NODELAY socket option.
	
bool OSS_API socket_get_no_delay(socket_handle handle) ;
	/// Returns the value of the TCP_NODELAY socket option.

void OSS_API socket_set_keep_alive(socket_handle handle, bool flag);
	/// Sets the value of the SO_KEEPALIVE socket option.
	
bool OSS_API socket_get_keep_alive(socket_handle handle) ;
	/// Returns the value of the SO_KEEPALIVE socket option.

void OSS_API socket_set_reuse_address(socket_handle handle, bool flag);
	/// Sets the value of the SO_REUSEADDR socket option.

bool OSS_API socket_get_reuse_address(socket_handle handle) ;
	/// Returns the value of the SO_REUSEADDR socket option.

void OSS_API socket_set_reuse_port(socket_handle handle, bool flag);
	/// Sets the value of the SO_REUSEPORT socket option.
	/// Does nothing if the socket implementation does not
	/// support SO_REUSEPORT.

bool OSS_API socket_get_reuse_port(socket_handle handle) ;
	/// Returns the value of the SO_REUSEPORT socket option.
	///
	/// Returns false if the socket implementation does not
	/// support SO_REUSEPORT.
	
void OSS_API socket_set_oob_inline(socket_handle handle, bool flag);
	/// Sets the value of the SO_OOBINLINE socket option.

bool OSS_API socket_get_oob_inline(socket_handle handle) ;
	/// Returns the value of the SO_OOBINLINE socket option.

void OSS_API socket_set_blocking(socket_handle handle, bool flag);
	/// Sets the socket in blocking mode if flag is true,
	/// disables blocking mode if flag is false.

bool OSS_API socket_get_blocking(socket_handle handle) ;
	/// Returns the blocking mode of the socket.
	/// This method will only work if the blocking modes of 
	/// the socket are changed via the setBlocking method!

bool OSS_API socket_is_tcp(socket_handle handle);
  /// Return true if the handle is of type TCP socket

bool OSS_API socket_is_udp(socket_handle handle);
  /// Return true if the handle is of type UDP socket

OSS::IPAddress OSS_API socket_get_address(socket_handle handle) ;
	/// Returns the IP address and port number of the socket.
	
OSS::IPAddress OSS_API socket_get_peer_address(socket_handle handle) ;
	/// Returns the IP address and port number of the peer socket.

bool OSS_API socket_supports_ip4();
	/// Returns true if the system supports IPv4.
	
bool OSS_API socket_supports_ip6();
	/// Returns true if the system supports IPv6.

bool OSS_API socket_supports_poll();
  /// Returns true if the system supports the socket poll() function


//
//  TCP Socket Functions
//


socket_handle OSS_API socket_tcp_client_create();
  /// Create a new TCP client socket.  This will leak a resource
  /// if not later on freed by socket_free()

socket_handle OSS_API socket_tcp_server_create();
  /// Create a new TCP server socket.  This will leak a resource
  /// if not later on freed by socket_free()

void OSS_API socket_tcp_client_connect(socket_handle handle, const std::string& address, OSS::UInt16 port);
		/// Initializes the socket and establishes a connection to 
		/// the TCP server at the given address.
		///
		/// Can also be used for UDP sockets. In this case, no
		/// connection is established. Instead, incoming and outgoing
		/// packets are restricted to the specified address.

void OSS_API socket_tcp_client_connect(socket_handle handle, const std::string& address, OSS::UInt16 port, OSS::UInt32 timeout);
	/// Initializes the socket, sets the socket timeout and 
	/// establishes a connection to the TCP server at the given address.

void OSS_API socket_tcp_client_connect_nb(socket_handle handle, const std::string& address, OSS::UInt16 port);
	/// Initializes the socket and establishes a connection to 
	/// the TCP server at the given address. Prior to opening the
	/// connection the socket is set to nonblocking mode.

void OSS_API socket_tcp_client_shutdown_receive(socket_handle handle);
	/// Shuts down the receiving part of the socket connection.
	
void OSS_API socket_tcp_client_shutdown_send(socket_handle handle);
	/// Shuts down the sending part of the socket connection.
	
void OSS_API socket_tcp_client_shutdown(socket_handle handle);
	/// Shuts down both the receiving and the sending part
	/// of the socket connection.

int OSS_API socket_tcp_client_send_bytes(socket_handle handle, const void* buffer, int length, int flags = 0);
	/// Sends the contents of the given buffer through
	/// the socket.
	///
	/// Returns the number of bytes sent, which may be
	/// less than the number of bytes specified.

int OSS_API socket_tcp_client_receive_bytes(socket_handle handle, void* buffer, int length, int flags = 0);
	/// Receives data from the socket and stores it
	/// in buffer. Up to length bytes are received.
	///
	/// Returns the number of bytes received. 
	/// A return value of 0 means a graceful shutdown 
	/// of the connection from the peer.
	///
	/// Throws a TimeoutException if a receive timeout has
	/// been set and nothing is received within that interval.
	/// Throws a NetException (or a subclass) in case of other errors.

void OSS_API socket_tcp_client_send_urgent(socket_handle handle, unsigned char data);
	/// Sends one byte of urgent data through
	/// the socket.
	///
	/// The data is sent with the MSG_OOB flag.
	///
	/// The preferred way for a socket to receive urgent data
	/// is by enabling the SO_OOBINLINE option.

void OSS_API socket_tcp_server_bind(socket_handle handle, const std::string& address, OSS::UInt16 port, bool reuseAddress = false);
	/// Bind a local address to the socket.
	///
	/// This is usually only done when establishing a server
	/// socket. TCP clients should not bind a socket to a
	/// specific address.
	///
	/// If reuseAddress is true, sets the SO_REUSEADDR
	/// socket option.

void OSS_API socket_tcp_server_bind(socket_handle handle, OSS::UInt16 port, bool reuseAddress = false);
	/// Bind a local port to the socket.
	///
	/// This is usually only done when establishing a server
	/// socket. 
	///
	/// If reuseAddress is true, sets the SO_REUSEADDR
	/// socket option.
	
void OSS_API socket_tcp_server_listen(socket_handle handle, int backlog = 64);
	/// Puts the socket into listening state.
	///
	/// The socket becomes a passive socket that
	/// can accept incoming connection requests.
	///
	/// The backlog argument specifies the maximum
	/// number of connections that can be queued
	/// for this socket.

socket_handle OSS_API socket_tcp_server_accept_connection(socket_handle handle, std::string& address, OSS::UInt16& port);
	/// Get the next completed connection from the
	/// socket's completed connection queue.
	///
	/// If the queue is empty, waits until a connection
	/// request completes.
	///
	/// Returns a new TCP socket for the connection
	/// with the client.
	///
	/// The client socket's address is returned in clientAddr.

socket_handle OSS_API socket_tcp_server_accept_connection(socket_handle handle);
	/// Get the next completed connection from the
	/// socket's completed connection queue.
	///
	/// If the queue is empty, waits until a connection
	/// request completes.
	///
	/// Returns a new TCP socket for the connection
	/// with the client.

//
// UDP Socket functions
//

socket_handle OSS_API socket_udp_create();
  /// Create a new UDP socket.  The handle must be freed using socket_free() function.

void OSS_API socket_udp_connect(socket_handle handle, const std::string& address, OSS::UInt16 port);
	/// Restricts incoming and outgoing
	/// packets to the specified address.
	///
	/// Cannot be used together with bind().

void OSS_API socket_udp_bind(socket_handle handle, const std::string& address, OSS::UInt16 port, bool reuseAddress = false);
	/// Bind a local address to the socket.
	///
	/// This is usually only done when establishing a server
	/// socket. 
	///
	/// If reuseAddress is true, sets the SO_REUSEADDR
	/// socket option.
	///
	/// Cannot be used together with connect().

int OSS_API socket_udp_send_bytes(socket_handle handle, const void* buffer, int length, int flags = 0);
	/// Sends the contents of the given buffer through
	/// the socket.
	///
	/// Returns the number of bytes sent, which may be
	/// less than the number of bytes specified.

int OSS_API socket_udp_receive_bytes(socket_handle handle, void* buffer, int length, int flags = 0);
	/// Receives data from the socket and stores it
	/// in buffer. Up to length bytes are received.
	///
	/// Returns the number of bytes received.

int OSS_API socket_udp_send_to(socket_handle handle, const void* buffer, int length, const std::string& address, OSS::UInt16 port, int flags = 0);
	/// Sends the contents of the given buffer through
	/// the socket to the given address.
	///
	/// Returns the number of bytes sent, which may be
	/// less than the number of bytes specified.

int OSS_API socket_udp_receive_from(socket_handle handle, void* buffer, int length, std::string& address, OSS::UInt16& port, int flags = 0);
	/// Receives data from the socket and stores it
	/// in buffer. Up to length bytes are received.
	/// Stores the address of the sender in address.
	///
	/// Returns the number of bytes received.

void OSS_API socket_udp_set_broadcast(socket_handle handle, bool flag);
	/// Sets the value of the SO_BROADCAST socket option.
	///
	/// Setting this flag allows sending datagrams to
	/// the broadcast address.

bool OSS_API socket_udp_get_broadcast(socket_handle handle);
	/// Returns the value of the SO_BROADCAST socket option.

bool OSS_API http_get(const std::string& uri, std::ostream& response);
//
// IO Service functions
//

boost::asio::io_service& net_io_service();
    /// Return a raw reference to the io_service

boost::asio::ip::tcp::resolver& net_resolver();
    /// Return a raw reference to the TCP resolver

//
// Net Timer functions
//
typedef boost::function<void()> net_timer_func;

class OSS_API NetTimer
{
public:
  typedef boost::shared_ptr<NetTimer> Ptr;
  NetTimer();
  ~NetTimer();
  void onTimer(const boost::system::error_code& e);
  OSS_HANDLE _timerHandle;
  net_timer_func _handler;
};

typedef NetTimer::Ptr NET_TIMER_HANDLE;

NET_TIMER_HANDLE OSS_API net_io_timer_create(int millis, net_timer_func handler);
  /// Create a new timer and bind it the the handler.  
  /// The handler will be invoked after millis has elpased.
  ///
  /// Take note that the timer will get cancelled and destroyed after APP_TIMER_HANDLE
  /// goes out of scope

void OSS_API net_io_timer_cancel(NET_TIMER_HANDLE timerHandle);
  /// Cancel execution of a timer.  

//
// Inlines
//



} // OSS


#endif //OSS_NET_H_INCLUDED


