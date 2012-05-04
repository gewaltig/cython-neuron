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
    number_of_connections(0),mask(),synapse_model(0)
  {

    Name connection_type = getValue<Name>(dict,names::connection_type);

    updateValue<long_t>(dict,names::number_of_connections,number_of_connections);

    if (connection_type==names::convergent) {

      if (number_of_connections) {
        type = Convergent;
      } else {
        type = Source_driven;
      }

    } else {

      throw BadProperty("Unknown connection type.");

    }

    if(dict->known(names::mask)) {

      const Token& t = dict->lookup(names::mask);

      MaskDatum *maskd = dynamic_cast<MaskDatum*>(t.datum());
      if (maskd) {
        mask = *maskd;
      } else {

        DictionaryDatum *dd = dynamic_cast<DictionaryDatum*>(t.datum());
        if (dd) {

          assert((*dd)->size() == 1);  // FIXME: Fail gracefully
          Name n = (*dd)->begin()->first;
          DictionaryDatum mask_dict = getValue<DictionaryDatum>(*dd,n);
          mask = lockPTR<AbstractMask>(Topology3Module::mask_factory().create(n,mask_dict));

        } else {
          throw BadProperty("Mask must be masktype or dictionary.");
        }
      }
    }

    // Get synapse type
    if(dict->known("synapse_model")) {

      const std::string syn_name =
        getValue<std::string>(dict, "synapse_model");

      const Token synmodel =
        Topology3Module::get_network().get_synapsedict().lookup(syn_name);

      if ( synmodel.empty() )
        throw UnknownSynapseType(syn_name);

      synapse_model = static_cast<index>(synmodel);

    }

  }


} // namespace nest
