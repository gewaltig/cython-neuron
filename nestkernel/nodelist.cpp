/*
 *  nodelist.cpp
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

#include "nodelist.h"

namespace nest{

  LocalNodeList::iterator LocalNodeList::begin() const
  {
    if (empty())
      return end();

    Subnet *r=root_;
    vector<Node*>::iterator n;

    while( r != NULL && !r->local_empty() )
    {
      n = r->local_begin(); //!< Move down in tree
      if ( (r=dynamic_cast<Subnet*>(*n)) == NULL )
	     break;
    }
    /** We have reached the end of tree */
    return iterator(n);
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
    /**
     * We must assume that this operator is not called on end(). For
     * this case, the result is undefined!
     */

    /**
     * This subnet is the container to which e belongs! If c yields
     * NULL, the tree is ill-formed!
     */
    Subnet *c=(*p_)->get_parent();
    assert(c != NULL);

    /**
     * 1. Find the right neighbor
     * 2.   Traverse the left-most branch
     * 3.   return leaf of leftmost branch
     * 4. If no right neigbor exists, go up one level
     * 5.   return element.
     * 6. If we cannot go up, return end() of local subnet
     */

    /** Goto right neighbor */
    ++p_;
    
    if(p_ != c->local_end())
    {
      Subnet *r=dynamic_cast<Subnet *>(*p_);

      while(r != NULL && ! r->local_empty())
      {
	p_=r->local_begin();
	r=dynamic_cast<Subnet *>(*p_);
      }
      
      return *this;
    }
    
    
    /** This is the case where no right neighbor exists.
     * We have to go up and return the parent
     */
    Subnet *p=c->get_parent();
    if(p==NULL)
    {
      /** We are already at the root container and
       * there is no right neighbor. Thus, we
       * have reached the end.
       */
      p_=c->local_end();
      return *this;
    }
    /**
     * We are at the end of a local container
     * thus, we have to ascend, so that we can proceed
     * with its right neigbor in the next round
     */

    /** 
     * Compute the iterator which points to c
     */
    p_= p->local_begin()+c->get_lid();
    return *this;
  }

  void LocalNodeList::set_root(Subnet &r)
  {
    root_=&r;
  }

  Subnet & LocalNodeList::get_root() const
  {
    assert(root_ != NULL);
    return *root_;
  }
}
	
	
	
