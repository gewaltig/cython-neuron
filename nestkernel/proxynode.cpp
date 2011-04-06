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

port proxynode::check_connection(Connection& c, port receptor_type)
{
  return network()->get_model(get_model_id())->check_connection(c, receptor_type);
}

} // namespace
