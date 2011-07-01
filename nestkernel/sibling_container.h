/*
 *  sibling_container.h
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

#ifndef SIBLING_CONTAINER_H
#define SIBLING_CONTAINER_H
#include <vector>
#include <string>
#include "node.h"
#include "dictdatum.h"

namespace nest{

  using std::vector;

  class Node;

  /**
   * SiblingContainer class.
   * This class is used to group the replicas of nodes on different
   * threads into one entity. It is derived from node in order to take
   * advantage of the pool allocator, which has only very little
   * overhead compared to a normal std::vector.
   */
  class SiblingContainer: public Node
  {
  public:

    SiblingContainer();   

    SiblingContainer(const SiblingContainer &);

    virtual ~SiblingContainer(){}
   
    void set_status(const DictionaryDatum&) { assert(false); }
    void get_status(DictionaryDatum&) const { assert(false); }

    bool has_proxies() const;
          
    Node * at(index) const;
    Node * operator[](index) const;

    size_t size() const;
    size_t local_size() const;
    bool   empty() const;
    void   reserve(size_t);

    void push_back(Node*);
    
    /**
     * Add a node to the SiblingContainer. The node is appended to
     * the SiblingContainer's child-list.
     */ 
    void add_node(Node *);

    /**
     * Return iterator to the first child node.
     */
    vector<Node*>::iterator begin();

    /**
     * Return iterator to the end of the child-list.
     */
    vector<Node*>::iterator end();

    /**
     * Return const iterator to the first child node.
     */
    vector<Node*>::const_iterator begin() const;

    /**
     * Return const iterator to the end of the child-list.
     */
    vector<Node*>::const_iterator end() const;

  protected:
    void init_node_(const Node&) {}
    void init_state_(const Node&) {}
    void init_buffers_() {}

    void calibrate() {}
    void update(Time const &, const long_t, const long_t) {}
    
    /**
     * Pointer to child nodes.
     * This vector contains the pointers to the child nodes.
     * Since deletion of Nodes is possible, entries in this
     * vector may be NULL. Note that all code must handle
     * this case gracefully.
     */
    vector<Node *> nodes_;       //!< Pointer to child nodes.
  };

  inline
  void SiblingContainer::add_node(Node *n)
  {
// Do we need to do this for a SiblinContainer?
//    const index lid = next_lid_;
//    const index mid = n->get_model_id();
//    if ((homogeneous_) && (lid > 0))
//      if (mid != last_mid_)
//	homogeneous_ = false;
//    n->set_lid_(next_lid_);
    nodes_.push_back(n);
//    n->set_parent_(this);
//    next_lid_++;
//    last_mid_ = mid;
//    return lid;
  }

  inline
  void SiblingContainer::push_back(Node *n)
  {
    nodes_.push_back(n);
// Do we need to do this for a SiblinContainer?
//    last_mid_ = n->get_model_id();
//    next_lid_++;
  }  
  
  /**
   * Index child node (with range check).
   * \throws std::out_of_range (implemented via \c std::vector)
   */
  inline
  Node* SiblingContainer::at(index i) const
  {
    return nodes_.at(i); //with range check
  }

  /**
   * Index child node (without range check).
   */
  inline
  Node* SiblingContainer::operator [](index i) const
  {
    return nodes_[i]; //without range check
  }

  inline
  vector<Node*>::iterator SiblingContainer::begin()
  {
    return nodes_.begin();
  }

  inline
  vector<Node*>::iterator SiblingContainer::end()
  {
    return nodes_.end();
  }

  inline
  vector<Node*>::const_iterator SiblingContainer::begin() const
  {
    return nodes_.begin();
  }

  inline
  vector<Node*>::const_iterator SiblingContainer::end() const
  {
    return nodes_.end();
  }

  inline
  bool SiblingContainer::empty() const
  {
    return nodes_.empty();
  }

  inline
  size_t SiblingContainer::size() const
  {
    return nodes_.size();
//    return next_lid_;
  }

  inline
  size_t SiblingContainer::local_size() const
  {
    return nodes_.size();
  }

  inline 
  void SiblingContainer::reserve(size_t n)
  {
    nodes_.reserve(n);
  }

  inline
  bool SiblingContainer::has_proxies() const
  {
    return false;
  }

} // namespace

#endif
