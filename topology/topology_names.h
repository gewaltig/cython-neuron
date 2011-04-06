#ifndef TOPOLOGY_NAMES_H
#define TOPOLOGY_NAMES_H

/*
 *  topology_names.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2008 by
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
    This file is part of the NEST topology module.
    Author: Kittel Austvoll

*/

#include "name.h"

namespace nest
{
  /**
   * This namespace contains Name objects that are used by the topology
   * module. See nest_names.h for more info.
   */
  namespace names
  {
    extern const Name rows;    //!< Number of rows in a layer or mask
    extern const Name columns;
    extern const Name row;
    extern const Name column;
    extern const Name depth;
    extern const Name extent;
    extern const Name center;
    extern const Name edge_wrap;
    extern const Name anchor;
    extern const Name x;
    extern const Name y;
    extern const Name positions;
    extern const Name topology;
    extern const Name points;
    extern const Name sources;
    extern const Name mask;
    extern const Name lid;
    extern const Name elements;
    extern const Name allow_oversized_mask;
  }
}

#endif
