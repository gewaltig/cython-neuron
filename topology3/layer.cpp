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
#include "grid_layer.h"
#include "topology_names.h"
#include "dictutils.h"
#include "exceptions.h"
#include "network.h"

namespace nest {

  index AbstractLayer::cached_ntree_layer_ = -1;
  index AbstractLayer::cached_vector_layer_ = -1;

  AbstractLayer::~AbstractLayer()
  {
  }

  index AbstractLayer::create_layer(const DictionaryDatum & layer_dict)
  {
    index length = 0;
    const char *layer_model_name = 0;
    std::vector<long_t> element_ids;
    std::string element_name;
    Token element_model;

    const Token& t = layer_dict->lookup(names::elements);
    ArrayDatum* ad = dynamic_cast<ArrayDatum *>(t.datum());

    if (ad) {

      for (Token* tp = ad->begin(); tp != ad->end(); ++tp) {

        element_name = std::string(*tp);
        element_model = net_->get_modeldict().lookup(element_name);

        if ( element_model.empty() )
          throw UnknownModelName(element_name);

        element_ids.push_back(static_cast<long>(element_model));

      }

    } else {

      element_name = getValue<std::string>(layer_dict, names::elements);
      element_model = net_->get_modeldict().lookup(element_name);

      if ( element_model.empty() )
        throw UnknownModelName(element_name);

      element_ids.push_back(static_cast<long>(element_model));

    }

    if (layer_dict->known(names::positions)) {
      if (layer_dict->known(names::rows) or layer_dict->known(names::columns) or layer_dict->known(names::layers))
        throw BadProperty("Can not specify both positions and rows or columns.");

      TokenArray positions = getValue<TokenArray>(layer_dict, names::positions);

      if (positions.size() == 0) {
        throw BadProperty("Empty positions array.");
      }

      std::vector<double_t> pos = getValue<std::vector<double_t> >(positions[0]);
      if (pos.size() == 2)
        layer_model_name = "topology_layer_free";
      else if (pos.size() == 3)
        layer_model_name = "topology_layer_3d";
      else
        throw BadProperty("Positions must have 2 or 3 coordinates.");

      length = positions.size();

    } else if (layer_dict->known(names::columns)) {

      if (not layer_dict->known(names::rows)) {
        throw BadProperty("Both columns and rows must be given.");
      }

      length=getValue<long_t>(layer_dict, names::columns) * getValue<long_t>(layer_dict, names::rows);

      if (layer_dict->known(names::layers)) {
        layer_model_name = "topology_layer_grid_3d";
        length *= getValue<long_t>(layer_dict, names::layers);
      } else {
        layer_model_name = "topology_layer_grid";
      }

    } else {
      throw BadProperty("Unknown layer type.");
    }

    assert(layer_model_name != 0);
    Token layer_model = net_->get_modeldict().lookup(layer_model_name);
    if ( layer_model.empty() )
      throw UnknownModelName(layer_model_name);

    index layer_node = net_->add_node(layer_model);

    // Remember original subnet
    const index cwnode = net_->get_cwn()->get_gid();
					  
    net_->go_to(layer_node);

    // Create layer nodes.
    for(size_t i=0;i<element_ids.size();++i) {
      for(index n=0;n<length;++n) {
        net_->add_node(element_ids[i]);
      }
    }

    // Return to original subnet
    net_->go_to(cwnode);

    //Set layer parameters according to input dictionary.
    AbstractLayer *layer = 
      dynamic_cast<AbstractLayer *>(net_->get_node(layer_node));

    layer->depth_ = element_ids.size();
    layer->set_status(layer_dict);

    return layer_node;
  }

} // namespace nest
