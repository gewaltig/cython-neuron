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

class LocalNodeList
{
public:

  class iterator
  {
    friend class LocalNodeList;

  private:
    //! Create iterator from pointer to Node in subnet
    iterator(std::vector<Node*>::iterator const &node,
             std::vector<Node*>::iterator const &list_end) :
      current_node_(node), list_end_(list_end) {}

  public:
    iterator operator++();

    Node*        operator*();
    Node const*  operator*() const;

    bool operator==(const iterator&) const;
    bool operator!=(const iterator&) const;

  private:
    //! iterator to the current node in subnet
    vector<Node *>::iterator current_node_;
    vector<Node *>::iterator list_end_;
  };

  explicit LocalNodeList(Subnet &subnet) : subnet_(subnet) {}

  /**
   * Return iterator pointing to first node in subnet.
   *
   * Depth-first/post-order traversal means the first node is
   * the leftmost, bottommost node.
   */
  iterator begin() const;

  /**
   * Return iterator pointing to node past last node.
   */
  iterator end()   const;

  bool   empty()   const; //!< Returns true if no local nodes
  size_t size()    const; //!< Number of (local) nodes in list

  Subnet& get_subnet() const; //!< Return subnet wrapped by LocalNodeList

private:
  Subnet& subnet_;  //!< root of the network

};

inline
bool LocalNodeList::empty() const
{
  return subnet_.local_empty();
}

inline
size_t LocalNodeList::size() const
{
  return subnet_.local_size();
}

inline
bool LocalNodeList::iterator::operator==(const iterator& i) const
{
  return current_node_ == i.current_node_;
}

inline
bool LocalNodeList::iterator::operator!=(const iterator& i) const
{
  return current_node_ != i.current_node_;
}

inline
Node* LocalNodeList::iterator::operator*()
{
  return *current_node_;
}

inline
Node const * LocalNodeList::iterator::operator*() const
{
  return *current_node_;
}

}
#endif
