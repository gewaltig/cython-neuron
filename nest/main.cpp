/*
 *  main.cpp
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

#include <config.h>
#include "neststartup.h"
#include "communicator.h"
#include "interpret.h"
#include "booldatum.h"
#include "dictdatum.h"
#include "network.h"

int main(int argc, char *argv[])
{
#ifdef HAVE_MPI
  nest::Communicator::init(&argc, &argv);
#endif

  nest::Network *pNet = 0;

  /** 
   * Create the interpreter object. Due to its dependence
   * on various static objects (e.g. of class Name), the
   * interpreter engine MUST NOT be global.
   */
  SLIInterpreter engine;

  neststartup(argc, argv, engine, pNet);

  // start the interpreter session
  int exitcode = engine.execute();

#ifdef HAVE_MPI
  nest::Communicator::finalize();
#endif

  // delete the Network before modules are deleted by interpreter's destructor
  // because otherwise models defined in a module might still be referenced by
  // the Network
  delete pNet;

  return exitcode;
}
