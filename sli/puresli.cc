/*
 *  puresli.cc
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

/* 
    SLI main
*/
#include "config.h"
#include "interpret.h"
#include "gnureadline.h"
#include "slistartup.h"
#include "sliarray.h"
#include "oosupport.h"
#include "processes.h"
#include "sliregexp.h"
#include "integerdatum.h"
#include "tokenutils.h"
#include "filesystem.h"

 int main(int argc, char *argv[])
 {
  /**
    We disable synchronization between stdio and istd::ostreams
    this has to be done before any in- or output has been
    done.
  */

#ifdef __GNUC__
#if  __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 1)
  // Broken with GCC 3.1 and higher. 
  // cin.get() never returns, or leaves cin in a broken state.
  //
  std::ios::sync_with_stdio(false);
#endif
#else
  // This is for all other compilers
  std::ios::sync_with_stdio(false);
#endif


// Create the interpreter object. Due to its dependence
// on various static objects (e.g. of class Name), the
// interpreter engine MUST NOT be global.


   SLIInterpreter engine;
#ifdef HAVE_READLINE
   addmodule<GNUReadline>(engine);
#endif 
   addmodule<SLIArrayModule>(engine);
   addmodule<OOSupportModule>(engine);
   engine.addmodule(new SLIStartup(argc,argv));
   addmodule<Processes>(engine);
   addmodule<RegexpModule>(engine);
   addmodule<FilesystemModule>(engine);
   int exitcode = engine.execute();
   
   return exitcode;
 }
