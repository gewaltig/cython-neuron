/*
 *  subnet.cpp
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

#include "event.h"
#include "subnet.h"
#include "dictdatum.h"
#include "arraydatum.h"
#include "dictutils.h"
#include "network.h"
#include <string>

#ifdef N_DEBUG
#undef N_DEBUG
#endif

nest::Subnet::Subnet()
  :Node(),
   nodes_(),
   children_on_same_vp_(false),
   children_vp_(0),
   label_(),
   customdict_(new Dictionary),
   homogeneous_(true),
   next_lid_(0),
   last_mid_(0)
{
  set(frozen);  // freeze subnet by default
}

nest::Subnet::Subnet(const Subnet &c)
  :Node(c),
   nodes_(c.nodes_),
   children_on_same_vp_(c.children_on_same_vp_),
   children_vp_(c.children_vp_),
   label_(c.label_),
   customdict_(new Dictionary(*(c.customdict_))),
   homogeneous_(c.homogeneous_),
   next_lid_(c.next_lid_),
   last_mid_(c.last_mid_)
{}

void nest::Subnet::set_status(const DictionaryDatum& dict)
{
  updateValue<std::string>(dict,"label",label_);
  updateValue<DictionaryDatum> (dict,"customdict",customdict_);

  bool children_on_same_vp;
  if (updateValue<bool>(dict,"children_on_same_vp", children_on_same_vp))
  {
    bool parent_children_on_same_vp = (get_gid() == 0) ? false : get_parent()->get_children_on_same_vp();
    if (parent_children_on_same_vp && !children_on_same_vp)
    {
      network()->message(SLIInterpreter::M_ERROR, "SetStatus", "Setting /children_on_same_vp to false is not possible,");
      network()->message(SLIInterpreter::M_ERROR, "SetStatus", "because it set to true in the parent subnet.");
    }
    else if (nodes_.size() > 0)
    {
      network()->message(SLIInterpreter::M_ERROR, "SetStatus", "Modifying /children_on_same_vp is not possible,");
      network()->message(SLIInterpreter::M_ERROR, "SetStatus", "because the subnet already contains nodes."); 
    }
    else
      children_on_same_vp_ = children_on_same_vp;
  }
}

void nest::Subnet::get_status(DictionaryDatum& dict) const
{
  (*dict)["number_of_children"]= size();
  (*dict)["label"]=label_;
  (*dict)["customdict"]=customdict_;
  (*dict)["children_on_same_vp"]=children_on_same_vp_; 
}

void nest::Subnet::get_dimensions_(std::vector<int> & dim) const
{
  dim.push_back(next_lid_);
  if(nodes_.empty())
    return;
  if(homogeneous_ && (dynamic_cast<Subnet *>(nodes_.at(0)) !=NULL))
    {
      bool homog=true;
      for(size_t i=0; i< nodes_.size()-1; ++i)
	{
	  Subnet *c1=dynamic_cast<Subnet *>(nodes_.at(i));
	  Subnet *c2=dynamic_cast<Subnet *>(nodes_.at(i+1));

	  if(c1->size() != c2->size())
	    {
	      homog=false;
	      continue;
	    }
	}
      // If homog is true, all child-subnets have the same size
      // and we go one level deeper.
      if(homog)
	{
	  Subnet *c=dynamic_cast<Subnet *>(nodes_.at(0));
	  c->get_dimensions_(dim);
	}
    }
}


std::string nest::Subnet::print_network(int max_depth, int level, std::string prefix)
{
  // When the function is first called, we have to have a single
  // space as prefix, otherwise everything will by slightly out of
  // format.
  if(prefix == "")
    prefix=" ";

  std::ostringstream out;
  if(get_parent())
  {
    out << "+-[" << get_lid()+1 << "] ";
 
    if (get_label() != "")
      out << get_label();
    else
      out << get_name();
  }
  else
  {
    out << "+-" << "[0] ";
    if (get_label() != "")
      out << get_label();
    else
      out << "root";
  }

  std::vector<int> dim;
  get_dimensions_(dim);

  out << " dim=[";
  for(size_t k = 0; k < dim.size() - 1; ++k)
    out << dim[k] << " ";
  out << dim[dim.size() - 1] << "]" << std::endl;

  if(max_depth <= level)
    return out.str();

  if(nodes_.size()==0)
    return out.str();

  prefix += "  ";
  out << prefix << "|" << std::endl;

  size_t first=0;
  for(size_t i=0; i< nodes_.size(); ++i)
  {

    size_t next=i+1;
    if(nodes_[i]==NULL)
    {
      out << prefix << "+-NULL" << std::endl;
      // Print extra line, if we are at the end of a subnet.
      if(next==nodes_.size())
	out << prefix << std::endl;
      first=i+1;
      continue;
    }
    
    Subnet *c=dynamic_cast<Subnet *>(nodes_[i]);
    if(c !=NULL)
    {
      // this node is a subnet,
      // the sequence is printed, so
      // we print the children and move on
      // print subnet
      //
      // If the subnet is the last node of the parent subnet,
      // we must not print the continuation line '|', so we distinguish 
      // this case.
      if(next==nodes_.size())
	out << prefix << nodes_[i]->print_network(max_depth, level + 1, prefix+" ");
      else
	out << prefix << nodes_[i]->print_network(max_depth, level + 1, prefix+"|");

      first=next;
      continue;
    }
      
    // now we look one into the future
    // to determine whether this is a sequence
    // or not.
      
    if(next < nodes_.size())
    {
      // we have a successor
      if(nodes_[next]!=NULL)
      {
	// it is not NULL
	
	c=dynamic_cast<Subnet *>(nodes_[next]);
	if(c == NULL)
	{
	  // and not a subnet, so we skipp 
	  // the printout, until the end
	  // of the sequence is found.
	  if((nodes_[first]->get_name() == nodes_[next]->get_name()))
	  {
	    continue;
	  }
	} // if the next node is a compount we flush the sequence 
      } // if the next node is NULL, we flush the sequence
    } // if there is no next node, we flush the sequence
    
    if(first<i)
    {
      // Here we print the sequence of consecutive nodes.
      // We can be sure that neither first, nor i point to NULL.
      out << prefix << "+-[" << first+1 << "]...[" << i+1 << "] " 
	  << nodes_[first]->get_name() << std::endl;
    // Print extra line, if we are at the end of a subnet.
      if(next==nodes_.size())
	out << prefix << std::endl;
      first=next;
      continue;
    }
    
    // Here, we deal the case of an individual Node with no identical neighbours.

      out << prefix << "+-[" << i+1 << "] " 
	  << nodes_[first]->get_name() << std::endl;

    // Print extra line, if we are at the end of a subnet.
    if(next==nodes_.size())
       out << prefix << std::endl;
    first=next;
    
  }
  return out.str();
}

void nest::Subnet::set_label(std::string const l)
{
  // set the new label on all sibling threads
  for (thread t = 0; t < network()->get_num_threads(); ++t)
  {
    Node* n = network()->get_node(get_gid(), t);
    Subnet* c = dynamic_cast<Subnet*>(n);
    assert(c);
    c->set_label_non_recursive(l);
  }
}

void nest::Subnet::set_label_non_recursive(std::string const l)
{
  label_=l;
}

bool nest::Subnet::allow_entry() const
{
  return true;
}

