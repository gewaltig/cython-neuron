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
#include "compound.h"

namespace nest
{

  lockPTR<std::vector<NodeWrapper> >
  NodeWrapper::get_nodewrappers(Node* n, const Position<double_t>& pos, std::vector<double_t> *extent)
  {
    Compound *subnet = dynamic_cast<Compound*>(n);
    assert(subnet != 0);
    // Slicing of layer before calling ConnectLayer function
    // assures that the subnet isn't nested.

    lockPTR<std::vector<NodeWrapper> > nodewrappers(new std::vector<NodeWrapper>());    

    for(std::vector<Node*>::iterator it = subnet->begin();
	it != subnet->end(); ++it)
      {
	nodewrappers->push_back(NodeWrapper(*it, pos, extent));
      }

    return nodewrappers;
  }

}
