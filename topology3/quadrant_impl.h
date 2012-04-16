#ifndef QUADRANT_IMPL_H
#define QUADRANT_IMPL_H

/*
 *  quadrant_impl.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "quadrant.h"

namespace nest {

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::append_nodes_(std::vector<std::pair<Position<2>,T> >&v)
  {
    if (leaf_) {
      std::copy(nodes_.begin(), nodes_.end(), std::back_inserter(v));
    } else {
      for (int i=0;i<4;++i)
        children_[i]->append_nodes_(v);
    }
  }

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::append_nodes_(std::vector<std::pair<Position<2>,T> >&v, const Mask<2> &mask)
  {
    if (mask.outside(lower_left_, lower_left_+extent_))
      return;

    if (mask.inside(lower_left_, lower_left_+extent_))
      return append_nodes_(v);

    if (!leaf_) {
      for (int i=0;i<4;++i)
        children_[i]->append_nodes_(v,mask);
      return;
    }      

    for(typename std::vector<std::pair<Position<2>,T> >::iterator i=nodes_.begin();
        i!=nodes_.end(); ++i) {
      if (mask.inside(i->first))
        v.push_back(*i);
    }
  }

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::split_()
  {
    assert(leaf_);

    children_[0] = new Quadrant<T,max_capacity>(lower_left_, extent_*0.5,this,0);
    children_[1] = new Quadrant<T,max_capacity>(lower_left_ + extent_*Position<2>(0.5,0.0), extent_*0.5,this,1);
    children_[2] = new Quadrant<T,max_capacity>(lower_left_ + extent_*Position<2>(0.0,0.5), extent_*0.5,this,2);
    children_[3] = new Quadrant<T,max_capacity>(lower_left_ + extent_*0.5, extent_*0.5,this,3);

    for(typename std::vector<std::pair<Position<2>,T> >::iterator i=nodes_.begin(); i!=nodes_.end(); ++i) {
      children_[subquad_(i->first)]->insert(i->first,i->second);
    }

    nodes_.clear();

    leaf_ = false;
  }


}

#endif
