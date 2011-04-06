/*
 *  connection_id.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "connection_id.h"

namespace nest
{

  ConnectionID::ConnectionID(long source_gid, long target_thread, long synapse_typeid, long port)
          : source_gid_(source_gid),
            target_thread_(target_thread),
            synapse_typeid_(synapse_typeid),
            port_(port)
  {}
  
  DictionaryDatum ConnectionID::get_dict()
  {
    DictionaryDatum dict(new Dictionary);

    def<long>(dict, nest::names::source, source_gid_);             // The gid of the presynaptic node
    def<long>(dict, nest::names::synapse_typeid, synapse_typeid_); // The id of the synapse model
    def<long>(dict, nest::names::target_thread, target_thread_);   // The thread of the postsynaptic node
    def<long>(dict, nest::names::port, port_);                     // The index in the list 

    return dict;
  }

  bool ConnectionID::operator==(const ConnectionID& c)
  {
    return (source_gid_ == c.source_gid_)
        && (target_thread_ == c.target_thread_)
        && (port_ == c.port_)
        && (synapse_typeid_ == c.synapse_typeid_);
  }

  std::ostream & ConnectionID::print_me(std::ostream& out) const
  {
    out << "<connectiontype>";
    return out;
  }

std::ostream & operator<<(std::ostream& out, const nest::ConnectionID& c)
{
  return c.print_me(out);
}  

} // namespace


