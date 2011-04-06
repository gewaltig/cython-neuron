
/*
 *  common_synapse_properties.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */


#include "nest.h"
#include "dictdatum.h"
#include "common_synapse_properties.h"
#include "connector_model.h"
#include "nest_timeconverter.h"
#include "node.h"

namespace nest {

/**
 * Default implementation of an empty CommonSynapseProperties object.
 */

  CommonSynapseProperties::CommonSynapseProperties() {}

  CommonSynapseProperties::~CommonSynapseProperties() {}

  void CommonSynapseProperties::get_status(DictionaryDatum &) const {}
  
  void CommonSynapseProperties::set_status(const DictionaryDatum &, ConnectorModel&) {}
  
  Node* CommonSynapseProperties::get_node() {return 0;}
 

  void CommonSynapseProperties::calibrate(const TimeConverter &) {}
 
} // namespace nest
