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
#include "mask.h"

namespace nest {

  template<int D, class T, int max_capacity>
  Ntree<D,T,max_capacity>::iterator::iterator(Ntree& q):
    ntree_(&q), top_(&q), node_(0)
  {
    // First leaf
    while(!ntree_->is_leaf())
      ntree_ = ntree_->children_[0];

    // Find the first non-empty leaf
    while(ntree_->nodes_.size() == 0) {

      next_leaf_();
      if (ntree_ == 0) break;

    }
  }

  template<int D, class T, int max_capacity>
  typename Ntree<D,T,max_capacity>::iterator & Ntree<D,T,max_capacity>::iterator::operator++()
  {
    node_++;

    while (node_ >= ntree_->nodes_.size()) {

      next_leaf_();

      node_ = 0;

      if (ntree_ == 0) break;

    }

    return *this;
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::iterator::next_leaf_()
  {

    // If we are on the last subntree, move up
    while(ntree_ && (ntree_ != top_) && (ntree_->my_subquad_ == N-1)) {
      ntree_ = ntree_->parent_;
    }

    // Since we stop at the top, this should never happen!
    assert(ntree_ != 0);

    // If we have reached the top, mark as invalid and return
    if (ntree_ == top_) {
      ntree_ = 0;
      return;
    }

    // Move to next sibling
    ntree_ = ntree_->parent_->children_[ntree_->my_subquad_+1];

    // Move down if this is not a leaf.
    while(!ntree_->is_leaf())
      ntree_ = ntree_->children_[0];

  }


  template<int D, class T, int max_capacity>
  Ntree<D,T,max_capacity>::masked_iterator::masked_iterator(Ntree<D,T,max_capacity>& q, const Mask<D> &mask, const Position<D> &anchor):
    ntree_(&q), top_(&q), allin_top_(0), node_(0), mask_(&mask), anchor_(anchor), anchors_(), current_anchor_(0)
  {
    if (ntree_->periodic_.any()) {
      // Add one image of anchor for each periodic dimension
      // This assumes that the mask never extends beyond more than one half layer
      // from the anchor in any direction.

      for(int i=0;i<D;++i) {
        if (ntree_->periodic_[i]) {
          anchor_[i] = ntree_->lower_left_[i] + std::fmod(anchor_[i]-ntree_->lower_left_[i], ntree_->extent_[i]);
          if (anchor_[i]<ntree_->lower_left_[i])
            anchor_[i] += ntree_->extent_[i];
        }
      }
      anchors_.push_back(anchor_);

      for(int i=0;i<D;++i) {
        if (ntree_->periodic_[i]) {
          int n = anchors_.size();
          if ((anchor_[i]-ntree_->lower_left_[i]) > 0.5*ntree_->extent_[i]) {
            for(int j=0;j<n;++j) {
              Position<D> p = anchors_[j];
              p[i] -= ntree_->extent_[i];
              anchors_.push_back(p);
            }
          } else {
            for(int j=0;j<n;++j) {
              Position<D> p = anchors_[j];
              p[i] += ntree_->extent_[i];
              anchors_.push_back(p);
            }
          }
        }
      }
    }

    init_();
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::masked_iterator::init_()
  {
    node_ = 0;
    allin_top_ = 0;
    ntree_ = top_;

    if (mask_->outside(ntree_->lower_left_-anchor_,ntree_->lower_left_-anchor_+ntree_->extent_)) {

      next_anchor_();

    } else {

      if (mask_->inside(ntree_->lower_left_-anchor_, ntree_->lower_left_-anchor_+ntree_->extent_)) {
        first_leaf_inside_();
      } else {
        first_leaf_();
      }

      if ((ntree_->nodes_.size() == 0) || (!mask_->inside(ntree_->nodes_[node_].first-anchor_))) {
        ++(*this);
      }
    }
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::masked_iterator::next_anchor_()
  {
    ++current_anchor_;
    if (current_anchor_ >= anchors_.size()) {
      // Done. Mark as invalid.
      ntree_ = 0;
      node_ = 0;
    } else {
      anchor_ = anchors_[current_anchor_];
      init_();
    }
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::masked_iterator::next_leaf_()
  {

    // There are two states: the initial state, and "all in". In the
    // all in state, we are in a subtree which is completely inside
    // the mask. The allin_top_ is the top of this subtree. When
    // exiting the subtree, the state changes to the initial
    // state. In the initial state, we must check each quadrant to
    // see if it is completely inside or outside the mask. If inside,
    // we go all in. If outside, we move on to the next leaf. If
    // neither, keep going until we find a leaf. Upon exiting from
    // this function, we are either done (ntree_==0), or on a leaf
    // node which at least intersects with the mask. If allin_top_!=0,
    // the leaf is completely inside the mask.

    if (allin_top_) {
      // state: all in

      // If we are on the last subtree, move up
      while(ntree_ && (ntree_ != allin_top_) && (ntree_->my_subquad_ == N-1)) {
        ntree_ = ntree_->parent_;
      }

      // Since we stop at the top, this should never happen!
      assert(ntree_ != 0);

      // If we reached the allin_top_, we are no longer all in.
      if (ntree_ != allin_top_) {

        // Move to next sibling
        ntree_ = ntree_->parent_->children_[ntree_->my_subquad_+1];

        // Move down if this is not a leaf.
        while(!ntree_->is_leaf())
          ntree_ = ntree_->children_[0];

        return;

      }

      allin_top_ = 0;
      // Will continue as not all in.

    }

    // state: Not all in

    do {

      // If we are on the last subtree, move up
      while(ntree_ && (ntree_ != top_) && (ntree_->my_subquad_ == N-1)) {
        ntree_ = ntree_->parent_;
      }

      // Since we stop at the top, this should never happen!
      assert(ntree_ != 0);

      // If we have reached the top, mark as invalid and return
      if (ntree_ == top_) {
        return next_anchor_();
      }

      // Move to next sibling
      ntree_ = ntree_->parent_->children_[ntree_->my_subquad_+1];

      if (mask_->inside(ntree_->lower_left_-anchor_, ntree_->lower_left_-anchor_+ntree_->extent_)) {
        return first_leaf_inside_();
      }

    } while (mask_->outside(ntree_->lower_left_-anchor_, ntree_->lower_left_-anchor_+ntree_->extent_));

    return first_leaf_();
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::masked_iterator::first_leaf_()
  {
    while(!ntree_->is_leaf()) {

      ntree_ = ntree_->children_[0];

      if (mask_->inside(ntree_->lower_left_-anchor_, ntree_->lower_left_-anchor_+ntree_->extent_)) {
        return first_leaf_inside_();
      }

      if (mask_->outside(ntree_->lower_left_-anchor_,ntree_->lower_left_-anchor_+ntree_->extent_)) {
        return next_leaf_();
      }

    }
  }


  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::masked_iterator::first_leaf_inside_()
  {

    allin_top_ = ntree_;

    while(!ntree_->is_leaf()) {
      ntree_ = ntree_->children_[0];
    }
  }

  template<int D, class T, int max_capacity>
  typename Ntree<D,T,max_capacity>::masked_iterator & Ntree<D,T,max_capacity>::masked_iterator::operator++()
  {
    node_++;

    if (allin_top_ == 0) {
      while((node_ < ntree_->nodes_.size()) && (!mask_->inside(ntree_->nodes_[node_].first-anchor_))) {
        node_++;
      }
    }

    while (node_ >= ntree_->nodes_.size()) {

      next_leaf_();

      node_ = 0;

      if (ntree_ == 0) break;

      if (allin_top_ == 0) {
        while((node_ < ntree_->nodes_.size()) && (!mask_->inside(ntree_->nodes_[node_].first-anchor_))) {
          node_++;
        }
      }
    }

    return *this;
  }

  template<int D, class T, int max_capacity>
  int Ntree<D,T,max_capacity>::subquad_(const Position<D>& pos)
  {
    Position<D> offset = pos - lower_left_;

    offset /= extent_/2;

    int r = 0;
    for(int i=0;i<D;++i)
      r += (1<<i) * int(offset[i]);

    return r;
  }

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

    if (leaf_) {

      for(typename std::vector<std::pair<Position<D>,T> >::iterator i=nodes_.begin();
          i!=nodes_.end(); ++i) {
        if (mask.inside(i->first - anchor))
          v.push_back(*i);
      }

    } else {

      for (int i=0;i<N;++i)
        children_[i]->append_nodes_(v,mask,anchor);

    }
  }

  template<int D, class T, int max_capacity>
  typename Ntree<D,T,max_capacity>::iterator Ntree<D,T,max_capacity>::insert(const Position<D>& pos, const T& node)
  {
    if (leaf_ && (nodes_.size()>=max_capacity))
      split_();

    if (leaf_) {

      assert((pos >= lower_left_) && (pos < lower_left_ + extent_));

      nodes_.push_back(std::pair<Position<D>,T>(pos,node));

      return iterator(*this,nodes_.size()-1);

    } else {

      return children_[subquad_(pos)]->insert(pos,node);

    }
  }

  template<int D, class T, int max_capacity>
  void Ntree<D,T,max_capacity>::split_()
  {
    assert(leaf_);

    for(int j=0;j<N;++j) {
      Position<D> ll = lower_left_;
      for(int i=0;i<D;++i) {
        if (j & (1<<i))
          ll[i] += extent_[i]*0.5;
      }

      children_[j] = new Ntree<D,T,max_capacity>(ll, extent_*0.5,0,this,j);
    }

    for(typename std::vector<std::pair<Position<D>,T> >::iterator i=nodes_.begin(); i!=nodes_.end(); ++i) {
      children_[subquad_(i->first)]->insert(i->first,i->second);
    }

    nodes_.clear();

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
