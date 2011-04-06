/*
 *  modelsmodule.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2006 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef MODELSMODULE_H
#define MODELSMODULE_H
/* 
    This file is part of NEST

    modelmodule.h -- Header to the modelmodule
    (see cpp file for details)


    Author: Marc-Oliver Gewaltig (marc-oliver.gewaltig@honda-ri.de)
            Hans Ekkehard Plesser (hans.ekkehard.plesser@umb.no)

    $Date$
    Last change: $Author$
    $Revision$
*/

#include "slimodule.h"

namespace nest
{
  class Network;
  
  /**
   * Module supplying all models that are included in the NEST release. 
   * @todo Should this be a dynamic module?
   */
   class ModelsModule: public SLIModule
   {
    public:

     ModelsModule(Network&);
     ~ModelsModule();

     /**
      * Initialize module by registering models with the network.
      * @param SLIInterpreter* SLI interpreter
      * @param nest::Network&  Network with which to register models
      */
     void init(SLIInterpreter*);

     const std::string name(void) const;
     const std::string commandstring(void) const;

   private:
     
     //! network where models are to be registered
     Network& net_;
   };


} // namespace

#endif
