/*
 *  connection_hom_wd.cpp
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

#include "network.h"
#include "dictdatum.h"
#include "connector_model.h"
#include "common_synapse_properties.h"
#include "connection_hom_wd.h"
#include "event.h"

namespace nest
{
  /* CommonPropertiesHomWD */

  CommonPropertiesHomWD::CommonPropertiesHomWD() :
    CommonSynapseProperties(),
    weight_(1.0),
    delay_(Time(Time::ms(1.0)).get_steps())
  {}

  void CommonPropertiesHomWD::get_status(DictionaryDatum & d) const
  {
    CommonSynapseProperties::get_status(d);
    def<double_t>(d, "weight", weight_);
    def<double_t>(d, names::delay, Time(Time::step(delay_)).get_ms());
  }

  void CommonPropertiesHomWD::set_status(const DictionaryDatum & d, ConnectorModel &cm)
  {
    CommonSynapseProperties::set_status(d, cm);

    double_t delay;

    if (updateValue<double_t>(d, names::delay, delay))
      {
	if (!cm.check_delay(delay))
	  throw BadDelay(delay);
	delay_ = Time(Time::ms(delay)).get_steps();
      }

    updateValue<double_t>(d, "weight", weight_);
  }

  /* ConnectionHomWD */

  ConnectionHomWD::ConnectionHomWD()
          : Connection()
  {}

  ConnectionHomWD::ConnectionHomWD(const ConnectionHomWD& c)
          : Connection(c)
  {}

  void ConnectionHomWD::get_status(DictionaryDatum & d) const
  {
    // base class properties, different for individual synapse
    Connection::get_status(d);
  }

  void ConnectionHomWD::initialize_property_arrays(DictionaryDatum & d) const
  {
    Connection::initialize_property_arrays(d);
  }

  /**
   * Append properties of this connection to the given dictionary. If the
   * dictionary is empty, new arrays are created first.
   */
  void ConnectionHomWD::append_properties(DictionaryDatum & d) const
  {
    Connection::append_properties(d);
  }

} // namespace nest
