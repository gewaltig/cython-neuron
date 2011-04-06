/*
 *  slimodule.cc
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

#include "slimodule.h"
#include "interpret.h"

void SLIModule::install(std::ostream &, SLIInterpreter *i)
{
  // Output stream is now set by the message level.
  // i->message(out, 5, name().c_str(), "Initializing.");
  i->message(5, name().c_str(), "Initializing.");
  init(i);
}

const std::string SLIModule::commandstring(void) const
{
  return std::string();
}

