// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: CLI
//
// Copyright (c) OSS Software Solutions
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


#ifndef CLI_CLIENT_H_INCLUDED
#define	CLI_CLIENT_H_INCLUDED


#include "OSS/Net.h"


namespace OSS {
namespace CLI {


class Client : boost::noncopyable
{
public:
  Client();

  Client(const OSS::IPAddress& host,
    const std::string& user,
    const std::string& password);

  ~Client();

  bool Connect(const OSS::IPAddress& host,
    const std::string& user,
    const std::string& password,
    unsigned timeout = 5000);

  bool isConnected();

  bool execute(
    const std::string& commandGroup,
    const std::string& command,
    const std::string& args,
    std::string& response);

  void close();

  void startConsole();

protected:
  bool login();
  bool readUntil(const std::string& mark, std::string& buffer);
  bool writeString(const std::string& buff);
private:
  OSS::IPAddress _host;
  std::string _user;
  std::string _password;
  
  enum State { Idle, Authenticating, Connected }_state;
  OSS::socket_handle _socket;
  bool _isConnected;
  bool _isConsole;
};


//
// Inlines
//
inline bool Client::isConnected()
{
  return _isConnected;
}

} }


#endif	// CLI_CLIENT_H_INCLUDED

