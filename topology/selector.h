#ifndef SELECTOR_H
#define SELECTOR_H

/*
 *  selector.h
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

#include "nest.h"
#include "dictdatum.h"
#include "subnet.h"

/** @file selector.h
 *  Implements the slicing functionality.
 */

namespace nest{

  /**
   * The Selector class extracts nodes from a node or a subnet 
   * structure. The nodes are extracted based upon the entries set
   * in the layer slice dictionaries. Nodes can be extracted based
   * upon modeltype and subnet depth.
   */
  class Selector{
  public:
    /**
     * Create a Selector based upon an input dictionary.
     * @param dictionary with slicing information
     */
    Selector(const DictionaryDatum&);
    
    /**
     * @param Subnet where nodes extracted by slicing are stored
     * @param Layer subnet from which nodes are sliced
     */
    void slice_node(Subnet&, Subnet&);
    
  private:
    long_t modeltype_;  //!< model type to select, -1 means
  };

}

#endif

