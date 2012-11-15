/*
 *  selector.cpp
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
  Author: Kittel Austvoll
 */

#include "selector.h"

#include "subnet.h"
#include "dictdatum.h"
#include "nodelist.h"
#include "communicator.h"
#include "communicator_impl.h"


#include "topology_names.h"

namespace nest{

Selector::Selector(const DictionaryDatum& selection_dict):
            modeltype_(-1)
{
  if ( selection_dict->known("lid") )
    throw BadProperty("topology no longer supports depth-based slicing (/lid).");

  // Get information about which node type that should
  // be connected to. All model types will be connected
  // to if no modelname is given.
  std::string modelname;

  if(updateValue<std::string>(selection_dict, names::model, modelname))
  {
    //Get model type.
    const Token model =
        Node::network()->get_modeldict().lookup(modelname);

    if ( model.empty() )
      throw UnknownModelName(modelname);

    modeltype_ = static_cast<long_t>(model);
  }
}

void Selector::slice_node(Subnet& temp_subnet, Node& layer_member)
{
  Subnet* layer_subnet = dynamic_cast<Subnet*>(&layer_member);
  if ( not layer_subnet )
  {
    // member is single node
    if ( modeltype_ == -1 || layer_member.get_model_id() == modeltype_ )
      temp_subnet.add_node(&layer_member);
  }
  else
  {
    // member is subnet
    LocalLeafList localnodes(*layer_subnet);
    vector<Communicator::NodeAddressingData> global_nodes;
    Communicator::communicate(localnodes, global_nodes, true);

    Network& netw = *Node::network();
    for ( vector<Communicator::NodeAddressingData>::iterator n = global_nodes.begin();
          n != global_nodes.end(); ++n)
    {
      const index gid = n->get_gid();
      const index model_id = netw.get_model_id_of_gid(gid);
      if ( modeltype_ == -1 ||  model_id == modeltype_ )
          temp_subnet.add_node(new proxynode(gid, n->get_parent_gid(),
                                             model_id, n->get_vp()));
    }
  }
}

}
