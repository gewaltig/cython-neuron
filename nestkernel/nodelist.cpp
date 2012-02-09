/*
 *  nodelist.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "nodelist.h"

/*
 * Iteration logic
 *
 * begin()
 * Descends recursively, until it reaches a subnet in which
 * the first entry is not a subnet. This is the first node and
 * an iterator to it is returned.
 *
 * operator++()
 * Proceeds to the right neighbor in the vector<Node*> of the
 * Subnet. If there is no right neighbor, it backtracks up.
 *
 * end()
 * By post-order traversal, the last item in the NodeList is the
 * last child of the subnet wrapped. Thus subnet_.local_end() is the
 * LocalNodeList::end().
 *
 */

namespace nest
{

  LocalNodeList::iterator LocalNodeList::begin() const
  {
    if ( empty() )
      return end();

    Subnet *current_subnet = &subnet_;  // start at wrapped subnet
    vector<Node*>::iterator node;       // node we are looking at

    do {
      assert(not current_subnet->local_empty());
      node = current_subnet->local_begin();  // leftmost in current subnet
      current_subnet = dynamic_cast<Subnet*>(*node); // attempt descend
    } while ( current_subnet && not current_subnet->local_empty() );

    // Either node is a non-subnet node or and empty subnet, this is
    // the first node.
    return iterator(node, subnet_.local_end());
  }

  LocalNodeList::iterator LocalNodeList::end() const
  {
    return iterator(subnet_.local_end(), subnet_.local_end());
  }

  /** 
   * NodeList::iterator::operator++()
   * Operator++ advances the iterator to the right neighbor
   * in a post-order tree traversal, including the non-leaf
   * nodes.
   *
   * The formulation is iterative. Maybe a recursive 
   * formulation would be faster, however, we will also
   * supply a chached-iterator, which does this work only once.
   */
    
  LocalNodeList::iterator LocalNodeList::iterator::operator++()
  {
    if ( current_node_ == list_end_ )  // we are at end
      return *this;

    // Obtain a pointer to the subnet to which the current node
    // belongs. We need it to check if we have reached the end
    // of that subnet.
    Subnet *current_subnet = (*current_node_)->get_parent();
    assert(current_subnet != NULL);

    /**
     * 1. Find the right neighbor
     * 2.   Traverse the left-most branch
     * 3.   return leaf of leftmost branch
     * 4. If no right neigbor exists, go up one level
     * 5.   return element.
     * 6. If we cannot go up, return end() of local subnet
     */

    ++current_node_; // go to right neighbor of current node

    if ( current_node_ != current_subnet->local_end() )
    {
      // We have a right neighbor.
      current_subnet = dynamic_cast<Subnet *>(*current_node_);
      while( current_subnet && not current_subnet->local_empty() )
      {
        // current_node_ is a subnet and we descend into it
        current_node_ = current_subnet->local_begin();
        current_subnet = dynamic_cast<Subnet *>(*current_node_);
      }
      // current_node_ is either not a subnet or an empty subnet,
      // so we have found the proper right neigbor.
      return *this;
    }
    else if ( current_node_ == list_end_ )
      return *this;   // we are at end of subnet
    
    // current_node_ is not a valid right neighbor, nor the end of
    // the subnet, so we need to move up
    Subnet* parent = current_subnet->get_parent();
    assert(parent);
    
    // We now need to find the iterator to the parent, within the node
    // vector of the parent's parent.
    Subnet* grandparent = parent->get_parent();
    assert(grandparent);
    current_node_ = grandparent->local_begin() + parent->get_subnet_index();
    assert(*current_node_ == parent);  // make sure we got iterator to correct node

    return *this;
  }

}
	
	
	
