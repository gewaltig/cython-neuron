/*
 *  neststartup.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2010 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/* 
    This code will be used to startup NEST. It will be called only
    for starting NEST as a stand-alone application. Similar code is
    found in PyNEST.   
*/

#include "config.h"
#include <fstream>
#include "interpret.h"
#include "dict.h"
#include "dictdatum.h"
#include "random_numbers.h"
#include "gnureadline.h"
#include "slistartup.h"
#include "sliarray.h"
#include "oosupport.h"
#include "processes.h"
#include "nestmodule.h"
#include "sliregexp.h"
#include "specialfunctionsmodule.h"
#include "sligraphics.h"
#include "dynamicloader.h"
#include "filesystem.h"
#include "neststartup.h"

#include "static_modules.h"

int neststartup(int argc, char**argv, SLIInterpreter &engine, nest::Network* &pNet)
{

  // We disable synchronization between stdio and istd::ostreams
  // this has to be done before any in- or output has been done.
/*
 * TODO: This block looks to me as if it would evaluate to the same stuff
 *       in all cases. Can it be removed (or simplified, if I'm wrong ;-)
 */
#ifdef __GNUC__
#if  __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 1)
  // Broken with GCC 3.1 and higher. 
  // cin.get() never returns, or leaves cin in a broken state.
  std::ios::sync_with_stdio(false);
#endif
#else
  // This is for all other compilers
  std::ios::sync_with_stdio(false);
#endif

  addmodule<OOSupportModule>(engine);   
  addmodule<RandomNumbers>(engine);
#ifdef HAVE_READLINE
  addmodule<GNUReadline>(engine);
#endif
  addmodule<SLIArrayModule>(engine);
  addmodule<SpecialFunctionsModule>(engine);   // safe without GSL
  addmodule<SLIgraphics>(engine);
  engine.addmodule(new SLIStartup(argc,argv));
  addmodule<Processes>(engine);
  addmodule<RegexpModule>(engine);
  addmodule<FilesystemModule>(engine);

  // create the network and register with NestModule class
  pNet = new nest::Network(engine);
  assert(pNet != 0);
  nest::NestModule::register_network(*pNet);
  addmodule<nest::NestModule>(engine);

  // now add static modules providing models
  add_static_modules(engine, *pNet);

#ifdef HAVE_LIBLTDL  // no dynamic loading without LIBLTDL
  //dynamic loader module for managing linked and dynamically loaded extension modules
  nest::DynamicLoaderModule *pDynLoader = new nest::DynamicLoaderModule(pNet, engine);

  // initialize all modules that were linked into at compile time
  // these modules have registered via calling DynamicLoader::registerLinkedModule
  // from their constructor
  pDynLoader->initLinkedModules(engine);

  // interpreter will delete module on destruction
  engine.addmodule(pDynLoader);
#endif

  return engine.startup();
}
