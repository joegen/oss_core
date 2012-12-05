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
#include "OSS/Net/Firewall.h"
#include "OSS/Logger.h"


extern "C" { size_t strlcpy(char *dst, const char *src, size_t siz); };


namespace OSS {
namespace Net {


fw_t* _pInstance = 0;

Firewall& Firewall::instance()
{
  static Firewall fw;
  return fw;
}

Firewall::Firewall()
{
  _pInstance = fw_open();
}

Firewall::~Firewall()
{
  if (_pInstance)
    fw_close(_pInstance);
}

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


bool Firewall::addRule(const FirewallRule& rule)
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

bool Firewall::deleteRule(FirewallRule::Table::iterator& iter)
{
  if (!_pInstance)
  {
    OSS_LOG_ERROR("Dnet Firewall hook is not open.");
    return false;
  }

  fw_rule fr;
  rule_to_fwrule(*iter, &fr);
  int ret = fw_delete(_pInstance, &fr);
  if (ret < 0)
  {
    OSS_LOG_ERROR("Firewall:  Error deleting rule (ret=" << ret << ")");
    return false;
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

  (*pHandler)(rule);
  return 0;
}

void Firewall::tableLoop(TableLoopHandler& handler)
{
  if (!_pInstance)
  {
    OSS_LOG_ERROR("Dnet Firewall hook is not open.");
    return;
  }
  fw_loop(_pInstance, internal_table_loop, (void*)&handler);
}


} } // OSS::Net



