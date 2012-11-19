/*
 *  connection_creator.cpp
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

#include "connection_creator.h"

namespace nest
{

  ConnectionCreator::ConnectionCreator(DictionaryDatum dict):
    allow_autapses_(true),
    allow_multapses_(true),
    source_filter_(),
    target_filter_(),
    number_of_connections_(0),
    mask_(),
    kernel_(),
    synapse_model_(0),
    parameters_(),
    net_(Topology3Module::get_network())
  {
    Name connection_type;

    for(Dictionary::iterator dit = dict->begin(); dit != dict->end(); ++dit) {

      if (dit->first == names::connection_type) {

        connection_type = getValue<std::string>(dit->second);

      } else if (dit->first == names::allow_autapses) {

        allow_autapses_ = getValue<bool>(dit->second);

      } else if (dit->first == names::allow_multapses) {

        allow_multapses_ = getValue<bool>(dit->second);

      } else if (dit->first == names::allow_oversized_mask) {

        allow_oversized_ = getValue<bool>(dit->second);

      } else if (dit->first == names::number_of_connections) {

        number_of_connections_ = getValue<long_t>(dit->second);

      } else if (dit->first == names::mask) {

        mask_ = Topology3Module::create_mask(dit->second);

      } else if (dit->first == names::kernel) {

        kernel_ = Topology3Module::create_parameter(dit->second);

      } else if (dit->first == names::synapse_model) {

        const std::string syn_name = getValue<std::string>(dit->second);

        const Token synmodel =
          net_.get_synapsedict().lookup(syn_name);

        if ( synmodel.empty() )
          throw UnknownSynapseType(syn_name);

        synapse_model_ = static_cast<index>(synmodel);

      } else if (dit->first == names::targets) {

        target_filter_ = getValue<DictionaryDatum>(dit->second);

      } else if (dit->first == names::sources) {

        source_filter_ = getValue<DictionaryDatum>(dit->second);

      } else if (dit->first == names::weights) {

        parameters_[names::weight] = Topology3Module::create_parameter(dit->second);

      } else if (dit->first == names::delays) {

        parameters_[names::delay] = Topology3Module::create_parameter(dit->second);

      } else {

        parameters_[dit->first] = Topology3Module::create_parameter(dit->second);

      }

    }

    if (connection_type==names::convergent) {

      if (number_of_connections_) {
        type_ = Convergent;
      } else {
        type_ = Target_driven;
      }

    } else if (connection_type==names::divergent) {

      if (number_of_connections_) {
        type_ = Divergent;
      } else {
        type_ = Source_driven;
      }

    } else {

      throw BadProperty("Unknown connection type.");

    }

  }

} // namespace nest
