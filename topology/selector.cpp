/*
 *  selector.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/*
  This file is part of the NEST topology module.
  Author: Kittel Austvoll
*/

#include "selector.h"

#include "compound.h"
#include "nestmodule.h"
#include "dictdatum.h"

#include "topology_names.h"

namespace nest{

  Selector::Selector():
    slice_depth_(0),
    modeltype_(-1)
  {}
  
  Selector::Selector(const DictionaryDatum& selection_dict):
    slice_depth_(0),
    modeltype_(-1)
  {
    // Retrieves information about which depth nodes will
    // be picked from. If the variable is left unchanged
    // (i.e. given the value 0) nodes will be picked from 
    // all depths. LID is a reference to local node id
    updateValue<long_t>(selection_dict, "lid", slice_depth_);
    
    // Get information about which node type that should
    // be connected to. All model types will be connected
    // to if no modelname is given.
    std::string modelname;
    
    if(updateValue<std::string>(selection_dict, names::model, 
				modelname))
      {
	//Get model type.
	const Token model = 
	  NestModule::get_network().get_modeldict().lookup(modelname);
	
	if ( model.empty() )
	  throw UnknownModelName(modelname);
	
	modeltype_ = static_cast<long_t>(model);
      }
  }

  void Selector::slice_node(Compound& subnet,
			    Node* node)
  {
    slice_node(subnet, node, slice_depth_);
  }
  
  void Selector::slice_node(Compound& subnet,
			    Node* node,
			    index slice_depth)
  {
    Compound *c = dynamic_cast<Compound*>(node);

    if(c)
      {
	// Move to correct node depth if a slice depth is selected.
	//
	// Only topmost Compound is sliced based upon depth. Slicing
	// based upon depth is ignored if a single node is found at 
	// topmost node level.
	if(slice_depth != 0)
	  {
	    if(slice_depth > c->size())
	      {
		throw EntryTypeMismatch("depth <= layer depth",
					"depth > layer depth");
	      }

	    // Get node from selected depth.
	    // slice_node(..) is called with a slice depth 
	    // of 0 at consecutive calls of the function (to 
	    // avoid further slicing based upon depth).
	    slice_node(subnet, (*c)[slice_depth-1], 0);
	    return;
	  }
	else
	  {
	    for(std::vector<Node*>::iterator it = c->begin();
		it != c->end(); ++it)
	      {
		slice_node(subnet, *it, 0);
	      }
	    return;
	  }
      }
    else
      {
	// Check if we want to select nodes based on modeltype
	if(modeltype_ == -1 || 
	   node->get_model_id() == modeltype_)
	  {
	    // At this point all node criterias are fulfilled
	    // and the node is added to the result subnet.
	    //
	    // The subnet Compound structure only acts as a wrapper
	    // for the candidate node and is not the actual parent 
	    // of the node so we should not use the Compound 
	    // function add_node(..) to add the node pointer 
	    // (push_back() suffice).
	    subnet.push_back(node);
	  }
      }
  }

}
