
// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCConsole.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/Console.h"
#include "OSS/UTL/termcolor.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTime.h>
#include <OSS/SIP/SIPParser.h>

#include "OSS/ABNF/ABNFLRSequence.h"
#include "OSS/ABNF/ABNFAnyOf.h"
#include "OSS/ABNF/ABNFSIPTopLabel.h"
#include "OSS/ABNF/ABNFSIPQuotedString.h"
#include "OSS/ABNF/ABNFSIPURIParameter.h"
#include "OSS/ABNF/ABNFLoopUntil.h"
#include "OSS/ABNF/ABNFSIPURI.h"


using namespace OSS::ABNF; 

typedef ABNFAnyOfMultiple16<
  ABNFCharAlphaNumeric,
  ABNFCharComparison<'.'>,
  ABNFCharComparison<'-'>,
  ABNFCharComparison<'@'>,
  ABNFCharComparison<':'>,
  ABNFCharComparison<','>,
  ABNFCharComparison<':'>,
  ABNFCharComparison<';'>,
  ABNFCharComparison<'+'>,
  ABNFCharComparison<'+'>,
  ABNFCharComparison<'['>,
  ABNFCharComparison<']'>
> TokenCharacterSet;
typedef ABNFLoopUntil<TokenCharacterSet, ABNFLoopExitIfNul, 0, 1024> AlphaNumericToken;
typedef ABNFAnyOfMultiple3<AlphaNumericToken, ABNFSIPQuotedString, ABNFOneOrMoreWhiteSpaces> ABNFConsoleCommandToken;
typedef ABNFLoopUntil<ABNFConsoleCommandToken, ABNFLoopExitIfNul, 0, 2048> ABNFConsoleCommand;
ABNFEvaluate<ABNFConsoleCommand> verifyConsoleCommand;
ABNFConsoleCommand commandParser;
ABNFEvaluate<ABNFOneOrMoreWhiteSpaces> isOneOrMoreWhiteSpace;

#define DEFAULT_SBC_AUXILIARY_SOCKET_ADDRESS "127.0.0.1:40550"
#define CONSOLE_CMD "cli-command"
#define CLIENT_PING_MONITOR_TIMEOUT 500
#define CLIENT_PING_MONITOR_FREQUENCY 2000


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::UTL::Console;


SBCConsole::Client::Client() :
  _pSocket(0),
  _pSubscriber(0),
  _pSubscriberThread(0),
  _pPingThread(0),
  _terminated(false),
  _monitorEvents(false)
{
}

SBCConsole::Client::~Client()
{
  terminate();
  if (_pSocket)
  {
    _pSocket->close();
    delete _pSocket;
  }
  
  if (_pSubscriber)
  {
    _pSubscriber->close();
    
    if (_pSubscriberThread)
    {
      _pSubscriberThread->join();
      delete _pSubscriberThread;
    }
    delete _pSubscriber;
  }
  
  if (_pPingThread)
  {
    _pPingThread->join();
    delete _pPingThread;
  }
}

bool SBCConsole::Client::connect(const OSS::Net::IPAddress& host)
{
  if (_pSocket)
  {
    delete _pSocket;
    _pSocket = 0;
  }
  _serverAddress = host;
  _pSocket = new OSS::ZMQ::ZMQSocket(OSS::ZMQ::ZMQSocket::REQ);
  std::ostringstream srvaddr;
  srvaddr << "tcp://" << _serverAddress.toIpPortString();
  if (!_pSocket->connect(srvaddr.str()))
  {
    return false;
  }
 
  return true;
}

bool SBCConsole::Client::subscribe()
{
  if (!_pSubscriberThread)
  {
    _pSubscriberThread = new boost::thread(boost::bind(&SBCConsole::Client::receiveEvents, this));
  }
  
  if (!_pPingThread)
  {
    _pPingThread = new boost::thread(boost::bind(&SBCConsole::Client::monitorPing, this));
  }
  return true;
}

void  SBCConsole::Client::monitorEvents(bool monitorEvents) 
{
  if (monitorEvents == _monitorEvents)
  {
    return;
  }
  _monitorEvents = monitorEvents ? subscribe() : monitorEvents;
}

void SBCConsole::Client::receiveEvents()
{
  OSS::Net::IPAddress publisher(_serverAddress);
  publisher.setPort(_serverAddress.getPort() + 1);
  
  if (_pSubscriber)
  {
    delete _pSubscriber;
    _pSubscriber = 0;
  }
  _pSubscriber = new OSS::ZMQ::ZMQSocket(OSS::ZMQ::ZMQSocket::SUB);
  std::ostringstream srvaddr;
  srvaddr << "tcp://" << publisher.toIpPortString();
  
  if (!_pSubscriber->connect(srvaddr.str()))
  {
    _monitorEvents = false;
    delete _pSubscriber;
    _pSubscriber = 0;
    return;
  }
  
  _pSubscriber->subscribe("");

  while (!_terminated)
  {
    std::string cmd;
    std::string data;
    if (_pSubscriber->receiveReply(data, 0))
    {
      if (_monitorEvents)
      {
        Poco::DateTime now;
        std::cout << std::endl << "\r" << termcolor::blue << 
          "[" << now.hour() << ":" << now.minute() << ":" << now.second() << "] " <<
          data.c_str() << termcolor::reset;
        std::cout.flush();
      }
    }
    else
    {
      if (_monitorEvents)
      {
        Poco::DateTime now;
        std::cout << std::endl << "\r" << termcolor::blue << 
          "[" << now.hour() << ":" << now.minute() << ":" << now.second() << "] " <<
          "receive reply failed!" << termcolor::reset;
        std::cout.flush();
      }
    }
  }
}

void SBCConsole::Client::monitorPing()
{
  bool displayNextSuccess = false;
  int waitTime = 0;
  while (!_terminated)
  {
    OSS::thread_sleep(1000);
    if ((waitTime += 1000) >= CLIENT_PING_MONITOR_FREQUENCY)
    {
      waitTime = 0;
      if (ping(CLIENT_PING_MONITOR_TIMEOUT))
      {
        if (_monitorEvents && displayNextSuccess)
        {
          Poco::DateTime now;
          std::cout << std::endl << "\r" << termcolor::green << 
            "[" << now.hour() << ":" << now.minute() << ":" << now.second() << "] " <<
            "Karoo Bridge Status: ONLINE" << termcolor::reset;
          std::cout.flush();
          displayNextSuccess = false;
        }
      }
      else
      {
        if (_monitorEvents && !displayNextSuccess)
        {
          Poco::DateTime now;
          std::cout << std::endl << "\r" << termcolor::red << 
            "[" << now.hour() << ":" << now.minute() << ":" << now.second() << "] " <<
            "Karoo Bridge Status: OFFLINE" << termcolor::reset;
          std::cout.flush();
          displayNextSuccess = true;
        }
      }
    }
  }
}


bool SBCConsole::Client::sendData(const std::string& data, std::string& result, int timeout)
{
  if (!_pSocket)
  {
    return false;
  }
  return _pSocket->sendAndReceive(CONSOLE_CMD, data, result, timeout);
}

bool SBCConsole::Client::ping(int timeout)
{
  std::string response;
  return sendData("ping", response, timeout);
}

bool SBCConsole::Client::requestConfiguration(int timeout)
{
  std::string response;
  if (!sendData("cli configure", response, timeout) || response.empty())
  {
    return false;
  }
  
  json::Object config;
  try
  {
    
    std::stringstream rstrm;
    rstrm << response;
    json::Reader::Read(config, rstrm);
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SBCConsole::Client - (Reader) JSON exception: " << e.what());
    return false;
  }
  
  if (config.Find("completion") != config.End())
  {
    json::Object completion = config["completion"];
    for (json::Object::iterator iter = completion.Begin(); iter != completion.End(); iter++)
    {
      Console::registerCompletion(iter->name, iter->name);
      json::Array items = iter->element;
      for(std::size_t i = 0; i < items.Size(); i++)
      {
        json::String item = items[i];
        Console::registerCompletion(iter->name, item.Value());
      }
    }
  }
      
  return true;
}


////////////////////////////////////////////////////////////////////////////////


SBCConsole::SBCConsole()
{
  _socket.addHandler("cli-command", boost::bind(&SBCConsole::handleCommand, this, _1, _2));
  
  std::ostringstream exitString;
  exitString << "~~sbc-console-exit-" << this;
  _socket.setExitString(exitString.str());
  
  addCommandHandler(boost::bind(&SBCConsole::handleConfigurationRequest, this, _1, _2));
  addCommandHandler(boost::bind(&SBCConsole::handlePingRequest, this, _1, _2));
  addCommandHandler(boost::bind(&SBCConsole::handleSyntaxRequest, this, _1, _2));
  addCommandHandler(boost::bind(&SBCConsole::handleHelpRequest, this, _1, _2));
  addCommandHandler(boost::bind(&SBCConsole::handlePublishRequest, this, _1, _2));
}
  
SBCConsole::~SBCConsole()
{
  stop();
}
  
void SBCConsole::handleCommand(const std::string& data, std::string& result)
{
  CommandTokens commandTokens;
  if (SBCConsole::getCommandTokens(data, commandTokens))
  {
    for (CommandHandlers::iterator handler = _handlers.begin(); handler != _handlers.end(); handler++)
    {
      if ((*handler)(commandTokens,result))
      {
        return;
      }
    }
  }
  else
  {
    result = "[ERROR] Syntax Error - `" + data + "`";
    return;
  }
  result = "[ERROR] Unknown command [" + data + "]";
}

void SBCConsole::run()
{
  SBCAuxiliarySocket::Properties auxConfig;
  auxConfig.bindAddress = OSS::Net::IPAddress::fromV4IPPort(DEFAULT_SBC_AUXILIARY_SOCKET_ADDRESS);
  auxConfig.publisherAddress = OSS::Net::IPAddress::fromV4IPPort(DEFAULT_SBC_AUXILIARY_SOCKET_ADDRESS);
  auxConfig.publisherAddress.setPort(auxConfig.bindAddress.getPort() + 1);
  _socket.run(auxConfig);
  _internalClient.connect(OSS::Net::IPAddress::fromV4IPPort(DEFAULT_SBC_AUXILIARY_SOCKET_ADDRESS));
}

void SBCConsole::stop()
{
  std::string response;
  _internalClient.sendData(_socket.getExitString(), response, 1000);
}

void SBCConsole::publishEvent(const std::string& event)
{
  _socket.publish(event);
}

void SBCConsole::addCommand(const std::string& commandToken, const std::string& completionString, const std::string& helpString)
{
  CompletionMap::iterator token = _completion.find(commandToken);
  CompletionParams params;
  params.push_back(completionString);
  params.push_back(helpString);
  if (token == _completion.end())
  {
    Completion completion;
    completion.push_back(params);
    _completion[commandToken] = completion;
  }
  else
  {
    token->second.push_back(params);
  }
}

void SBCConsole::addCommandHandler(const CommandHandler& handler)
{
  _handlers.push_back(handler);
}

bool SBCConsole::handleSyntaxRequest(const SBCConsole::CommandTokens& data, std::string& result)
{
  if (!SBCConsole::isCommand("syntax", data))
  {
    return false;
  }
  
  std::ostringstream strm;
  for (std::size_t i = 1; i < data.size(); i++)
  {
    strm << "token[" << i-1 << "] = " << data[i] << std::endl; 
  }
  result = strm.str();
  return true;
}

bool SBCConsole::handlePingRequest(const SBCConsole::CommandTokens& data, std::string& result)
{
  if (!SBCConsole::isCommand("ping", data))
  {
    return false;
  }
  result = "pong";
  return true;
}

bool SBCConsole::handleConfigurationRequest(const SBCConsole::CommandTokens& data, std::string& result)
{
  if (!SBCConsole::isCommand("cli configure", data))
  {
    return false;
  }
  json::Object config;
  
  //
  // Add completions
  //
  for (CompletionMap::iterator token = _completion.begin(); token != _completion.end(); token++)
  {
    json::Array completionVector;
    for (Completion::iterator completion = token->second.begin(); completion != token->second.end(); completion++)
    {
      completionVector.Insert(json::String(completion->front()));
    }
    config["completion"][token->first] = completionVector;
  }
  
  try
  {
    std::stringstream strm;
    json::Writer::Write(config, strm);
    result = strm.str();
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SBCConsole::handleConfigurationRequest - (Writer) JSON exception: " << e.what());
  }
  return true;
}

bool SBCConsole::handleHelpRequest(const SBCConsole::CommandTokens& data, std::string& result)
{
  std::ostringstream helpText;
  std::string back(data.back());
  OSS::string_to_lower(back);
  std::string front(data.front());
  OSS::string_to_lower(front);
  std::size_t count = 0;
  
  if (back != "help")
  {
    return false;
  }
  
  if (front == "help")
  {
    helpText << "Type [command] help.  Available commands are: " << std::endl;
    for (CompletionMap::iterator token = _completion.begin(); token != _completion.end(); token++)
    {
      helpText << " - " << token->first;
      if (++count < _completion.size())
      {
        helpText << std::endl;
      }
    }
    result = helpText.str();
    return true;
  }
  
  CompletionMap::iterator token = _completion.find(front);
  if (token != _completion.end())
  {
    Completion& completion = token->second;
    for (Completion::iterator iter = completion.begin(); iter != completion.end(); iter++)
    {
      CompletionParams& completionParams = *iter;
      helpText << completionParams[0] << " : " << completionParams[1];
      if (++count < completion.size())
      {
        helpText << std::endl;
      }
    }
  }
  else
  {
    helpText << "No help entry for " << front;
  }
  
  result = helpText.str();
  return true;
}

bool SBCConsole::handlePublishRequest(const SBCConsole::CommandTokens& data, std::string& result)
{
  if (!SBCConsole::isCommand("publish", data))
  {
    return false;
  }
  if (data.size() == 2)
  {
    publishEvent(data[1]);
  }
  result = "noprint";
  return true;
}

bool SBCConsole::verifyCommandSyntax(const std::string& command)
{
  return verifyConsoleCommand(command.c_str());
}

bool SBCConsole::getCommandTokens(const std::string& command, CommandTokens& tokens)
{
  CommandTokens commandTokens;
  if (!verifyCommandSyntax(command))
  {
    return false;
  }
  
  if (!commandParser.parseTokens(command.c_str(), commandTokens))
  {
    return false;
  }
  
  for (CommandTokens::iterator iter = commandTokens.begin(); iter != commandTokens.end(); iter++)
  {
    if (!isOneOrMoreWhiteSpace(iter->c_str()))
    {
      OSS::string_trim(*iter);
      OSS::SIP::SIPParser::unquoteString(*iter);
      tokens.push_back(*iter);
    }
  }
  return !tokens.empty();
}

bool SBCConsole::isCommand(const std::string& command, const CommandTokens& tokens)
{
  CommandTokens commandTokens;
  if (!getCommandTokens(command, commandTokens))
  {
    return false;
  }
  
  if (tokens.size() < commandTokens.size())
  {
    return false;
  }
  
  for (std::size_t i = 0; i < commandTokens.size(); i++)
  {
    if (commandTokens[i] != tokens[i])
    {
      return false;
    }
  }
  
  return true;
}


} } } // OSS::SIP::SBC

