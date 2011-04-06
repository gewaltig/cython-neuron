#ifndef QUADTREE_H
#define QUADTREE_H

/*
 *  quadtree.h
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

#include "quadrant.h"

/** @file quadtree.h
 *  Implements the quadtree structure.
 */

namespace nest
{

  template <class DataType>
    class Position;

  class Quadrant;
  class Box;
  class NodeWrapper;

  /**
   * Class that implements a quadtree structure. A quadtree is a 
   * structure that helps in storing and retrieving data points 
   * distributed in a 2 dimensional space.
   *
   * A quadtree structure divides the space recursively into 4 
   * equal rectangular regions. Each node is placed into the
   * quadtree region that it overlaps. This region is further split into
   * 4 additional regions if the number of nodes in the region excedes 
   * a given limit.
   *
   * Once the quadtree is constructed the nodes covering a certain region 
   * in the full 2D space can easily be retrieved by traversing the 
   * correct tree branches. 
   *
   * For future implementations of the class: A quadtree that covers
   * a 3D space is called an oct-tree.
   * 
   */
  class Quadtree
  {

  public:
    /**
     * Empty default constructor.
     */
    Quadtree();

    /**
     * Constructs a quadtree based on a set of node pointer
     * and a corresponding set of positions.
     * @param nodes  Set of nodes that the tree will be based upon.
     * @param pos    Position of nodes.
     * @param ll     Lower left corner of quadtree space. Must be
     *               less than the position of all nodes.
     * @param ur     Upper right corner of quadtree space. Must be
     *               greater than any node positions.
     * @param quadrant_max maximum number of nodes allowed in each 
     *               quadtree leaf region. It is not necessary
     *               to adjust this parameter for most users. In a 
     *               few cases it might lead to improved performance
     *               of the quadtree structure if this variable is
     *               altered.
     */
    Quadtree(const std::vector<Node*>&, 
	     const std::vector<Position<double_t> >&,
	     const Position<double_t>& ll, 
	     const Position<double_t>& ur,
	     index quadrant_max_nodes);

    ~Quadtree();

    /**
     * Find the nodes in the quadtree quadrant covering the the input 
     * position.
     * @param pos  Position somewhere in the space covered by the tree
     * @returns 
     */
    std::vector<Node*> find(Position<double_t> pos);

    /**
     * Print a layout of the Quadtree structure.
     */
    void print();

    /**
     * Retrieve all quadtree leaves (quadrants) covering a given 
     * rectangular input region.
     * @param upper_left   Position of upper left corner of input region.
     * @param lower_right  Position of lower right corner of input region.
     * @returns vector of quadrant pointers covering the input region.
     */
    std::vector<Quadrant> 
      get_leaves(const Position<double_t>& upper_left, 
		 const Position<double_t>& lower_right) const;

    /**
     * Get nodes pointers and positions of nodes overlapping input 
     * region.
     * Note: "delete box;" is called upon the termination of this function.
     * The "box" object must because of this be dispensable.
     *
     * @param box    Region
     * @param extent Layer extent for periodic boundary conditions, or 0 otherwise
     * @returns vector of nodewrappers containing information about
     *          both node id and node position.
     */
    lockPTR<std::vector<NodeWrapper> >
      get_nodewrappers(Region const * const box, std::vector<double_t> * extent=0) const;

  private:

    /**
     * Inserts a new node in the Quadtree structure.
     * @param node  Node pointer
     * @param pos   Position of input node.
     */
    void insert(Node*, Position<double_t>);

    // Mother cell(quadrant) of the quadtree structure.
    lockPTR<Quadrant> root_;

  };

}

#endif
