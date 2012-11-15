/*
 *  layer.cpp
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
  This file is part of the NEST topology module.
  Authors: Kittel Austvoll, Håkon Enger
 */

#include "dictutils.h"
#include "layer.h"
#include "selector.h"
#include "topology_names.h"
#include "communicator.h"
#include "communicator_impl.h"
#include "network.h"

namespace nest
{
Layer::Layer(int dim):
        extent_(dim, 1.0),
        center_(dim, 0.0),
        upper_left_(),
        depth_(0),
        EDGE_WRAP_(false)
{
}

Layer::Layer(const Layer& l):
        Subnet(l),
        extent_(l.extent_),
        center_(l.center_),
        upper_left_(l.upper_left_),
        depth_(l.depth_),
        EDGE_WRAP_(l.EDGE_WRAP_)
{
}

Layer::~Layer()
{
}

std::vector<Node*>
Layer::slice_layer(const DictionaryDatum& layer_connection_dict) const
{
  // Create Selector object used to pick desired nodes from layer.
  Selector selector(layer_connection_dict);
  std::vector<Node*> result;

  // iterate over the top level (layer level) and put all nodes at
  // one layer location into a subnet, then collect these subnets
  // in the results vector
  for(std::vector<Node*>::const_iterator it=local_begin(); it != local_end(); ++it)
  {
    Subnet* loc_subnet = dynamic_cast<Subnet*>(*it);
    assert(loc_subnet);
    Subnet* dest_subnet = new Subnet;
    selector.slice_node(*dest_subnet, *loc_subnet);
    result.push_back(dest_subnet);
  }

  return result;
}

void Layer::set_status(const DictionaryDatum& layer_dict)
{
  // Layer properties can either be passed inside a sub topology
  // dictionary or on the upper level of a dictionary
  DictionaryDatum layer_dictionary = new Dictionary();
  if(!updateValue<DictionaryDatum>(layer_dict, names::topology,
      layer_dictionary))
  {
    layer_dictionary = layer_dict;
  }

  // Properties that are not set by the user
  // here takes on the default values.
  updateValue<long_t>(layer_dictionary, names::depth, depth_);

  updateValue<std::vector<double_t> >(layer_dictionary,
      "extent", extent_);

  updateValue<std::vector<double_t> >(layer_dictionary,
      "center", center_);

  if(extent_.size() < 2 || center_.size() < 2)
  {
    throw TypeMismatch("extent and center with at least 2 elements",
        "extent and center with less than 2 elements");
  }

  if(extent_.size() != center_.size())
  {
    throw DimensionalityMismatch("extent and size");
  }

  for(std::vector<double_t>::iterator it = extent_.begin();
      it != extent_.end(); ++it)
  {
    if(*it < 0)
    {
      throw EntryTypeMismatch("extent >= 0", "extent < 0");
    }
  }

  //Derived values.

  switch(center_.size()) {
  case 2:
    upper_left_ = Position<double_t>(center_.at(0) - extent_.at(0)/2,
        center_.at(1) + extent_.at(1)/2);
    break;
  case 3:
    upper_left_ = Position<double_t>(center_.at(0) - extent_.at(0)/2,
        center_.at(1) + extent_.at(1)/2,
        center_.at(2) + extent_.at(2)/2);
    break;
  default:
    assert(0);
  }

  updateValue<bool>(layer_dictionary, names::edge_wrap, EDGE_WRAP_);

  Subnet::set_status(layer_dictionary);
}


void Layer::get_status(DictionaryDatum& d) const
{
  DictionaryDatum dict(new Dictionary);

  (*dict)[names::depth] = depth_;
  (*dict)[names::extent] = extent_;
  (*dict)[names::center] = center_;
  (*dict)[names::edge_wrap] = EDGE_WRAP_;

  (*d)[names::topology] = dict;
  Subnet::get_status(d);
}

Position<double_t> Layer::get_upper_left() const
{
  return upper_left_;
}

const Position<double_t>
Layer::get_position(index gid, index parent_gid)
{
  Network& netw = *Node::network();
  assert(netw.is_local_gid(parent_gid)); // must be local as it is subnet
  Node* parent = netw.get_node(parent_gid);

  if(parent == 0) //root
  {
    throw LayerExpected();
  }

  Layer* const layer = dynamic_cast<Layer*>(parent);

  if ( layer == 0 )
  {
    // Move upwards in subnet structure if node
    // isn't a direct member of a layer.
    return get_position(parent_gid, parent->get_parent()->get_gid());
  }

  Node * child = netw.get_node(gid);
  assert(child);
  return layer->get_position(child->get_lid());
}

Layer* Layer::get_layer(const Node& child)
{
  Node* parent = child.get_parent();

  if(parent == 0) //root
  {
    // Return 0 if the node is not
    // a member of any layer.
    return 0;
  }

  Layer* const layer = dynamic_cast<Layer*>(parent);

  if(layer == 0)
  {
    // Move upwards in subnet structure if node
    // isn't a direct member of a layer.
    return get_layer(*(child.get_parent()));
  }

  return layer;
}

std::vector<Node*> Layer::get_nodes(Node* n)
{
  Subnet *subnet = dynamic_cast<Subnet*>(n);
  assert(subnet != 0);
  // Slicing of layer before calling ConnectLayer function
  // assures that the subnet isn't nested.

  // this is a pseudo-subnet created by slicing, it must be dense
  assert(subnet->global_size() == subnet->local_size());

  return std::vector<Node*>(subnet->local_begin(), subnet->local_end());
}

void Layer::dump_nodes(std::ostream* out)
{
  LocalLeafList localnodes(*dynamic_cast<Subnet*>(this));
  vector<Communicator::NodeAddressingData> global_nodes;
  Communicator::communicate(localnodes, global_nodes, true);

  // now only Rank 0 writes, the others were along for communicating only
  if ( Communicator::get_rank() > 0  || not out )
    return;

  for ( vector<Communicator::NodeAddressingData>::iterator n = global_nodes.begin();
      n != global_nodes.end(); ++n)
  {
    const index gid = n->get_gid();
    *out << gid << ' ';
    Layer::get_position(gid, n->get_parent_gid()).print(*out);
    *out << '\n';
  }
}

bool Layer::edge_wrap_is_set() const
{
  return EDGE_WRAP_;
}

const std::vector<double_t>& Layer::get_extent() const
{
  return extent_;
}

bool Layer::allow_entry() const
{
  return false;
}

} // namespace nest
