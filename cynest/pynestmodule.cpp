/*
 *  pynestmodule.cpp
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

/* 
    This file is part of NEST

    pynestmodule.cpp -- module providing models depending on Python

    Author(s): 
    Hans Ekkehard Plesser, 
    based on work by M.O.Gewaltig, E. Mueller and M. Helias

    First Version: January 2009
*/

#include "config.h"
#include "pynestmodule.h"
#include "network.h"
#include "model.h"
#include "genericmodel.h"
#include <string>

// Node headers


namespace nest
{

  SLIType PynestModule::Pyobjecttype;

  /* At the time when PynestModule is constructed, the SLI Interpreter
     must already be initialized. PynestModule relies on the presence of
     the following SLI datastructures: Name, Dictionary
  */

  PynestModule::PynestModule(Network& net) :
    net_(net)
   {
    Pyobjecttype.settypename("pyobjecttype");
    Pyobjecttype.setdefaultaction(SLIInterpreter::datatypefunction);
   }

   PynestModule::~PynestModule()
   {
     Pyobjecttype.deletetypename();
   }

   const std::string PynestModule::name(void) const
   {
     return std::string("NEST Python-dependent Models Module"); // Return name of the module
   }

   const std::string PynestModule::commandstring(void) const
   {
     return std::string(""); // Run associated SLI startup script
   }

  //-------------------------------------------------------------------------------------

  void PynestModule::init(SLIInterpreter *)
  {
    // register models
  }  // PynestModule::init()


} // namespace nest

