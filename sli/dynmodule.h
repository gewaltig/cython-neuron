/*
 *  dynmodule.h
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

#ifndef DYNMODULE_H
#define DYNMODULE_H

#include <iostream>
#include <string>
#include "sliexceptions.h"

class SLIInterpreter;

namespace nest {
  class Network;
}

/**
 * Base class for dynamically loadable SLI interpreter modules.
 */
class DynModule
{
  public:
  virtual ~DynModule(){};

  /**
   * Initialise the module.
   * When this function is called, most of the
   * interpreter's fascilities are up and running.
   * However, depending on where in the interpreter's 
   * bootstrap sequence the module is initialised, not 
   * all services may be available.
   */
  virtual void init(SLIInterpreter *, nest::Network *) = 0;

  /**
   * Unregister the symbols defined in module.
   */
  virtual void unregister(SLIInterpreter *, nest::Network *)
  { 
    throw DynamicModuleManagementError();
  }

  /**
   * Return name of the module.
   */
  virtual const std::string name(void) const=0;
  
  /**
   * Return sli command sequence to be executed for initialisation.
   */
  virtual const std::string commandstring(void) const;

  /**
   * Print installation message via interpreter message command.
   */
  void install(std::ostream &, SLIInterpreter *, nest::Network *);

};

#endif
