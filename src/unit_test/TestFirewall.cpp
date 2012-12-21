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


#include "gtest/gtest.h"
#include "OSS/Net/Firewall.h"
#include <boost/bind.hpp>

using namespace OSS;
using namespace OSS::Net;

#ifdef TEST_FIREWALL_RULE
TEST(FirewallTest, test_Add_rule)
{

  OSS::Net::FirewallRule rule1;
  rule1.setDevice("p33p1");
  rule1.setSourceAddress("192.169.1.250");
  rule1.setOperation(OSS::Net::FirewallRule::OP_ALLOW);
  rule1.setProtocol(OSS::Net::FirewallRule::PROTO_TCP);
  rule1.setDirection(OSS::Net::FirewallRule::DIR_IN);
  ASSERT_TRUE(OSS::Net::Firewall::instance().iptAddRule(rule1));
}


TEST(FirewallTest, test_print_rules)
{
  std::vector<std::string> input;
  std::vector<std::string> output;

  OSS::Net::Firewall::instance().iptGetRules(OSS::Net::FirewallRule::DIR_IN, input);
  OSS::Net::Firewall::instance().iptGetRules(OSS::Net::FirewallRule::DIR_OUT, output);

  ASSERT_TRUE(!input.empty() || !output.empty());
}
#endif
