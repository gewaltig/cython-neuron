#ifndef NTREE_IMPL_H
#define NTREE_IMPL_H

/*
 *  ntree_impl.h
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

#include "ntree.h"

namespace nest {

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::append_nodes_(std::vector<std::pair<Position<D>,T> >&v)
  {
    if (leaf_) {
      std::copy(nodes_.begin(), nodes_.end(), std::back_inserter(v));
    } else {
      for (int i=0;i<N;++i)
        children_[i]->append_nodes_(v);
    }
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::append_nodes_(std::vector<std::pair<Position<D>,T> >&v, const Mask<D> &mask, const Position<D> &anchor)
  {
    if (mask.outside(lower_left_-anchor, lower_left_-anchor+extent_))
      return;

    if (mask.inside(lower_left_-anchor, lower_left_-anchor+extent_))
      return append_nodes_(v);

    if (!leaf_) {
      for (int i=0;i<N;++i)
        children_[i]->append_nodes_(v,mask,anchor);
      return;
    }      

    for(typename std::vector<std::pair<Position<D>,T> >::iterator i=nodes_.begin();
        i!=nodes_.end(); ++i) {
      if (mask.inside(i->first - anchor))
        v.push_back(*i);
    }
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::split_()
  {
    assert(leaf_);

    // std::cout << "splitting..." << std::endl;

    for(int j=0;j<N;++j) {
      Position<D> ll = lower_left_;
      for(int i=0;i<D;++i) {
        if (j & (1<<i))
          ll[i] += extent_[i]*0.5;
      }
      // std::cout << "  child " << j << " has ll " << ll << std::endl;

      children_[j] = new Ntree<D,T,max_capacity>(ll, extent_*0.5,this,j);
    }

    for(typename std::vector<std::pair<Position<D>,T> >::iterator i=nodes_.begin(); i!=nodes_.end(); ++i) {
      // std::cout << "  putting node " << i->second << " at " << i->first << " into subtree " << subquad_(i->first) << std::endl;
      children_[subquad_(i->first)]->insert(i->first,i->second);
    }

    nodes_.clear();

    // std::cout << "splitting done." << std::endl;

    leaf_ = false;
  }

  template<int D, class T, int max_capacity>
  std::vector<T> Ntree<D,T,max_capacity>::get_nodes_only()
  {
    std::vector<T> result;
    for(iterator i=begin(); i != end(); ++i) {
      result.push_back(i->second);
    }

    return result;
  }

  template<int D, class T, int max_capacity>
  std::vector<T> Ntree<D,T,max_capacity>::get_nodes_only(const AbstractMask &mask, const std::vector<double_t> &anchor)
  {
    std::vector<std::pair<Position<D>,T> > result_pairs;
    append_nodes_(result_pairs,dynamic_cast<const Mask<D>&>(mask),Position<D>(anchor));

    std::vector<T> result;
    for(typename std::vector<std::pair<Position<D>,T> >::iterator i=result_pairs.begin(); i != result_pairs.end(); ++i) {
      result.push_back(i->second);
    }
    return result;
  }


}

#endif
