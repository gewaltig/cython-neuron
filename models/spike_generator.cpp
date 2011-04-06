/*
 *  spike_generator.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "spike_generator.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "booldatum.h"
#include "arraydatum.h"
#include "dictutils.h"
#include "exceptions.h"


/* ---------------------------------------------------------------- 
 * Default constructors defining default parameters and state
 * ---------------------------------------------------------------- */

nest::spike_generator::Parameters_::Parameters_()
  : precise_times_(true),
    spike_times_(),
    spike_weights_()
{}

nest::spike_generator::Parameters_::Parameters_(const Parameters_& op)
  :  precise_times_(op.precise_times_),
     spike_times_(op.spike_times_),
     spike_weights_(op.spike_weights_)
{}

nest::spike_generator::State_::State_()
  : position_(0)
{}


/* ---------------------------------------------------------------- 
 * Parameter extraction and manipulation functions
 * ---------------------------------------------------------------- */

void nest::spike_generator::Parameters_::get(DictionaryDatum &d) const
{  
  (*d)[names::spike_times] = DoubleVectorDatum(new std::vector<double_t>(spike_times_));
  (*d)["spike_weights"] = DoubleVectorDatum(new std::vector<double_t>(spike_weights_));
  (*d)["precise_times"] = BoolDatum(precise_times_);
}
 
void nest::spike_generator::Parameters_::set(const DictionaryDatum& d, State_& s)
{
  if ( d->known("precise_times") )
    precise_times_ = getValue<bool>(d->lookup("precise_times"));

  bool updated_spike_times = d->known(names::spike_times);
  
  if(updated_spike_times)
  {
    // First obtain times as double vector, then convert to Time representation
    // Also compute absolute times
    std::vector<double_t> d_times;
    
    d_times = getValue<std::vector<double> >(d->lookup(names::spike_times));

    // ensure that spike times are sorted
    if ( !d_times.empty() )
    { 
      std::vector<double_t>::const_iterator prev = d_times.begin();
      if ( !precise_times_ && !Time(Time::ms(*prev)).is_grid_time() ) {
	std::stringstream msg;
	msg << "spike_generator: Time point " << *prev << " is not representable in current resolution.";
	throw BadProperty(msg.str());
      }

      for ( std::vector<double_t>::const_iterator next = prev + 1;
            next != d_times.end() ; ++next, ++prev )
	{
	  if ( *prev > *next )
	    throw BadProperty("Spike times must be sorted in non-descending order.");

	  if ( !precise_times_ && !Time(Time::ms(*next)).is_grid_time() ) {
	    std::stringstream msg;
	    msg << "spike_generator: Time point " << *next << " is not representable in current resolution.";
	    throw BadProperty(msg.str());
	  }
	}
    }
    
    // Now copy to vector of Time entries
    spike_times_ = d_times;

  }

  // spike_weights can be the same size as spike_times, or can be of size 0 to
  // only use the spike_times array
  bool updated_spike_weights = d->known("spike_weights");
  if (updated_spike_weights)
  {
    std::vector<double> spike_weights = getValue<std::vector<double> >(d->lookup("spike_weights"));
    
    if (spike_weights.empty())
      spike_weights_.clear();
    else
    {
      if (spike_weights.size() != spike_times_.size())
        throw BadProperty("spike_weights must have the same number of elements as spike_times,"
                          " or 0 elements to clear the property.");

      spike_weights_.swap(spike_weights);
    }
  }

  // Set position to start if something changed
  if (updated_spike_times || updated_spike_weights || d->known(names::origin))
    s.position_ = 0;
}


/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::spike_generator::spike_generator()
  : Node(),
    device_(), 
    P_(),
    S_()
{}

nest::spike_generator::spike_generator(const spike_generator& n)
  : Node(n), 
    device_(n.device_),
    P_(n.P_),
    S_(n.S_)
{}


/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::spike_generator::init_node_(const Node& proto)
{
  const spike_generator& pr = downcast<spike_generator>(proto);

  device_.init_parameters(pr.device_);

  P_ = Parameters_(pr.P_);   // force recalibration of Time objects
  S_ = pr.S_;
}

void nest::spike_generator::init_state_(const Node& proto)
{ 
  const spike_generator& pr = downcast<spike_generator>(proto);

  device_.init_state(pr.device_);
  S_ = pr.S_;
}

void nest::spike_generator::init_buffers_()
{ 
  device_.init_buffers();
}

void nest::spike_generator::calibrate()
{
  device_.calibrate();
}


/* ---------------------------------------------------------------- 
 * Other functions
 * ---------------------------------------------------------------- */

void nest::spike_generator::update(Time const & sliceT0, const long_t from, const long_t to)
{
  if ( P_.spike_times_.empty() )
    return;

  const Time  tstart = sliceT0 + Time::step(from);
  const Time  tstop  = sliceT0 + Time::step(to);
  const Time& origin = device_.get_origin();
  
  // We fire all spikes with time stamps up to including sliceT0 + to
  while ( S_.position_ < P_.spike_times_.size() )
  {
    double Dtnext = P_.spike_times_[S_.position_];

    Time Ttnext;
    double offset;

    if (P_.precise_times_)
      {
	Ttnext = Time::ms_stamp(Dtnext);
	offset = Ttnext.get_ms() - Dtnext;
      }
    else
      {
	Ttnext = Time(Time::ms(Dtnext));
	offset = 0.;
      }

    // origin is always rounded to time grid, so just add to steps
    Ttnext += origin; 

    // skip all spikes that are in the past
    // this might happen due to wrong usage of the generator
    if ( Ttnext <= tstart ) {
      ++S_.position_;
      continue;
    }

    if ( Ttnext > tstop )
      break;

    if ( device_.is_active(Ttnext) )
    {
      SpikeEvent* se;

      // if we have to deliver weighted spikes, we need to get the
      // event back to set its weight according to the entry in
      // spike_weights_, so we use a DSSpike event and event_hook()
      if (P_.spike_weights_.size() != 0)
        se = new DSSpikeEvent;
      else
	se = new SpikeEvent;

      se->set_offset(offset);
      
      // we need to subtract one from stamp which is added again in send()
      long_t lag = Time(Ttnext - sliceT0).get_steps() - 1;
      
      // all spikes are sent locally, so offset information is always preserved
      network()->send(*this, *se, lag);
      delete se;
    }

    ++S_.position_;
  }
}

void nest::spike_generator::event_hook(DSSpikeEvent& e)
{
  e.set_weight(P_.spike_weights_[S_.position_] * e.get_weight());
  e.get_receiver().handle(e);
}
