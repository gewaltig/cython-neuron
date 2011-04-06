#ifndef OCTTREE_H
#define OCTTREE_H

/*
 *  octtree.h
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

#include <vector>

#include "nest.h"
#include "node.h"

#include "octant.h"
#include "quadtree.h"

#include "region.h"

namespace nest
{

  template <class DataType>
    class Position;

  class Octant;
  class NodeWrapper;

  /**
   *
   */
  class Octtree
  {

  public:
    /**
     * Empty default constructor.
     */
    Octtree();
    /**
     *
     */
    Octtree(const std::vector<Node*>&, 
	    const std::vector<Position<double_t> >&,
	    const Position<double_t>& ll, 
	    const Position<double_t>& ur,
	    index octant_max_nodes);

    ~Octtree();

    /**
     * Print a layout of the Octtree structure.
     */
    void print();

    /**
     * Retrieve all octtree leaves (quadboxes) covering a given 
     * rectangular input region.
     * @param lower_left   Position of upper left corner of input region.
     * @param upper_right  Position of lower right corner of input region.
     * @returns vector of quadbox pointers covering the input region.
     */
    std::list<Octant> 
      get_leaves(const Position<double_t>& upper_left, 
		 const Position<double_t>& lower_right) const;

    /**
     * Note: "delete box;" is called upon the termination of this function
     *
     * @param lower_left 
     * @param upper_right
     * @param shift  
     * @param extent Layer extent for periodic boundary conditions, or 0 otherwise
     * @returns vector of nodewrappers containing information about
     *          both node id and node position.
     */
    lockPTR<std::vector<NodeWrapper> >
      get_nodewrappers(Volume const * const box, std::vector<double_t> * extent=0) const;

    /**
     * Retrieve a set of nodes covering a circular region defined by
     * input center and radius. TODO: Calculate input parameters 
     * lower_left and upper_right automatically based upon input 
     * center and radius.
     * @param lower_left  
     * @param upper_right
     * @param center  position of center of circular region
     * @param radius  radius of circular region
     */

    std::vector<Node*> 
      get_nodes(Position<double_t> lower_left, 
		Position<double_t> upper_right) const;

      
  private:
    /**
     * Inserts a new node in the Octtree structure.
     * @param node  Node pointer
     * @param pos   Position of input node.
     */
    void insert(Node*, Position<double_t>);

    // Mother cell(octant) of the octtree structure.
    Octant* root_;

  };

}

#endif
