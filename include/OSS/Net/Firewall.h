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
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "OSS/Net/FirewallRule.h"


namespace OSS {
namespace Net {


class Firewall : boost::noncopyable
{
  //
  // This is a wrapper class around libdnet firewall functions.
  // libdnet only supports ipchains which is now obsolete in most
  // recent linux kernels and replaced by iptables.
  //
public:
  typedef boost::function<void(const FirewallRule&)> TableLoopHandler;
  static Firewall& instance();
  bool addRule(const FirewallRule& rule);
  bool deleteRule(FirewallRule::Table::iterator& iter);
  FirewallRule::Table::iterator begin();
  FirewallRule::Table::iterator end();
  void tableLoop(TableLoopHandler& handler);

private:
  friend Firewall& instance();
  Firewall();
  ~Firewall();
  FirewallRule::Table _table;
};


//
// Inlines
//
inline FirewallRule::Table::iterator Firewall::begin()
{
  return _table.begin();
}

inline FirewallRule::Table::iterator Firewall::end()
{
  return _table.end();
}


} } // OSS::Net


#endif /* FIREWALL_H_ */
