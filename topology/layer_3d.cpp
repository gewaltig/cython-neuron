/*
 *  layer_3d.cpp
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

#include "layer_3d.h"

#include "layer_unrestricted.h"

#include <limits>

#include "tokenarray.h"
#include "exceptions.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "dictutils.h"
#include "numerics.h"
#include "dictutils.h"

#include "quadtree.h"
#include "octtree.h"
#include "topology_names.h"
#include "nodewrapper.h"
#include "region.h"
#include "topologyconnector.h"
#include "layer_slice.h"

namespace nest
{

  Layer3D::Layer3D():
    LayerUnrestricted(),
    tree_()
  {
    
  }

  Layer3D::Layer3D(const Layer3D& l):
    LayerUnrestricted(l),
    tree_(l.tree_)
  {
  }

  Layer3D::Layer3D(const Layer3D& l, 
		   const std::vector<Node*>& nodes):
    LayerUnrestricted(l),
    tree_(l.tree_)
  {
    nodes_ = nodes;
  }

  lockPTR<Layer> Layer3D::slice(bool, const DictionaryDatum& options) const
  {
    return lockPTR<Layer>(new LayerSlice<Layer3D>(*this, options));
  }

  void Layer3D::set_tree_settings(const DictionaryDatum&)
  {

  }

  void Layer3D::get_tree_settings(DictionaryDatum&) const
  {

  }

  void Layer3D::make_tree()
  {
    tree_ = Octtree(nodes_, positions_, 
		    Position<double_t>(center_.at(0) - extent_.at(0)/2,
					     center_.at(1) - extent_.at(1)/2, 
					     center_.at(2) - extent_.at(2)/2),
		    Position<double_t>(center_.at(0) + extent_.at(0)/2, 
					     center_.at(1) + extent_.at(1)/2,
					     center_.at(2) + extent_.at(2)/2), 
		    100);
  }

  lockPTR<std::vector<NodeWrapper> > Layer3D::
  get_pool_nodewrappers(const Position<double_t>& driver_coo,
			AbstractRegion* region)
  {
    Volume* v = dynamic_cast<Volume*>(region);
    
    if(v != NULL)
      {
	v = v->copy();
	v->set_anchor(driver_coo);
	
	return tree_.get_nodewrappers(v);
      }
    else
      {
	throw EntryTypeMismatch("3D region", 
				"fixed grid region");
      }
  }

}
