/*
 *  proxynode.cpp
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


#include "network.h"
#include "dictutils.h"
#include "proxynode.h"
#include "connection.h"

namespace nest
{

proxynode::proxynode(index gid, index parent_gid, index model_id) :
    Node()
{
  set_gid_(gid);
  Subnet* parent = dynamic_cast<Subnet*>(network()->get_node(parent_gid));
  assert(parent);
  set_parent_(parent);
  this->set_model_id(model_id);
  set(frozen);
}

port proxynode::check_connection(Connection& c, port receptor_type)
{
  return network()->get_model(get_model_id())->check_connection(c, receptor_type);
}

} // namespace
