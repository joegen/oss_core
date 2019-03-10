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



#ifndef OSS_SBCCONSOLE_H_INCLUDED
#define	OSS_SBCCONSOLE_H_INCLUDED


#include "OSS/SIP/SBC/SBCAuxiliarySocket.h"
#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/UTL/termcolor.h"


namespace OSS {
namespace SIP {
namespace SBC {


class SBCConsole
{
public:
  
  typedef std::vector<std::string> CommandTokens;
  typedef boost::function<bool(const CommandTokens&, std::string&result)> CommandHandler;
  typedef std::vector<CommandHandler> CommandHandlers;
  
  
  class Client
  {
  public:
    Client();
    ~Client();
    bool connect(const OSS::Net::IPAddress& serverAddr);
    bool ping(int timeout);
    bool requestConfiguration(int timeout);
    bool sendData(const std::string& data, std::string& result, int timeout);
    void terminate() {_terminated = true;};
    void monitorEvents(bool monitorEvents);
  
  protected:
    friend class SBCConsole;
    bool subscribe();
    void receiveEvents();
    void monitorPing();
    OSS::ZMQ::ZMQSocket* _pSocket;
    OSS::ZMQ::ZMQSocket* _pSubscriber;
    boost::thread* _pSubscriberThread;
    boost::thread* _pPingThread;
    bool _terminated;
    bool _monitorEvents;
    OSS::Net::IPAddress _serverAddress;
  };
  
  SBCConsole();
  
  ~SBCConsole();
  
  void run();
  
  void stop();
  
  void addCommand(const std::string& commandToken, const std::string& completion, const std::string& helpString);
  
  void addCommandHandler(const CommandHandler& handler);
  
  void publishEvent(const std::string& event);
  
  static bool verifyCommandSyntax(const std::string& command);

  static bool getCommandTokens(const std::string& command, CommandTokens& tokens);
  
  static bool isCommand(const std::string& command, const CommandTokens& tokens);

protected:
  void handleCommand(const std::string& data, std::string& result);
  bool handleConfigurationRequest(const SBCConsole::CommandTokens& data, std::string& result);
  bool handlePingRequest(const SBCConsole::CommandTokens& data, std::string& result);
  bool handleSyntaxRequest(const SBCConsole::CommandTokens& data, std::string& result);
  bool handleHelpRequest(const SBCConsole::CommandTokens& data, std::string& result);
  bool handlePublishRequest(const SBCConsole::CommandTokens& data, std::string& result);
  
  Client _internalClient;
  SBCAuxiliarySocket _socket;
  typedef std::vector<std::string> CompletionParams;
  typedef std::vector<CompletionParams> Completion;
  typedef std::map<std::string, Completion> CompletionMap;
  CompletionMap _completion;
  CommandHandlers _handlers;
};

#define SBC_CONSOLE_LOG_GREEN(log) std::cout << termcolor::green << log << termcolor::reset << std::endl
#define SBC_CONSOLE_LOG_RED(log) std::cout << termcolor::red << log << termcolor::reset << std::endl
#define SBC_CONSOLE_LOG_BLUE(log) std::cout << termcolor::blue << log << termcolor::reset << std::endl


} } } // OSS::SIP::SBC

#endif	// OSS_SBCCONSOLE_H_INCLUDED

