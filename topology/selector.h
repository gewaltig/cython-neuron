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
#include "compound.h"

/** @file selector.h
 *  Implements the slicing functionality.
 */

namespace nest{

  /**
   * The Selector class extracts nodes from a node or a compound 
   * structure. The nodes are extracted based upon the entries set
   * in the layer slice dictionaries. Nodes can be extracted based
   * upon modeltype and subnet depth.
   */
  class Selector{
  public:
    /**
     * Create an empty Selector.
     */
    Selector();

    /**
     * Create a Selector based upon an input dictionary.
     * @param dictionary with slicing information
     */
    Selector(const DictionaryDatum&);
    
    /**
     * @param subnet Compound where nodes extracted by slicing are stored
     * @param node   Layer node that will be sliced
     */
    void slice_node(Compound& subnet, Node* node);
    
  private:
    // Class variables. slice_depth_ of 0 indicates that all depths
    // should be connected to. modeltype_ of -1 indicates that all
    // modeltypes should be connected.
    index slice_depth_;
    long_t modeltype_;
    
    /**
     * Recursive function called by slice_node(Compound&, Node*).
     *
     * @param subnet Compound where nodes extracted by slicing are stored
     * @param node   Node that will be sliced
     * @param slice_depth 
     */
    void slice_node(Compound& subnet, Node* node,
		    index slice_depth);

  };

}

#endif

