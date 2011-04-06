/*
 *  connection.cpp
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

#include "common_synapse_properties.h"
#include "connection.h"
#include "connector_model.h"
#include "network.h"

namespace nest
{

Connection::Connection()
        : target_(0),
          rport_(0)
{}

Connection::Connection(const Connection& c)
        : target_(c.target_),
          rport_(c.rport_)
{}

void Connection::get_status(DictionaryDatum & d) const
{
  if (target_ != 0)
  {
    def<long>(d, names::rport, rport_);
    def<long>(d, names::target, target_->get_gid());
  }
}


void Connection::initialize_property_arrays(DictionaryDatum & d) const
{
  initialize_property_array(d, names::targets);
  initialize_property_array(d, names::rports);
}

void Connection::append_properties(DictionaryDatum & d) const
{
  append_property<index>(d, names::targets, target_->get_gid());
  append_property<long_t>(d, names::rports, rport_);
}

} // namespace nest
