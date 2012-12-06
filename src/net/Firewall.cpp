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


#include <dnet.h>
#include <sstream>
#include "OSS/Net/Firewall.h"
#include "OSS/Logger.h"
#include "OSS/Exec/Command.h"


extern "C" { size_t strlcpy(char *dst, const char *src, size_t siz); };

namespace OSS {
namespace Net {

//
// DNET specific functions
//
fw_t* _pInstance = 0;

static bool rule_to_fwrule(const FirewallRule& rule, fw_rule* fr)
{
  fr->fw_proto = rule.getProtocol();
  fr->fw_op = rule.getOperation();
  fr->fw_dir = rule.getDirection();

  if (!rule.getDevice().empty())
    strlcpy(fr->fw_device, rule.getDevice().c_str(), sizeof(fr->fw_device));

  if (!rule.getSourceAddress().empty())
  {
    if (addr_pton(rule.getSourceAddress().c_str(), &fr->fw_dst) < 0)
        return false;
  }

  if (!rule.getDestinationAddress().empty())
  {
    if (addr_pton(rule.getDestinationAddress().c_str(), &fr->fw_dst) < 0)
        return false;
  }

  if (rule.getSourcePort() != 0)
  {
    fr->fw_sport[0] = rule.getSourcePort();
    if (rule.getSourceEndPort() != 0)
      fr->fw_sport[1] = rule.getSourceEndPort();
    else
      fr->fw_sport[1] = rule.getSourcePort();
  }
  else if (fr->fw_proto == IP_PROTO_TCP || fr->fw_proto == IP_PROTO_UDP)
  {
    fr->fw_sport[0] = 0;
    fr->fw_sport[1] = TCP_PORT_MAX;
  }

  if (rule.getDestinationPort() != 0)
  {
    fr->fw_dport[0] = rule.getDestinationPort();
    if (rule.getDestinationEndPort() != 0)
      fr->fw_dport[1] = rule.getDestinationEndPort();
    else
      fr->fw_dport[1] = rule.getDestinationPort();
  }
  else if (fr->fw_proto == IP_PROTO_TCP || fr->fw_proto == IP_PROTO_UDP)
  {
    fr->fw_dport[0] = 0;
    fr->fw_dport[1] = TCP_PORT_MAX;
  }

  return true;
}

static int internal_table_loop(const struct fw_rule *fr, void *arg)
{
  Firewall::TableLoopHandler* pHandler = reinterpret_cast<Firewall::TableLoopHandler*>(arg);

  FirewallRule rule(
      fr->fw_device ? fr->fw_device : "",
      fr->fw_src.addr_type ? addr_ntoa(&fr->fw_src) : "",
      fr->fw_sport[0],
      fr->fw_sport[1],
      fr->fw_src.addr_type ? addr_ntoa(&fr->fw_dst) : "",
      fr->fw_dport[0],
      fr->fw_dport[1],
      fr->fw_proto,
      fr->fw_dir,
      fr->fw_op
      );

  (*pHandler)(rule.str());
  return 0;
}

//
//  EOF Dnet functions
//


//
// Start of IPTables functions
//

std::string iptable_add_rule(const FirewallRule& rule)
{
  assert (rule.getDirection() != -1 && rule.getOperation() != -1);

  std::ostringstream cmd;
  cmd << "/sbin/iptables --append";

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
  cmd << "/sbin/iptables --delete";

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
  cmd << "/sbin/iptables --list-rules";

  if (direction == FirewallRule::DIR_IN)
    cmd << " INPUT ";
  else
    cmd << " OUTPUT ";

  return cmd.str();
}


Firewall& Firewall::instance()
{
  static Firewall fw(false /* o not use libdnet */);
  return fw;
}

Firewall::Firewall(bool useLibDnet) :
  _useLibDnet(useLibDnet)
{
  if (_useLibDnet)
  {
    _pInstance = fw_open();
  }
}

Firewall::~Firewall()
{
  if (_useLibDnet)
  {
    if (_pInstance)
      fw_close(_pInstance);
  }
}


bool Firewall::dnetAddRule(const FirewallRule& rule)
{
  if (_useLibDnet)
  {
    if (!_pInstance)
    {
      OSS_LOG_ERROR("Dnet Firewall hook is not open.");
      return false;
    }

    fw_rule fr;
    if (!rule_to_fwrule(rule, &fr))
    {
      OSS_LOG_ERROR("Dnet Firewall hook is not open.");
      return false;
    }
    int ret = fw_add(_pInstance, &fr);
    if (ret < 0)
    {
      OSS_LOG_ERROR("Firewall:  Error adding rule (ret=" << ret << ")");
      return false;
    }
    return true;
  }
  return false;
}

bool Firewall::iptAddRule(const FirewallRule& rule)
{
  OSS_EXEC(iptable_add_rule(rule));
  return true;
}

bool Firewall::iptDeleteRule(FirewallRule::Direction direction, std::size_t index)
{
  if (!_useLibDnet)
  {
    OSS_EXEC(iptables_delete(direction, index));
    return true;
  }
  return false;
}

bool Firewall::dnetDeleteRule(const FirewallRule& rule)
{
  if (_useLibDnet)
  {
    if (!_pInstance)
    {
      OSS_LOG_ERROR("Dnet Firewall hook is not open.");
      return false;
    }

    fw_rule fr;
    rule_to_fwrule(rule, &fr);
    int ret = fw_delete(_pInstance, &fr);
    if (ret < 0)
    {
      OSS_LOG_ERROR("Firewall:  Error deleting rule (ret=" << ret << ")");
      return false;
    }
  }
  return true;
}


void Firewall::dnetTableLoop(TableLoopHandler& handler)
{
  if (_useLibDnet)
  {
    if (!_pInstance)
    {
      OSS_LOG_ERROR("Dnet Firewall hook is not open.");
      return;
    }
    fw_loop(_pInstance, internal_table_loop, (void*)&handler);
  }
}

void Firewall::iptGetRules(FirewallRule::Direction direction, std::vector<std::string>& rules)
{
  OSS_EXEC_EX(iptables_get_rules(direction), rules);
}


} } // OSS::Net



