#ifndef NTREE_H
#define NTREE_H

/*
 *  ntree.h
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

  class AbstractMask;

  template<int D>
  class Mask;

  /**
   * Abstract base class for all Ntrees containing items of type T
   */
  template<class T>
  class AbstractNtree
  {
  public:
    /**
     * @returns member nodes in ntree without positions.
     */
    virtual std::vector<T> get_nodes_only() = 0;

    /**
     * Applies a Mask to this ntree.
     * @returns member nodes in ntree inside mask without positions.
     */
    virtual std::vector<T> get_nodes_only(const AbstractMask &mask) = 0;
  };

  /**
   * A Ntree object represents a subtree or leaf in a Ntree structure. Any
   * ntree covers a specific region in space. A leaf ntree contains a list
   * of items and their corresponding positions. A branch ntree contains a
   * list of N=1<<D other ntrees, each covering a region corresponding to the
   * upper-left, lower-left, upper-right and lower-left corner of their
   * mother ntree.
   *
   */
  template<int D, class T, int max_capacity=100>
  class Ntree : public AbstractNtree<T>
  {
  public:

    class iterator;

    static const int N = 1<<D;

    /**
     * Helper class which points to a leaf ntree
     */
    class ntree_iterator {
    public:
      /**
       * Constructor for invalid iterator.
       */
      ntree_iterator() : ntree_(0) {}

      /**
       * Initialize the iterator to point to the first leaf ntree below
       * the given ntree.
       */
      ntree_iterator(Ntree& q) : ntree_(&q)
        {
          while(!ntree_->is_leaf())
            ntree_ = ntree_->children_[0];
        }

      bool valid() { return ntree_ != 0; }

      ntree_iterator& operator++()
        {

          // If we are on the last subntree, move up
          while(ntree_ && (ntree_->my_subquad_ == N-1)) {
            ntree_ = ntree_->parent_;
          }

          // If we reached the end, return
          if (ntree_ == 0)
            return *this;

          // If we have reached the top, mark as invalid and return
          if (ntree_->parent_ == 0) {
            ntree_ = 0;
            return *this;
          }

          // Move to next sibling
          ntree_ = ntree_->parent_->children_[ntree_->my_subquad_+1];

          // Move down if this is not a leaf.
          while(!ntree_->is_leaf())
            ntree_ = ntree_->children_[0];

          return *this;
        }

      Ntree& operator*()
        { return *ntree_; }

      Ntree* operator->()
        { return ntree_; }

      bool operator==(const ntree_iterator &other) const
        { return ntree_ == other.ntree_; }

      bool operator!=(const ntree_iterator &other) const
        { return ntree_ != other.ntree_; }

      friend class iterator;

    protected:

      Ntree *ntree_;

    };

    /**
     * Iterator iterating the nodes in a Quadtree. The iterator will
     * traverse the tree until it finds leaf Ntrees.
     */
    class iterator {
    public:
      /**
       * Initialize an invalid iterator.
       */
      iterator() : ntree_(), node_(0) {}

      /**
       * Initialize an iterator to point to the first leaf node within the
       * tree below this Ntree.
       */
      iterator(Ntree& q) : ntree_(q), node_(0) {}

      std::pair<Position<D>,T> & operator*() { return ntree_->nodes_[node_]; }
      std::pair<Position<D>,T> * operator->() { return &ntree_->nodes_[node_]; }

      /**
       * Move the iterator to the next node within the tree.
       */
      iterator & operator++()
        {
          node_++;

          while (node_ >= ntree_->nodes_.size()) {

            ++ntree_;

            node_ = 0;

            if (!ntree_.valid()) break;

          }

          return *this;
        }

      /**
       * Iterators are equal if they point to the same node in the same
       * ntree.
       */
      bool operator==(const iterator &other) const
        { return (other.ntree_==ntree_) && (other.node_==node_); }
      bool operator!=(const iterator &other) const
        { return (other.ntree_!=ntree_) || (other.node_!=node_); }

    protected:

      ntree_iterator ntree_;
      index node_;
    };

    /**
     * Create a Ntree that covers the region defined by the two
     * input positions.
     * @param lower_left  Lower left corner of ntree.
     * @param extent      Size (width,height) of ntree.
     */
    Ntree(const Position<D>& lower_left,
	     const Position<D>& extent, Ntree *parent, int subquad);

    /**
     * Traverse quadtree structure from current ntree.
     * Inserts node in correct leaf in quadtree.
     */
    void insert(const Position<D>& pos, const T& node);

    /**
     * @returns member nodes in ntree and their position.
     */
    std::vector<std::pair<Position<D>,T> > get_nodes();

    /**
     * Applies a Mask to this ntree.
     * @returns member nodes in ntree inside mask.
     */
    std::vector<std::pair<Position<D>,T> > get_nodes(const Mask<D> &mask);

    /**
     * @returns member nodes in ntree without positions.
     */
    std::vector<T> get_nodes_only();

    /**
     * Applies a Mask to this ntree.
     * @returns member nodes in ntree inside mask without positions.
     */
    std::vector<T> get_nodes_only(const AbstractMask &mask);

    /**
     * This function returns a node iterator which will traverse the rest
     * of the tree, including parent Ntrees, starting with the first
     * node in this Ntree.
     * @returns iterator for nodes in quadtree.
     */
    iterator begin()
      { return iterator(*this); }

    iterator end()
      { return iterator(); }

    /**
     * @returns true if ntree is a leaf.
     */
    bool is_leaf() const;

  protected:
    /**
     * Change a leaf ntree to a regular ntree with four
     * children regions.
     */
    void split_();

    /**
     * Append this ntree's nodes to the vector
     */
    void append_nodes_(std::vector<std::pair<Position<D>,T> >&);

    /**
     * Append this ntree's nodes inside the mask to the vector
     */
    void append_nodes_(std::vector<std::pair<Position<D>,T> >&, const Mask<D> &);

    /**
     * @returns the subquad number for this position
     */
    int subquad_(const Position<D>&);

    Position<D> lower_left_;
    Position<D> extent_;

    bool leaf_;

    std::vector<std::pair<Position<D>,T> > nodes_;

    Ntree* parent_;
    int my_subquad_;    ///< This Ntree's subquad number within parent
    Ntree* children_[N];

    friend class ntree_iterator;
    friend class iterator;
  };

  template<int D, class T, int max_capacity>
  Ntree<D,T,max_capacity>::Ntree(const Position<D>& lower_left,
                                     const Position<D>& extent,
                                     Ntree<D,T,max_capacity>* parent=0,
                                     int subquad=0) :
    lower_left_(lower_left),
    extent_(extent),
    leaf_(true),
    parent_(parent),
    my_subquad_(subquad)
  {
  }

  template<int D, class T, int max_capacity>
  bool Ntree<D,T,max_capacity>::is_leaf() const
  {
    return leaf_;
  }


  template<int D, class T, int max_capacity>
  std::vector<std::pair<Position<D>,T> > Ntree<D,T,max_capacity>::get_nodes()
  {
    std::vector<std::pair<Position<D>,T> > result;
    append_nodes_(result);
    return result;
  }

  template<int D, class T, int max_capacity>
  std::vector<std::pair<Position<D>,T> > Ntree<D,T,max_capacity>::get_nodes(const Mask<D> &mask)
  {
    std::vector<std::pair<Position<D>,T> > result;
    append_nodes_(result,mask);
    return result;
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
  void Ntree<D,T,max_capacity>::insert(const Position<D>& pos, const T& node)
  {
    if (leaf_ && (nodes_.size()>=max_capacity))
      split_();

    if (leaf_) {

      assert((pos >= lower_left_) && (pos < lower_left_ + extent_));

      nodes_.push_back(std::pair<Position<D>,T>(pos,node));

    } else {

      children_[subquad_(pos)]->insert(pos,node);

    }
  }

} // namespace nest

#endif
