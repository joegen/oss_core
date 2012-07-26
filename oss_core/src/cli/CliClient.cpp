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


#include "OSS/Core.h"
#include "OSS/CLI/Client.h"

#include <iostream>
#include <termios.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <stdio.h>

namespace OSS {
namespace CLI {


const int receive_buffer_size = 24;
const std::string user_prompt = "Username: ";
const std::string password_prompt = "Password: ";
const std::string command_prompt = "karoo> ";

Client::Client() :
  _state(Idle),
  _socket(0),
  _isConnected(false),
  _isConsole(false)
{
  _socket = OSS::socket_tcp_client_create();
  //if (_socket)
  //{
  //  OSS::socket_set_receive_buffer_size(_socket, receive_buffer_size);
  //}
}

Client::Client(const OSS::IPAddress& host,
  const std::string& user,
  const std::string& password)
{
  _socket = OSS::socket_tcp_client_create();
  //if (_socket)
  //{
  //  OSS::socket_set_receive_buffer_size(_socket, receive_buffer_size);
  //}
}

Client::~Client()
{
  OSS::socket_free(_socket);
}

bool Client::Connect(const OSS::IPAddress& host,
  const std::string& user,
  const std::string& password,
  unsigned timeout)
{
  try
  {
    OSS::socket_tcp_client_connect(
      _socket, host.toString(),
      host.getPort(),
      timeout);
  }
  catch(std::exception&e)
  {
    return false;
  }
  _user = user;
  _password = password;
  _isConnected = login();
  return _isConnected;
}

bool Client::login()
{
  _state = Authenticating;
  //
  // Read the welcome screen up to the prompt
  //
  std::string userPrompt;
  std::string command = _user + "\r\n";
  if (readUntil(user_prompt, userPrompt) && writeString(command))
  {
    std::string passwordPrompt;
    std::string command = _password + "\r\n";
    if (readUntil(password_prompt, passwordPrompt) && writeString(command))
    {
      std::string commandPrompt;
      return readUntil(command_prompt, commandPrompt);
    }

    return false;
  }
  return false;
}

bool Client::readUntil(const std::string& mark, std::string& buffer)
{
  if (!_socket)
    return false;
  buffer = std::string();

  for(;;)
  {
    char receiveBuffer[receive_buffer_size];
    size_t read = 0;
    try
    {
      read = OSS::socket_tcp_client_receive_bytes(
        _socket,
        receiveBuffer,
        receive_buffer_size);
    }
    catch (std::exception&e)
    {
      break;
    }
    if (read > 0)
    {
      buffer += std::string(receiveBuffer, read);
      if (OSS::string_right(buffer, mark.size()) == mark)
      {
        if (_isConsole)
        {
          std::vector<std::string> lines = OSS::string_tokenize(buffer, "\n");
          for (int i = 1; i < lines.size(); i++)
          {
            std::cout << lines[i];
            if (i < lines.size() - 1)
              std::cout << std::endl;
          }
        }
        return true;
      }
    }
    else
    {
      break;
    }
  }

  buffer = std::string();
  return false;
}

bool Client::writeString(const std::string& buff)
{
  if (!_socket)
    return false;
  int sent = 0;
  try
  {
    sent = socket_tcp_client_send_bytes(_socket, buff.c_str(), buff.size());
  }
  catch(std::exception&e)
  {
    return false;
  }
  return sent > 0;
}

bool Client::execute(
  const std::string& commandGroup,
  const std::string& command,
  const std::string& args,
  std::string& response)
{
  return false;
}

void Client::close()
{

}

void Client::startConsole()
{
  
  if (!_isConnected)
    return;

  _isConsole = true;
  
  for (;;)
  {
    std::string buff;
    while(true)
    {
      int c = getchar();
      if (c == 10)
        break;
      else
        buff += c;
    }

    
    if (buff.empty())
    {
      std::cout << command_prompt;
      continue;
    }

    buff += "\r\n";
    try
    {
      if (buff == "exit\r\n")
        buff = "quit\r\n";
      OSS::socket_tcp_client_send_bytes(_socket, buff.c_str(), buff.size());
      if( buff == "quit\r\n")
        break;
      std::string readBuff;
      readUntil(command_prompt, readBuff);
    }
    catch(std::exception& e)
    {
      break;
    }
  }
}


} }



