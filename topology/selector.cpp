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

#include "subnet.h"
#include "nestmodule.h"
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
        NestModule::get_network().get_modeldict().lookup(modelname);

    if ( model.empty() )
      throw UnknownModelName(modelname);

    modeltype_ = static_cast<long_t>(model);
  }
}

void Selector::slice_node(Subnet& temp_subnet,
                          Subnet& layer_subnet)
{
  LocalLeafList localnodes(layer_subnet);
  vector<Communicator::NodeAddressingData> global_nodes;
  Communicator::communicate(localnodes, global_nodes);

  for ( vector<Communicator::NodeAddressingData>::iterator n = global_nodes.begin();
      n != global_nodes.end(); ++n)
  {
    const index gid = n->get_gid();
    const index model_id = NestModule::get_network().get_model_id_of_gid(gid);
    if ( modeltype_ == -1 ||  model_id == modeltype_ )
      temp_subnet.add_node(new proxynode(gid,
          n->get_parent_gid(),
          model_id));
  }
}
}
