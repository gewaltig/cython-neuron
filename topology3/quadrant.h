#ifndef QUADRANT_H
#define QUADRANT_H

/*
 *  quadrant.h
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

#include <vector>
#include <utility>
#include "position.h"

namespace nest
{

  template<int D>
  class Mask;

  /**
   * A Quadrant object represents a node (branch or leaf) in a Quadtree
   * structure. Any quadrant covers a specific region in a 2D space. A leaf
   * quadrant contains a list of items and their corresponding positions. A
   * branch quadrant contains a list of four other quadrants, each covering
   * a region corresponding to the upper-left, lower-left, upper-right and
   * lower-left corner of their mother quadrant.
   *
   */
  template<class T, int max_capacity=100>
  class Quadrant
  {
  public:

    class iterator;

    /**
     * Helper class which points to a leaf quadrant
     */
    class quadrant_iterator {
    public:
      /**
       * Constructor for invalid iterator.
       */
      quadrant_iterator() : quadrant_(0) {}

      /**
       * Initialize the iterator to point to the first leaf quadrant below
       * the given quadrant.
       */
      quadrant_iterator(Quadrant& q) : quadrant_(&q)
        {
          while(!quadrant_->is_leaf())
            quadrant_ = quadrant_->children_[0];
        }

      bool valid() { return quadrant_ != 0; }

      quadrant_iterator& operator++()
        {

          // If we are on the last subquadrant, move up
          while(quadrant_ && (quadrant_->my_subquad_ == 3)) {
            quadrant_ = quadrant_->parent_;
          }

          // If we reached the end, return
          if (quadrant_ == 0)
            return *this;

          // If we have reached the top, mark as invalid and return
          if (quadrant_->parent_ == 0) {
            quadrant_ = 0;
            return *this;
          }

          // Move to next sibling
          quadrant_ = quadrant_->parent_->children_[quadrant_->my_subquad_+1];

          // Move down if this is not a leaf.
          while(!quadrant_->is_leaf())
            quadrant_ = quadrant_->children_[0];

          return *this;
        }

      Quadrant& operator*()
        { return *quadrant_; }

      Quadrant* operator->()
        { return quadrant_; }

      bool operator==(const quadrant_iterator &other) const
        { return quadrant_ == other.quadrant_; }

      bool operator!=(const quadrant_iterator &other) const
        { return quadrant_ != other.quadrant_; }

      friend class iterator;

    protected:

      Quadrant *quadrant_;

    };

    /**
     * Iterator iterating the nodes in a Quadtree. The iterator will
     * traverse the tree until it finds leaf Quadrants.
     */
    class iterator {
    public:
      /**
       * Initialize an invalid iterator.
       */
      iterator() : quadrant_(), node_(0) {}

      /**
       * Initialize an iterator to point to the first leaf node within the
       * tree below this Quadrant.
       */
      iterator(Quadrant& q) : quadrant_(q), node_(0) {}

      std::pair<Position<2>,T> & operator*() { return quadrant_->nodes_[node_]; }
      std::pair<Position<2>,T> * operator->() { return &quadrant_->nodes_[node_]; }

      /**
       * Move the iterator to the next node within the tree.
       */
      iterator & operator++()
        {
          node_++;

          while (node_ >= quadrant_->nodes_.size()) {

            ++quadrant_;

            node_ = 0;

            if (!quadrant_.valid()) break;

          }

          return *this;
        }

      /**
       * Iterators are equal if they point to the same node in the same
       * quadrant.
       */
      bool operator==(const iterator &other) const
        { return (other.quadrant_==quadrant_) && (other.node_==node_); }
      bool operator!=(const iterator &other) const
        { return (other.quadrant_!=quadrant_) || (other.node_!=node_); }

    protected:

      quadrant_iterator quadrant_;
      index node_;
    };

    /**
     * Create a Quadrant that covers the region defined by the two
     * input positions.
     * @param lower_left  Lower left corner of quadrant.
     * @param extent      Size (width,height) of quadrant.
     */
    Quadrant(const Position<2>& lower_left,
	     const Position<2>& extent, Quadrant *parent, int subquad);

    /**
     * Traverse quadtree structure from current quadrant.
     * Inserts node in correct leaf in quadtree.
     */
    void insert(const Position<2>& pos, const T& node);

    /**
     * @returns member nodes in quadrant.
     */
    std::vector<std::pair<Position<2>,T> > get_nodes() const;

    /**
     * Applies a Mask to this quadrant.
     * @returns member nodes in quadrant inside mask.
     */
    std::vector<std::pair<Position<2>,T> > get_nodes(const Mask<2> &mask) const;

    /**
     * This function returns a node iterator which will traverse the rest
     * of the tree, including parent Quadrants, starting with the first
     * node in this Quadrant.
     * @returns iterator for nodes in quadtree.
     */
    iterator begin()
      { return iterator(*this); }

    iterator end()
      { return iterator(); }

    /**
     * @returns true if quadrant is a leaf.
     */
    bool is_leaf() const;

  protected:
    /**
     * Change a leaf quadrant to a regular quadrant with four
     * children regions.
     */
    void split_();

    /**
     * Append this quadrant's nodes to the vector
     */
    void append_nodes_(std::vector<std::pair<Position<2>,T> >&) const;

    /**
     * Append this quadrant's nodes inside the mask to the vector
     */
    void append_nodes_(std::vector<std::pair<Position<2>,T> >&, const Mask<2> &) const;

    /**
     * @returns the subquad number for this position
     */
    int subquad_(const Position<2>&);

    Position<2> lower_left_;
    Position<2> extent_;

    bool leaf_;

    std::vector<std::pair<Position<2>,T> > nodes_;

    Quadrant* parent_;
    int my_subquad_;    ///< This Quadrant's subquad number within parent
    Quadrant* children_[4];

    friend class quadrant_iterator;
    friend class iterator;
  };

  template<class T, int max_capacity>
  Quadrant<T,max_capacity>::Quadrant(const Position<2>& lower_left,
                                     const Position<2>& extent,
                                     Quadrant<T,max_capacity>* parent=0,
                                     int subquad=0) :
    lower_left_(lower_left),
    extent_(extent),
    parent_(parent),
    my_subquad_(subquad),
    leaf_(true)
  {
  }

  template<class T, int max_capacity>
  bool Quadrant<T,max_capacity>::is_leaf() const
  {
    return leaf_;
  }

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::append_nodes_(std::vector<std::pair<Position<2>,T> >&v) const
  {
    if (leaf_) {
      std::copy(nodes_.begin(), nodes_.end(), std::back_inserter(v));
    } else {
      for (int i=0;i<4;++i)
        children_[i]->append_nodes_(v);
    }
  }

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::append_nodes_(std::vector<std::pair<Position<2>,T> >&v, const Mask<2> &mask) const
  {
  }

  template<class T, int max_capacity>
  std::vector<std::pair<Position<2>,T> > Quadrant<T,max_capacity>::get_nodes() const
  {
    std::vector<std::pair<Position<2>,T> > result;
    append_nodes_(result);
    return result;
  }

  template<class T, int max_capacity>
  std::vector<std::pair<Position<2>,T> > Quadrant<T,max_capacity>::get_nodes(const Mask<2> &mask) const
  {
    std::vector<std::pair<Position<2>,T> > result;
    append_nodes_(result,mask);
    return result;
  }

  template<class T, int max_capacity>
  int Quadrant<T,max_capacity>::subquad_(const Position<2>& pos)
  {
    Position<2> offset = pos - lower_left_;

    offset /= extent_/2;

    return int(offset[0]) + 2*int(offset[1]);
  }

  template<class T, int max_capacity>
  void Quadrant<T,max_capacity>::insert(const Position<2>& pos, const T& node)
  {
    if (leaf_ && (nodes_.size()>=max_capacity))
      split_();

    if (leaf_) {

      assert((pos >= lower_left_) && (pos < lower_left_ + extent_));

      nodes_.push_back(std::pair<Position<2>,T>(pos,node));

    } else {

      children_[subquad_(pos)]->insert(pos,node);

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


} // namespace nest

#endif
