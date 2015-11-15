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


#include "OSS/SDP/ICECandidate.h"


namespace OSS {
namespace SDP {  



ICECandidate::ICECandidate()
{
}

ICECandidate::ICECandidate(const ICECandidate& candidate)
{
  _component = candidate._component;
  _priority = candidate._priority;
  _port = candidate._port;
  _identifier = candidate._identifier;
  _ip = candidate._ip;
  _type = candidate._type;
  _protocol = candidate._protocol;
  _generation = candidate._generation;
}

ICECandidate::ICECandidate(const std::string& candidate)
{
  parseCandidate(candidate);
}

ICECandidate::~ICECandidate()
{
}

ICECandidate& ICECandidate::operator=(const ICECandidate& candidate)
{
  ICECandidate clonable(candidate);
  swap(clonable);
  return *this;
}

ICECandidate& ICECandidate::operator=(const std::string& candidate)
{
  ICECandidate clonable(candidate);
  swap(clonable);
  return *this;
  return *this;
}

void ICECandidate::swap(ICECandidate& candidate)
{
  std::swap(_component, candidate._component);
  std::swap(_priority, candidate._priority);
  std::swap(_port, candidate._port);
  std::swap(_identifier, candidate._identifier);
  std::swap(_ip, candidate._ip);
  std::swap(_type, candidate._type);
  std::swap(_protocol, candidate._protocol);
  std::swap(_generation, candidate._generation);
}

bool ICECandidate::parseCandidate(const std::string& candidate)
{ 
  //             0          1 2   3          4            5     6   7    8          9
  // a=candidate:3988902457 1 udp 2122260223 192.168.0.11 38639 typ host generation 0
  // a=candidate:3988902457 2 udp 2122260222 192.168.0.11 57406 typ host generation 0
  
  //             0          1 2   3          4            5 6   7    8       9      10         11
  // a=candidate:2739023561 1 tcp 1518280447 192.168.0.11 0 typ host tcptype active generation 0
  // a=candidate:2739023561 2 tcp 1518280446 192.168.0.11 0 typ host tcptype active generation 0
  
  //             0          1 2   3          4             5     6   7     8     9            10    11    12         13  
  // a=candidate:4667258690 1 udp 1686052607 49.146.254.61 59858 typ srflx raddr 192.168.0.11 rport 59858 generation 0
  // a=candidate:4667258690 2 udp 1686052606 49.146.254.61 46555 typ srflx raddr 192.168.0.11 rport 46555 generation 0
  
  std::vector<std::string> tokens = OSS::string_tokenize(candidate, " ");
  if (tokens.size() == 10)
  {
    int index = 0;
    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
    {
      switch (index)
      {
        case 0:
          //
          // Identifier
          //
          _identifier = *iter;
          break;
        case 1:
          //
          // Component (1 for data 2 for control)
          //
          _component = OSS::string_to_number<unsigned int>(*iter);
          break;
        case 2:
          //
          // Protocol
          //
          _protocol = *iter;
          break;
        case 3:
          //
          // Priority
          //
          _priority = OSS::string_to_number<unsigned long>(*iter);
          break;
        case 4:
          //
          // IP Address
          //
          _ip = *iter;
          break;
        case 5:
          //
          // Port
          //
          _port = OSS::string_to_number<unsigned short>(*iter);
          break;
        case 7:
          //
          // Type
          //
          _type = *iter;
          break;
        case 9:
          //
          // Generation
          //
          _generation = OSS::string_to_number<unsigned int>(*iter);
          break;

      };
      ++index;
    }
  }
  
  return true;
}

void ICECandidate::formatCandidate()
{
  // a=candidate:3988902457 1 udp 2122260223 192.168.0.11 38639 typ host generation 0
  // a=candidate:3988902457 2 udp 2122260222 192.168.0.11 57406 typ host generation 0
  
  std::ostringstream strm;
  strm << "a=candidate:" << _identifier;
  strm << " " << _component;
  strm << " " << _protocol;
  strm << " " << _priority;
  strm << " " << _ip;
  strm << " " << _port;
  strm << " type " << _type;
  strm << " generation " << _generation;
  
  _data = strm.str();
}
  

const std::string& ICECandidate::str()
{
  formatCandidate();
  return _data;
}



  
 


} } // OSS::SDP



