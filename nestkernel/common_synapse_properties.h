/*
 *  common_synapse_properties.h
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

#ifndef COMMON_SYNAPSE_PROPERTIES_H
#define COMMON_SYNAPSE_PROPERTIES_H

#include "nest.h"
#include "dictdatum.h"
#include "node.h"
#include "connector_model.h"

namespace nest {

// forward declarations
class ConnectorModel;
class TimeConverter;

/**
 * Class containing the common properties for all connections of a certain type.
 * Everything that needs to be stored commonly for all synapses goes into a CommonProperty
 * class derived by this base class.
 * Base class for all CommonProperty classes.
 * If the synapse type does not have any common properties, this class may be used as a placeholder.
 */
class CommonSynapseProperties
{
 public:
  
  /**
   * Standard constructor. Sets all common properties to default values.
   */
  CommonSynapseProperties();
 
  /**
   * Destructor.
   */
  ~CommonSynapseProperties();
 
  /**
   * Get all properties and put them into a dictionary.
   */
  void get_status(DictionaryDatum & d) const;
  
  /**
   * Set properties from the values given in dictionary.
   */
  void set_status(const DictionaryDatum & d, ConnectorModel& cm);

  
 
  /**
   * Calibrate all time objects, which might be contained in this object.
   */

  void calibrate(const TimeConverter &);

  /**
   * get reference to registering node
   */
  Node * get_node();

};

} // of namespace nest

#endif
