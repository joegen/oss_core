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

#ifndef FIREWALL_H_
#define FIREWALL_H_


#include <map>
#include <list>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "OSS/Net/FirewallRule.h"


namespace OSS {
namespace Net {


class Firewall : boost::noncopyable
{
  //
  // This is a wrapper class around libdnet firewall functions and iptables.
  // libdnet only supports ipchains which is now obsolete in most
  // recent linux kernels and replaced by iptables.  The current active implementation
  // class is iptables.  All functions are prefix with dnet and ipt (iptables) respectively
  //
public:
  typedef boost::function<void(const std::string&)> TableLoopHandler;
  static Firewall& instance();

  bool dnetAddRule(const FirewallRule& rule);
    /// Adds a new firewall rule

  bool dnetDeleteRule(const FirewallRule&);
    /// Delete the rule by valaue.

  void dnetTableLoop(TableLoopHandler& handler);
    /// Loop through all rules.  String representation of the rule is
    /// pushed the the handle callback.

  bool iptAddRule(const FirewallRule& rule);
    /// Adds a new firewall rule

  bool iptDeleteRule(FirewallRule::Direction direction, std::size_t index);
    /// Deletes the rule in this particular position.

  void iptGetRules(FirewallRule::Direction direction, std::vector<std::string>& rules);
    /// Get the rules for a specific chain.

private:
  friend Firewall& instance();
  Firewall(bool useLibDnet = false);
  ~Firewall();
  bool _useLibDnet;
};


//
// Inlines
//


} } // OSS::Net


#endif /* FIREWALL_H_ */
