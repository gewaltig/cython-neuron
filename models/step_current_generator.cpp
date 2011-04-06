/*
 *  step_current_generator.cpp
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

#include "step_current_generator.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "dictutils.h"

/* ---------------------------------------------------------------- 
 * Default constructors defining default parameter
 * ---------------------------------------------------------------- */
    
nest::step_current_generator::Parameters_::Parameters_()
  : amp_times_(),  // ms
    amp_values_()  // pA
{}

/* ---------------------------------------------------------------- 
 * Parameter extraction and manipulation functions
 * ---------------------------------------------------------------- */

void nest::step_current_generator::Parameters_::get(DictionaryDatum &d) const
{
  (*d)["amplitude_times"] = DoubleVectorDatum(new std::vector<double_t>(amp_times_));
  (*d)["amplitude_values"] = DoubleVectorDatum(new std::vector<double_t>(amp_values_));
}  

void nest::step_current_generator::Parameters_::set(const DictionaryDatum& d,
                                                    Buffers_& b)
{
  const bool ut = updateValue<std::vector<double_t> >(d, "amplitude_times", amp_times_);
  const bool uv = updateValue<std::vector<double_t> >(d, "amplitude_values", amp_values_);
  
  if ( ut xor uv )
    throw BadProperty("Amplitude times and values must be reset together.");
    
  if ( amp_times_.size() != amp_values_.size() )
    throw BadProperty("Amplitude times and values have to be the same size.");
    
  // ensure amp times are strictly monotonically increasing
  if ( !amp_times_.empty() )
  { 
    std::vector<double_t>::const_iterator prev = amp_times_.begin();
    for ( std::vector<double_t>::const_iterator next = prev + 1;
          next != amp_times_.end() ; ++next, ++prev )
      if ( *prev >= *next )
        throw BadProperty("Amplitude times must strictly increasing.");
  }
  
  if ( ut && uv )
    b.idx_ = 0;  // reset if we got new data
}


/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::step_current_generator::step_current_generator()
  : Node(),
    device_(), 
    P_()
{}

nest::step_current_generator::step_current_generator(const step_current_generator& n)
  : Node(n), 
    device_(n.device_),
    P_(n.P_)
{}


/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::step_current_generator::init_node_(const Node& proto)
{
  const step_current_generator& pr = downcast<step_current_generator>(proto);

  device_.init_parameters(pr.device_);
  
  P_ = pr.P_;
}

void nest::step_current_generator::init_state_(const Node& proto)
{ 
  const step_current_generator& pr = downcast<step_current_generator>(proto);

  device_.init_state(pr.device_);
}

void nest::step_current_generator::init_buffers_()
{ 
  device_.init_buffers();
  
  B_.idx_ = 0;
  B_.amp_ = 0;
}

void nest::step_current_generator::calibrate()
{
  device_.calibrate();
}


/* ---------------------------------------------------------------- 
 * Update function and event hook
 * ---------------------------------------------------------------- */

void nest::step_current_generator::update(Time const &origin, const long_t from, const long_t to)
{
  assert(P_.amp_times_.size() == P_.amp_values_.size());

  const long_t t0 = origin.get_steps();

  // Skip any times in the past. Since we must send events proactively,
  // idx_ must point to times in the future.
  const long_t first = t0 + from;
  while ( B_.idx_ < P_.amp_times_.size() && Time(Time::ms(P_.amp_times_[B_.idx_])).get_steps() <= first )
    ++B_.idx_;

  for ( long_t offs = from ; offs < to ; ++offs )
  {
    const long_t curr_time = t0 + offs;
    
    // Keep the amplitude up-to-date at all times.
    // We need to change the amplitude one step ahead of time, see comment
    // on class SimulatingDevice.
    if ( B_.idx_ < P_.amp_times_.size() && curr_time + 1 == Time(Time::ms(P_.amp_times_[B_.idx_])).get_steps() )
    {
      B_.amp_ = P_.amp_values_[B_.idx_];
      B_.idx_++;
    }
    
    // but send only if active
    if( device_.is_active(Time::step(curr_time)) )
    {
      CurrentEvent ce;
      ce.set_current(B_.amp_);
      network()->send(*this, ce, offs);
    }
  }
}

