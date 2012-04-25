/*
 *  mirollo_strogatz_ps.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "mirollo_strogatz_ps.h"

#include "exceptions.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "dictutils.h"
#include "numerics.h"
#include "universal_data_logger_impl.h"
#include <limits>
#include <float.h>

#include <iomanip>
#include <iostream>
#include <cstdio>

/* ---------------------------------------------------------------- 
 * Recordables map
 * ---------------------------------------------------------------- */

nest::RecordablesMap<nest::mirollo_strogatz_ps> nest::mirollo_strogatz_ps::recordablesMap_;

namespace nest // template specialization must be placed in namespace
{
  // Override the create() method with one call to
  // RecordablesMap::insert_() for each quantity to be recorded.
  template <>
  void RecordablesMap<mirollo_strogatz_ps>::create()
  {
    // Use standard names whereever you can for consistency!
    insert_(names::phi,
	    &mirollo_strogatz_ps::get_phi_);
  }
}

/* ---------------------------------------------------------------- 
 * Default constructors defining default parameters and state
 * ---------------------------------------------------------------- */
    
nest::mirollo_strogatz_ps::Parameters_::Parameters_()
  : phi_th(1.0),
    I(4.0)
{
  gamma = std::log(I);
  recordablesMap_.create();
}

nest::mirollo_strogatz_ps::State_::State_(const Parameters_ &p)
  : phi(0.0)
{
}

nest::mirollo_strogatz_ps::State_::State_(const State_ &s)
  : phi(s.phi)
{
}

nest::mirollo_strogatz_ps::State_& nest::mirollo_strogatz_ps::State_::operator=(const State_ &s)
{
  if ( this == &s ) // avoid assignment to self
    return *this;

  phi = s.phi;
  return *this;
}

nest::mirollo_strogatz_ps::Buffers_::Buffers_(mirollo_strogatz_ps &n)
  : logger_(n)
{
  // The other member variables are left uninitialised or are
  // automatically initialised by their default constructor.
}

nest::mirollo_strogatz_ps::Buffers_::Buffers_(const Buffers_ &, mirollo_strogatz_ps &n)
  : logger_(n)
{
  // The other member variables are left uninitialised or are
  // automatically initialised by their default constructor.
}

/* ---------------------------------------------------------------- 
 * Parameter and state extractions and manipulation functions
 * ---------------------------------------------------------------- */

void nest::mirollo_strogatz_ps::Parameters_::get(DictionaryDatum &d) const
{
  def<double>(d, names::phi_th, phi_th);
  def<double>(d, names::I, I);
  def<double>(d, names::gamma, gamma);
}

void nest::mirollo_strogatz_ps::Parameters_::set(const DictionaryDatum& d)
{
  updateValue<double>(d, names::phi_th, phi_th);
  if ( updateValue<double>(d, names::I, I) )
    gamma = std::log(I);
}

void nest::mirollo_strogatz_ps::State_::get(DictionaryDatum &d) const
{
  def<double>(d, names::phi, phi);
}

void nest::mirollo_strogatz_ps::State_::set(const DictionaryDatum& d, const Parameters_&)
{
  updateValue<double>(d, names::phi, phi);
}

/* ---------------------------------------------------------------- 
 * Default and copy constructor for node, and destructor
 * ---------------------------------------------------------------- */

nest::mirollo_strogatz_ps::mirollo_strogatz_ps()
  : Archiving_Node(), 
    P_(), 
    S_(P_),
    B_(*this)
{
}

nest::mirollo_strogatz_ps::mirollo_strogatz_ps(const mirollo_strogatz_ps& n)
  : Archiving_Node(n), 
    P_(n.P_), 
    S_(n.S_),
    B_(n.B_, *this)
{
}

nest::mirollo_strogatz_ps::~mirollo_strogatz_ps()
{
}

/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::mirollo_strogatz_ps::init_state_(const Node &proto)
{
  const mirollo_strogatz_ps &pr = downcast<mirollo_strogatz_ps>(proto);

  S_ = pr.S_;
}

void nest::mirollo_strogatz_ps::init_buffers_()
{
  Archiving_Node::clear_history();

  B_.events_.resize();
  B_.events_.clear();
  B_.currents_.clear(); // includes resize

  B_.logger_.reset();
}

void nest::mirollo_strogatz_ps::calibrate()
{
  B_.logger_.init();  // ensures initialization in case mm connected after Simulate
  V_.h_ms = Time::get_resolution().get_ms();
  V_.I    = P_.I;
}

/* ---------------------------------------------------------------- 
 * Update and spike handling functions
 * ---------------------------------------------------------------- */

void nest::mirollo_strogatz_ps::update(Time const &origin, long_t const from, long_t const to)
{
  assert ( to >= 0 && (delay) from < Scheduler::get_min_delay() );
  assert ( from < to );

  // at start of slice, tell input queue to prepare for delivery
  if ( from == 0 )
    B_.events_.prepare_delivery();

  for ( long_t lag = from; lag < to; ++lag )
  {
    // time at start of update step
    const long_t T = origin.get_steps() + lag;
    // time at end of h step
    double_t t_end = Time(Time::step(T+1)).get_ms();

    // event info
    double_t t_event;
    double_t ev_offset;
    double_t ev_weight;
    bool     end_of_refract;

    while ( B_.events_.get_next_spike(T, ev_offset, ev_weight, end_of_refract) )
    { // there are incoming spikes
      // printf("t_ev = %.3f\tw=%.3f\n",Time(Time::step(T+1)).get_ms()-ev_offset,ev_weight);
      t_event = t_end - ev_offset;
      while (S_.t_spike < t_event)
	generate_spike(T,lag,t_end);
      if ( ev_weight != 0.0 )
	{
	  // propagate phi
	  S_.phi += t_event-S_.t_last;
	  // integrate spike input
	  // transfer function: f(phi) = I*(1-e^(-gamma*phi)), g(x) = gamma^-1*ln(I/(I-(f(phi)+w)))
	  double_t const U = P_.I*(1-std::exp(-P_.gamma*S_.phi));
	  S_.phi += std::log(P_.I/(P_.I-U-ev_weight));
	  S_.t_last=t_event;
	  if (S_.phi >= P_.phi_th)
	    generate_spike(T,lag,t_end);
	}
    }
    while (S_.t_spike < t_end)
      generate_spike(T,lag,t_end);

    // log state data
    B_.logger_.record_data(T);

    // set new input current
    V_.I = P_.I+B_.currents_.get_value(lag);
  }
}


void nest::mirollo_strogatz_ps::handle(SpikeEvent &e)
{
  assert ( e.get_delay() > 0 );

  // We need to compute the absolute time stamp of the delivery time
  // of the spike, since spikes might spend longer than min_delay_
  // in the queue.  The time is computed according to Time Memo, Rule 3.
  const long_t Tdeliver = e.get_stamp().get_steps() + e.get_delay() - 1;

  B_.events_.add_spike(e.get_rel_delivery_steps(network()->get_slice_origin()), 
		       Tdeliver, e.get_offset(), e.get_weight() * e.get_multiplicity());
}

void nest::mirollo_strogatz_ps::handle(CurrentEvent &e)
{
  assert ( e.get_delay() > 0 );

  // add weighted current; HEP 2002-10-04
  B_.currents_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()), 
			 e.get_weight() * e.get_current());
}

void nest::mirollo_strogatz_ps::handle(DataLoggingRequest &e)
{
  B_.logger_.handle(e);
}
