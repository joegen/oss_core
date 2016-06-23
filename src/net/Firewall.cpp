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
#include "OSS/Net/Firewall.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Exec/Command.h"


extern "C" { size_t strlcpy(char *dst, const char *src, size_t siz); };

#define IP_TABLES_BIN "/sbin/iptables"

namespace OSS {
namespace Net {

//
// Start of IPTables functions
//

std::string iptable_add_rule(const FirewallRule& rule)
{
  assert (rule.getDirection() != -1 && rule.getOperation() != -1);

  std::ostringstream cmd;
#ifdef OSS_IP_TABLES
  cmd << OSS_IP_TABLES << "  --append";
#else
  cmd << IP_TABLES_BIN << " --append";
#endif
  //
  // Check which chain we are concerned with
  //
  if (rule.getDirection() == FirewallRule::DIR_IN)
  {
    cmd << " INPUT ";
    if (!rule.getDevice().empty())
      cmd << " --in-interface " << rule.getDevice();
  }else
  {
    cmd << " OUTPUT ";
    if (!rule.getDevice().empty())
      cmd << " --out-interface " << rule.getDevice();
  }

  //
  // Check if source address is set
  //
  if (!rule.getSourceAddress().empty())
    cmd << " --source " << rule.getSourceAddress();

  //
  // Check if the destination address is set
  //
  if (!rule.getDestinationAddress().empty())
    cmd << " --destination " << rule.getDestinationAddress();

  //
  // Check if the source port(s) is set
  //
  if (rule.getSourcePort() > 0)
  {
    cmd << " --source-ports " << rule.getSourcePort();
    if (rule.getSourceEndPort() > 0)
      cmd << ":" << rule.getSourceEndPort();
  }

  //
  // Check if the destination port(s) is set
  //
  if (rule.getDestinationPort() > 0)
  {
    cmd << " --destination-ports " << rule.getDestinationPort();
    if (rule.getDestinationEndPort() > 0)
      cmd << ":" << rule.getDestinationEndPort();
  }

  //
  // Check if the protocol is set
  //
  if (rule.getProtocol() != -1)
    cmd << " --protocol " << rule.getProtocol();

  //
  // Now we jump!
  //
  if (rule.getOperation() == FirewallRule::OP_ALLOW)
    cmd << " --jump " << "ACCEPT";
  else
    cmd << " --jump " << "DENY";

  return cmd.str();
}

static std::string iptables_delete(FirewallRule::Direction direction,  std::size_t index)
{
  std::ostringstream cmd;
#ifdef OSS_IP_TABLES
  cmd << OSS_IP_TABLES << "  --delete";
#else
  cmd << IP_TABLES_BIN << " --delete";
#endif
  
  if (direction == FirewallRule::DIR_IN)
    cmd << " INPUT ";
  else
    cmd << " OUTPUT ";

  cmd << index;

  return cmd.str();
}

static std::string iptables_get_rules(FirewallRule::Direction direction)
{
  std::ostringstream cmd;
  
#ifdef OSS_IP_TABLES
  cmd << OSS_IP_TABLES << "  --list-rules";
#else
  cmd << IP_TABLES_BIN << " --list-rules";
#endif
  
  if (direction == FirewallRule::DIR_IN)
    cmd << " INPUT ";
  else
    cmd << " OUTPUT ";

  return cmd.str();
}


Firewall& Firewall::instance()
{
  static Firewall fw;
  return fw;
}

Firewall::Firewall()
{
}

Firewall::~Firewall()
{
}




bool Firewall::iptAddRule(const FirewallRule& rule)
{
  OSS_EXEC(iptable_add_rule(rule));
  return true;
}

bool Firewall::iptDeleteRule(FirewallRule::Direction direction, std::size_t index)
{
  OSS_EXEC(iptables_delete(direction, index));
  return true;
}



void Firewall::iptGetRules(FirewallRule::Direction direction, std::vector<std::string>& rules)
{
  OSS_EXEC_EX(iptables_get_rules(direction), rules);
}


} } // OSS::Net



