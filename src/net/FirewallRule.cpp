/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include <sstream>
#include "OSS/Net/FirewallRule.h"


namespace OSS {
namespace Net {


FirewallRule::FirewallRule() :
  _sourcePort(0),
  _sourceEndPort(0),
  _destinationPort(0),
  _destinationEndPort(0),
  _protocol(-1),
  _direction(-1),
  _operation(-1)
{
}

FirewallRule::FirewallRule(const FirewallRule& rule) :
  _sourcePort(0),
  _sourceEndPort(0),
  _destinationPort(0),
  _destinationEndPort(0),
  _protocol(-1),
  _direction(-1),
  _operation(-1)
{
  _device = rule._device;
  _sourceAddress = rule._sourceAddress;
  _sourcePort = rule._sourcePort;
  _sourceEndPort = rule._sourceEndPort;
  _destinationAddress = rule._destinationAddress;
  _destinationPort = rule._destinationPort;
  _destinationEndPort = rule._destinationEndPort;
  _protocol = rule._protocol;
  _direction = rule._direction;
  _operation = rule._operation;
}

FirewallRule::FirewallRule(
  const std::string& device,
  const std::string& sourceAddress,
  unsigned short sourcePort,
  unsigned short sourceEndPort,
  const std::string& destinationAddress,
  unsigned short destinationPort,
  unsigned short destinationEndPort,
  int protocol,
  int direction,
  int operation
) : _device(device),
    _sourceAddress(sourceAddress),
    _sourcePort(sourcePort),
    _sourceEndPort(sourceEndPort),
    _destinationAddress(destinationAddress),
    _destinationPort(destinationPort),
    _destinationEndPort(destinationEndPort),
    _protocol(protocol),
    _direction(direction),
    _operation(operation)

{
}

FirewallRule::~FirewallRule()
{
}

void FirewallRule::swap(FirewallRule& rule)
{
  std::swap(_device, rule._device);
  std::swap(_sourceAddress, rule._sourceAddress);
  std::swap(_sourcePort, rule._sourcePort);
  std::swap(_sourceEndPort, rule._sourceEndPort);
  std::swap(_destinationAddress, rule._destinationAddress);
  std::swap(_destinationPort, rule._destinationPort);
  std::swap(_destinationEndPort, rule._destinationEndPort);
  std::swap(_protocol, rule._protocol);
  std::swap(_direction, rule._direction);
  std::swap(_operation, rule._operation);
}

FirewallRule& FirewallRule::operator=(const FirewallRule& rule)
{
  FirewallRule clone(rule);
  swap(clone);
  return *this;
}

std::string FirewallRule::str() const
{
  std::ostringstream strm;
  strm <<  _device << "\t";
  strm <<  _sourceAddress << "\t";
  strm <<  _sourcePort << ":";
  strm <<  _sourceEndPort << "\t";
  strm <<  _destinationAddress<< "\t";
  strm <<  _destinationPort << ":";
  strm <<  _destinationEndPort << "\t";
  strm <<  _protocol << "\t";
  strm <<  _direction << "\t";
  strm <<  _operation;
  return strm.str();
}


} } // OSS::Net



