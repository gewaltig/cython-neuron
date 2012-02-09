/*
 *  subnet.h
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

#ifndef SUBNET_H
#define SUBNET_H
#include <vector>
#include <string>
#include "node.h"
#include "dictdatum.h"

/* BeginDocumentation

Name: subnet - Root node for subnetworks.

Description:
A network node of type subnet serves as a root node for subnetworks

Parameters:
Parameters that can be accessed via the GetStatus and SetStatus functions:

children_on_same_vp (booltype) -
   Whether all children are allocated on the same virtual process
customdict (dictionarytype) -
   A user-defined dictionary, which may be used to store additional
   data.
label (stringtype) -
  A user-defined string, which may be used to give a symbolic name to
  the node. From the SLI level, the FindNodes command may be used to
  find a subnet's address from it's label.
number_of_children (integertype) -
  The numbber of direct children of the subnet

  SeeAlso: modeldict, Node
*/

namespace nest{

  using std::vector;

  class Node;

  /**
   * Base class for all subnet nodes.
   * This class can be used
   * - to group other Nodes into "sub-networks"
   * - to construct Node classes which are composed of multiple 
   *   subnodes.
   */
  class Subnet: public Node
  {
  public:

    Subnet();   

    Subnet(const Subnet &);

    virtual ~Subnet(){}
   
    void set_status(const DictionaryDatum&);
    void get_status(DictionaryDatum&) const;

    bool has_proxies() const;
          
    size_t global_size() const;  //!< Returns total number of children.
    size_t local_size() const; //!< Returns number of childern in local process.
    bool   global_empty() const; //!< returns true if subnet is empty *globally*
    bool   local_empty() const; //!< returns true if subnet has no local nodes

    void   reserve(size_t);

    /**
     * Add a local node to the subnet.
     * This function adds a node to the subnet and returns its local id.
     * The node is appended to the subnet child-list. 
     */ 
    index add_node(Node *);

    /**
     * Add a remote node to the subnet.
     * This function increments the next local id to be assigned.
     */ 
    index add_remote_node(index mid);

    /**
     * Return iterator to the first local child node.
     */
    vector<Node*>::iterator local_begin();

    /**
     * Return iterator to the end of the local child-list.
     */
    vector<Node*>::iterator local_end();

    /**
     * Return const iterator to the first local child node.
     */
    vector<Node*>::const_iterator local_begin() const;

    /**
     * Return const iterator to the end of the local child-list.
     */
    vector<Node*>::const_iterator local_end() const;

    /**
     * Return the subnets's user label.
     * Each subnet can be given a user-defined string as a label, which
     * may be used to give a symbolic name to the node. From the SLI
     * level, the FindNodes command may be used to find a subnet's
     * addess from it's label.
     */
    std::string get_label() const;
    
    /**
     * Set the subnet's user label.
     * Each subnet can be given a user-defined string as a label, which
     * may be used to give a symbolic name to the node. From the SLI
     * level, the FindNodes command may be used to find a subnet's
     * addess from it's label. This sets the label for all nodes on the
     * same level (i.e. for all threads) simulataneously
     */
    void set_label(std::string const);

    
    /**
     * Set the subnet's user label.
     * Each subnet can be given a user-defined string as a label, which
     * may be used to give a symbolic name to the node. From the SLI
     * level, the FindNodes command may be used to find a subnet's
     * addess from it's label. This does not set the label for the nodes
     * on other threads.
     */
    void set_label_non_recursive(std::string const);

    /**
     * Set the subnet's custom dictionary.
     * Each subnet can be given a user-defined dictionary, which
     * may be used to store additional data. From the SLI
     * level, the SetStatus command may be used to set a subnet's
     * custom dictionary.
     */
    DictionaryDatum get_customdict() const;
    
    /**
     * Return pointer to the subnet's custom dictionary.
     * Each subnet contains a user-definable dictionary, which
     * may be used to store additional data. From the SLI
     * level, the SetStatus command may be used to set a subnet's
     * custom dictionary.
     */
    void set_customdict(DictionaryDatum const dict);
    
    std::string print_network(int , int, std::string = "");

    bool get_children_on_same_vp() const;
    void set_children_on_same_vp(bool);

    thread get_children_vp() const;
    void set_children_vp(thread);
    
    virtual bool allow_entry() const;

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

    /**
     * flag indicating if all children of this subnet have to
     * be created on the same virtual process or not. Use with
     * care. This may lead to severe performance problems!
     */
    bool children_on_same_vp_;
    thread children_vp_;
    
  private:
    void get_dimensions_(std::vector<int>&) const;

    std::string     label_;      //!< user-defined label for this node.
    DictionaryDatum customdict_; //!< user-defined dictionary for this node.
    // note that DictionaryDatum is a pointer and must be initialized in the constructor.
    bool homogeneous_;           //!< flag which indicates if the subnet contains different kinds of models.
    index next_lid_;             //!< local index of next child
    index last_mid_;             //!< model index of last child
  };

  /**
   * Add a local node to the subnet.
   */
  inline
  index Subnet::add_node(Node *n)
  {
    const index lid = next_lid_;
    const index mid = n->get_model_id();
    if ((homogeneous_) && (lid > 0))
      if (mid != last_mid_)
	homogeneous_ = false;
    n->set_lid_(next_lid_);
    nodes_.push_back(n);
    n->set_parent_(this);
    next_lid_++;
    last_mid_ = mid;
    return lid;
  }
  /**
   * Add a remote node to the subnet.
   */
  inline
  index Subnet::add_remote_node(index mid)
  {
    const index lid = next_lid_;
    if((homogeneous_) && (lid > 0))
      if (mid != last_mid_)
	homogeneous_ = false;
    last_mid_ = mid;
    next_lid_++;
    return lid;
  }
  
  inline
  vector<Node*>::iterator Subnet::local_begin()
  {
    return nodes_.begin();
  }

  inline
  vector<Node*>::iterator Subnet::local_end()
  {
    return nodes_.end();
  }

  inline
  vector<Node*>::const_iterator Subnet::local_begin() const
  {
    return nodes_.begin();
  }

  inline
  vector<Node*>::const_iterator Subnet::local_end() const
  {
    return nodes_.end();
  }

  inline
  bool Subnet::local_empty() const
  {
    return nodes_.empty();
  }

  inline
  bool Subnet::global_empty() const
  {
    return next_lid_ == 0;
  }

  inline
  size_t Subnet::global_size() const
  {
    return next_lid_;
  }

  inline
  size_t Subnet::local_size() const
  {
    return nodes_.size();
  }

  inline 
  void Subnet::reserve(size_t n)
  {
    nodes_.reserve(n);
  }

  inline 
  std::string Subnet::get_label() const
  {
    return label_;
  }

  inline 
  DictionaryDatum Subnet::get_customdict() const
  {
    return customdict_;
  }
  
  inline 
  void Subnet::set_customdict(DictionaryDatum const d)
  {
    customdict_=d;
  }
  
  inline
  bool Subnet::has_proxies() const
  {
    return false;
  }

  inline
  bool Subnet::get_children_on_same_vp() const
  {
    return children_on_same_vp_;
  }

  inline
  void Subnet::set_children_on_same_vp(bool children_on_same_vp)
  {
    children_on_same_vp_ = children_on_same_vp;
  }

  inline
  thread Subnet::get_children_vp() const
  {
    return children_vp_;
  }

  inline
  void Subnet::set_children_vp(thread children_vp)
  {
    children_vp_ = children_vp;
  }
  
} // namespace

#endif
