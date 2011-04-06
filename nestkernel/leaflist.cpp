/*
 *  leaflist.cpp
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

#include "leaflist.h"
#include "nodelist.h"

namespace nest{

  bool LeafList::empty() const
  {
    // A LeafList is empty, if all members of the NodeList are
    // not leaves.

    for (NodeList::iterator i=NodeList::begin(); i!=NodeList::end(); ++i)
      {
        if (is_leaf_(*i))
          return false;
      }
    return true;
  }

  size_t LeafList::size() const
  {
    // The size of the LeafList is the number of elements
    // in the NodeList which are leaves.
    
    size_t count=0;
    for (NodeList::iterator i=NodeList::begin(); i!=NodeList::end(); ++i)
      {
        if (is_leaf_(*i))
          ++count;
      }
    return count;
  }
  

  LeafList::iterator LeafList::begin() const
  { // we traverse the NodeList, and return the first element that is
    // a leaf, or end() if none exitsts.
    for (NodeList::iterator i=NodeList::begin(); i!=NodeList::end(); ++i)
      {
        if (is_leaf_(*i))
          return iterator(i, *this);
      }
    return end();
  }


  /** 
   * LeafList::iterator::operator++()
   * Operator++ advances the iterator to the right neighbor
   * in a post-order tree traversal, excluding the non-leaf
   * nodes.
   * Note that this is the standard NEST counting order for element
   * counting in multidimensional subnets.
   */
  LeafList::iterator LeafList::iterator::operator++()
  {
    /**
     * We can assume that this operator is not called on
     * end(). For this case, the result is undefined!
     */

    // we use NodeList::operator++ for this, and call it until it
    // returns an element that is a leaf, or end()
    
    NodeList::iterator i;

    do i=NodeList::iterator::operator++();
    while ( (i != the_container_.end()) && !LeafList::is_leaf_(*i) );
    
    return iterator(i, the_container_);
  }


}
	
	
	
