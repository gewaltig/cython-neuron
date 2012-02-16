/*
 *  layer_regular.cpp
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
  Authors: Kittel Austvoll, Håkon Enger
*/

#include <vector>

#include "nodewrapper.h"
#include "subnet.h"

namespace nest
{

  lockPTR<std::vector<NodeWrapper> >
  NodeWrapper::get_nodewrappers(Node* n, const Position<double_t>& pos, std::vector<double_t> *extent)
  {
    Subnet *subnet = dynamic_cast<Subnet*>(n);
    assert(subnet != 0);
    // Slicing of layer before calling ConnectLayer function
    // assures that the subnet isn't nested.

    lockPTR<std::vector<NodeWrapper> > nodewrappers(new std::vector<NodeWrapper>());    

    // at this point, subnet is a full subnet of proxynodes created by Selector::slice_node()
    // we can thus iterate locally
    assert(subnet->local_size() == subnet->global_size());
    for(std::vector<Node*>::iterator it = subnet->local_begin();
        it != subnet->local_end(); ++it)
      {
	nodewrappers->push_back(NodeWrapper(*it, pos, extent));
      }

    return nodewrappers;
  }

}
