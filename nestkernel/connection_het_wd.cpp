/*
 *  connection_het_wd.cpp
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

#include "common_synapse_properties.h"
#include "connection_het_wd.h"
#include "connector_model.h"
#include "network.h"

namespace nest
{

ConnectionHetWD::ConnectionHetWD()
        : Connection(),
          weight_(1.0),
          delay_(Time(Time::ms(1.0)).get_steps())
{}

ConnectionHetWD::ConnectionHetWD(const ConnectionHetWD& c)
        : Connection(c),
          weight_(c.weight_),
          delay_(c.delay_)
{}

void ConnectionHetWD::get_status(DictionaryDatum & d) const
{
  Connection::get_status(d);
  def<double_t>(d, names::weight, weight_);
  def<double_t>(d, names::delay, Time(Time::step(delay_)).get_ms());
}

void ConnectionHetWD::set_status(const DictionaryDatum & d, ConnectorModel& cm)
{
  double_t delay;

  if (updateValue<double_t>(d, names::delay, delay))
  {
    if (!cm.check_delay(delay))
      throw BadDelay(delay);
    delay_ = Time(Time::ms(delay)).get_steps();
  }
  updateValue<double_t>(d, names::weight, weight_);
}

void ConnectionHetWD::set_status(const DictionaryDatum & d, index p, ConnectorModel& cm)
{
  // we need to check delays first, so the exception can be thrown before
  // any other parameter has been set.
  double_t delay;

  if ( set_property<double_t>(d, names::delays, p, delay) )
  {
    if (!cm.check_delay(delay))
      throw BadDelay(delay);
    delay_ = Time(Time::ms(delay)).get_steps();
  }
  set_property<double_t>(d, names::weights, p, weight_);
}

void ConnectionHetWD::initialize_property_arrays(DictionaryDatum & d) const
{
  Connection::initialize_property_arrays(d);
  initialize_property_array(d, names::weights);
  initialize_property_array(d, names::delays);
}

void ConnectionHetWD::append_properties(DictionaryDatum & d) const
{
  Connection::append_properties(d);
  append_property<double_t>(d, names::weights, weight_);
  append_property<double_t>(d, names::delays, Time(Time::step(delay_)).get_ms());
}

} // namespace nest
