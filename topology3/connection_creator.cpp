/*
 *  connection_creator.cpp
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

#include "connection_creator.h"

namespace nest
{

  ConnectionCreator::ConnectionCreator(DictionaryDatum dict):
    number_of_connections_(0),mask_(),synapse_model_(0),net_(Topology3Module::get_network())
  {
    Name connection_type;

    for(Dictionary::iterator dit = dict->begin(); dit != dict->end(); ++dit) {

      if (dit->first == names::connection_type) {

        connection_type = getValue<Name>(dit->second);

      } else if (dit->first == names::number_of_connections) {

        number_of_connections_ = getValue<long_t>(dit->second);

      } else if (dit->first == names::mask) {

        mask_ = Topology3Module::create_mask(dit->second);

      } else if (dit->first == names::synapse_model) {

        const std::string syn_name = getValue<std::string>(dit->second);

        const Token synmodel =
          net_.get_synapsedict().lookup(syn_name);

        if ( synmodel.empty() )
          throw UnknownSynapseType(syn_name);

        synapse_model_ = static_cast<index>(synmodel);

      } else {

        parameters_[dit->first] = Topology3Module::create_parameter(dit->second);

      }

    }

    if (connection_type==names::convergent) {

      if (number_of_connections_) {
        type_ = Convergent;
      } else {
        type_ = Source_driven;
      }

    } else {

      throw BadProperty("Unknown connection type.");

    }

  }


} // namespace nest
