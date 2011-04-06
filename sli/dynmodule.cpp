/*
 *  dynmodule.cpp
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

#include "dynmodule.h"
#include "interpret.h"

void DynModule::install(std::ostream &, SLIInterpreter *i, nest::Network *net)
{
  // Output stream for all messages are now decided by the message
  // level.
  //  i->message(out,5, name().c_str(), "Initializing.");
  i->message(5, name().c_str(), "Initializing.");
  init(i, net);
}

const std::string DynModule::commandstring(void) const
{
  return std::string();
}
