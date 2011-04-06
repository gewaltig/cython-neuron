/*
 *  proxynode.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef PROXYNODE_H
#define PROXYNODE_H

#include "node.h"

namespace nest
{
  class SpikeEvent;
  class CurrentEvent;
  class Connection;

/* BeginDocumentation
Name: proxynode - Proxy to provide Nodes on remote machines
Description:
Remarks:
Parameters: 
References:
Author: June 2005, Jochen Martin Eppler
*/

  /**
   * Proxy Node to provide Nodes, where there aren't real Nodes to be
   */
  class proxynode: public Node
  {    
    
  public:        
    proxynode() : Node() {set(frozen);}
    
    /**
     * Import sets of overloaded virtual functions.
     * We need to explicitly include sets of overloaded
     * virtual functions into the current scope.
     * According to the SUN C++ FAQ, this is the correct
     * way of doing things, although all other compilers
     * happily live without.
     */
    using Node::connect_sender;
    using Node::handle;
    
    port check_connection(Connection&, port);
    void handle(SpikeEvent &) {}
    port connect_sender(SpikeEvent &) {return 0;}
    std::string get_name() const {return "proxynode";}

    void get_status(DictionaryDatum &) const {}
    
    /**
     * Proxy nodes have no properties.
     * If set_status() gets called for a proxy node, this is
     * and error; we must prevent this from happening, since the
     * user might otherwise thaw a proxy node. It also causes 
     * problems with dictionary entry checking
     */
     void set_status(const DictionaryDatum &) { assert(false); }

    bool is_proxy() const;
    
  private:
    void init_node_(const Node&) {}
    void init_state_(const Node&) {}
    void init_buffers_() {}
    void calibrate() {}
    void update(Time const &, const long_t, const long_t) {}

  };

  inline
  bool proxynode::is_proxy() const
  {
    return true;
  }

} // namespace

#endif //PROXYNODE_H
