/*
 *  layer.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "layer.h"
#include "free_layer.h"
#include "topology_names.h"
#include "dictutils.h"
#include "exceptions.h"
#include "network.h"

namespace nest {

  index AbstractLayer::create_layer(const DictionaryDatum & layer_dict)
  {
    index layer_node;

    if (layer_dict->known(Name("positions"))) {
      TokenArray positions = getValue<TokenArray>(layer_dict, names::positions);

      if (positions.size() == 0) {
        throw BadProperty("Empty positions array.");
      }

      std::vector<double_t> pos = getValue<std::vector<double_t> >(positions[0]);
      Token layer_model;
      if (pos.size() == 2) {

        layer_model = 
          net_->get_modeldict().lookup("topology_layer_free");
        if ( layer_model.empty() )
          throw UnknownModelName("topology_layer_free");

      } else if (pos.size() == 3) {

        layer_model = 
          net_->get_modeldict().lookup("topology_layer_3d");
        if ( layer_model.empty() )
          throw UnknownModelName("topology_layer_3d");

      } else {
        throw BadProperty("Positions must have 2 or 3 coordinates.");
      }

      // FIXME: support arrays of elements
      std::string element_name = getValue<std::string>(layer_dict, names::elements);
      const Token element_model = 
        net_->get_modeldict().lookup(element_name);
      if ( element_model.empty() )
        throw UnknownModelName(element_name);

      long_t element_id = static_cast<long>(element_model);

      layer_node = net_->add_node(layer_model);

      // Remember original subnet
      const index cwnode = net_->get_cwn()->get_gid();
					  
      net_->go_to(layer_node);

      // Create layer nodes.
      for(index n=0;n<positions.size();++n)
      {
	net_->add_node(element_id);
      }

      // Return to original subnet
      net_->go_to(cwnode);

    } else {
      throw BadProperty("Unknown layer type.");
    }

    //Set layer parameters according to input dictionary.
    AbstractLayer *layer = 
      dynamic_cast<AbstractLayer *>(net_->get_node(layer_node));

    layer->set_status(layer_dict);

    return layer_node;
  }


} // namespace nest
