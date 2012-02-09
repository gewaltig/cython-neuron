/*
 *  nodelist.h
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

#ifndef NODELIST_H
#define NODELIST_H

#include "node.h"
#include "subnet.h"

namespace nest{

/**
 * List interface to network tree.
 *
 * LocalNodeList is an adaptor class which provides and iterator
 * interface to a subnet. The iterator traverses all underlying
 * subnets recursively in depth-first (post-order) order. This
 * iterator traverses only local nodes.
 *
 * This iterator is not used during Network update, since it is not
 * thread safe.
 *
 * For a list interface that only accesses the leaves of a network
 * tree, excluding the intermediate subnets, see class LocalLeafList
 * and its iterator.
 * LocalNodeList iterates only over local nodes.
 * @see GlobalNodeList
 */

template <typename ListIterator>
class NodeList
{
public:
  typedef ListIterator iterator;

  explicit NodeList(Subnet &subnet) : subnet_(subnet) {}

  /**
   * Return iterator pointing to first node in subnet.
   * @node Must be specialized by derived class.
   */
  iterator begin() const { assert(false);
                           return end(); }

  /**
   * Return iterator pointing to node past last node.
   */
  iterator end() const { return iterator(subnet_.local_end(),
                                         subnet_.local_end()); }

  //! Returns true if no local nodes
  bool empty() const { return subnet_.local_empty(); }

  //! Returns subnet wrapped by NodeList
  Subnet& get_subnet() const { return subnet_; }

private:
  Subnet& subnet_;  //!< root of the network
};

// ----------------------------------------------------------------------------

class LocalNodeListIterator
{
  friend class NodeList<LocalNodeListIterator>;
  friend class LocalLeafListIterator;

  private:
   //! Create iterator from pointer to Node in subnet
   LocalNodeListIterator(std::vector<Node*>::iterator const &node,
                         std::vector<Node*>::iterator const &list_end) :
     current_node_(node), list_end_(list_end) {}
   bool is_end_() const { return current_node_ == list_end_; }

 public:
   LocalNodeListIterator operator++();

   Node*        operator*() { return *current_node_; }
   Node const*  operator*() const { return *current_node_; }

   bool operator==(const LocalNodeListIterator& i) const { return current_node_ == i.current_node_; }
   bool operator!=(const LocalNodeListIterator& i) const { return not ( *this == i ); }

 private:
   //! iterator to the current node in subnet
   vector<Node *>::iterator current_node_;
   vector<Node *>::iterator list_end_;

};

template <>
NodeList<LocalNodeListIterator>::iterator
  NodeList<LocalNodeListIterator>::begin() const;

typedef NodeList<LocalNodeListIterator> LocalNodeList;

// ----------------------------------------------------------------------------

class LocalChildListIterator
{
  friend class NodeList<LocalChildListIterator>;

  private:
   //! Create iterator from pointer to Node in subnet
   LocalChildListIterator(std::vector<Node*>::iterator const &node,
                          std::vector<Node*>::iterator const &list_end) :
     current_node_(node), list_end_(list_end) {}

 public:
   LocalChildListIterator operator++();

   Node*        operator*() { return *current_node_; }
   Node const*  operator*() const { return *current_node_; }

   bool operator==(const LocalChildListIterator& i) const { return current_node_ == i.current_node_; }
   bool operator!=(const LocalChildListIterator& i) const { return not ( *this == i ); }

 private:
   //! iterator to the current node in subnet
   vector<Node *>::iterator current_node_;
   vector<Node *>::iterator list_end_;
};

template <>
NodeList<LocalChildListIterator>::iterator
  NodeList<LocalChildListIterator>::begin() const;

typedef NodeList<LocalChildListIterator> LocalChildList;

// ----------------------------------------------------------------------------

class LocalLeafListIterator
{
  friend class NodeList<LocalLeafListIterator>;

  private:
   //! Create iterator from pointer to Node in subnet
   LocalLeafListIterator(std::vector<Node*>::iterator const &node,
                          std::vector<Node*>::iterator const &list_end) :
     base_it_(node, list_end)
   {
     while ( not base_it_.is_end_() && not is_leaf_(*base_it_) )
       ++base_it_;
   }

 public:
   LocalLeafListIterator operator++();

   Node*        operator*() { return *base_it_; }
   Node const*  operator*() const { return *base_it_; }

   bool operator==(const LocalLeafListIterator& i) const { return base_it_ == i.base_it_; }
   bool operator!=(const LocalLeafListIterator& i) const { return not ( *this == i ); }

 private:
   LocalNodeListIterator base_it_;  //<! we use this one for the basic iteration

   static bool is_leaf_(Node *n)  { return not dynamic_cast<Subnet*>(n); }

};

template <>
NodeList<LocalLeafListIterator>::iterator
  NodeList<LocalLeafListIterator>::begin() const;

typedef NodeList<LocalLeafListIterator> LocalLeafList;



}
#endif
