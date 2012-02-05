/*
 *  nodelist.h
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

#ifndef NODELIST_H
#define NODELIST_H

#include "node.h"
#include "subnet.h"

namespace nest{

  /** 
   * List interface to network tree.  class LocalNodeList is an adaptor
   * class which turns a Network object into a list.  Class LocalNodeList
   * also provides an iterator which can be used to traverse the
   * network tree in post-order.  This iterator is not used during
   * Network update, since it is not thread safe.
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
    public:
      iterator():p_(){}
    private:
      iterator(vector<Node*>::iterator const &p):p_(p){}
    public:
      iterator operator++();

      Node*    operator*();
      Node const*  operator*() const;

      bool operator==(const iterator&) const;
      bool operator!=(const iterator&) const;

    private:
      vector<Node *>::iterator p_;  //!< iterator to the current node
    };

    LocalNodeList():root_(NULL){}
    explicit LocalNodeList(Subnet &c):root_(&c){};

    iterator begin() const;
    iterator end()   const;

    bool   empty()   const; //!< Returns true if no local nodes
    size_t size()    const; //!< Number of (local) nodes in list

    Subnet& get_root() const;
    void set_root(Subnet &);

  private:
    Subnet *root_;  //!< root of the network

  };

  inline 
  bool LocalNodeList::empty() const
  {
    return root_->local_empty();
  }

  inline
  size_t LocalNodeList::size() const
  {
    return root_->local_size();
  }

  inline
  LocalNodeList::iterator LocalNodeList::end() const
  {
    Subnet *p=root_->get_parent();
    return iterator(p == NULL ? root_->local_end()
    			              : p->local_begin()+root_->get_lid());
  }

  inline
  bool LocalNodeList::iterator::operator==(const iterator&i) const
  {
    return p_ == i.p_;
  }

  inline
  bool LocalNodeList::iterator::operator!=(const iterator&i) const
  {
    return p_ != i.p_;
  }

  inline
  Node* LocalNodeList::iterator::operator*()
  {
    return *p_;
  }

  inline
  Node const * LocalNodeList::iterator::operator*() const
  {
    return *p_;
  }
}
#endif
