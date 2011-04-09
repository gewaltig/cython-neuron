/*
 *  ac_gamma_generator.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2010 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "ac_gamma_generator.h"

#ifdef HAVE_GSL

#include "exceptions.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "arraydatum.h"
#include "dictutils.h"
#include "numerics.h"
#include "universal_data_logger_impl.h"

#include <cmath>
#include <limits>

#include <gsl/gsl_sf_gamma.h>

namespace nest {
  RecordablesMap<ac_gamma_generator> ac_gamma_generator::recordablesMap_;

  template <>
  void RecordablesMap<ac_gamma_generator>::create()
  {
    insert_(Name("Rate"), &ac_gamma_generator::get_rate_);
  }
}


nest::ac_gamma_generator::Parameters_::Parameters_()
  :  om_(0.0),
     phi_(0.0),
     order_(1.0),
     dc_(0.0),
     ac_(0.0)
{}

nest::ac_gamma_generator::Parameters_::Parameters_(const Parameters_& p)
  :  om_(p.om_),
     phi_(p.phi_),
     order_(p.order_),
     dc_(p.dc_),
     ac_(p.ac_)
{}

nest::ac_gamma_generator::Parameters_&
nest::ac_gamma_generator::Parameters_::operator=(const Parameters_& p)
{
  if ( this == &p )
    return *this;

  om_ = p.om_;
  phi_ = p.phi_;
  order_ = p.order_;
  dc_ = p.dc_;
  ac_ = p.ac_;

  return *this;
}

nest::ac_gamma_generator::State_::State_()
  : rate_(0),
    last_spike_(Time::step(-1))
{}


nest::ac_gamma_generator::Buffers_::Buffers_(ac_gamma_generator& n)
  : logger_(n)
{}

nest::ac_gamma_generator::Buffers_::Buffers_(const Buffers_&, ac_gamma_generator& n)
  : logger_(n)
{}

/* ----------------------------------------------------------------
 * Parameter extraction and manipulation functions
 * ---------------------------------------------------------------- */

void nest::ac_gamma_generator::Parameters_::get(DictionaryDatum &d) const
{
  (*d)["freq"]= om_ / ( 2.0 * numerics::pi / 1000.0);
  (*d)["phi"] = phi_;
  (*d)["order"] = order_;
  (*d)["dc"]  = dc_ * 1000.0;
  (*d)["ac"]  = ac_ * 1000.0;
}

void nest::ac_gamma_generator::State_::get(DictionaryDatum &d) const
{
  (*d)["last_spike"] = last_spike_.get_ms();
}

void nest::ac_gamma_generator::Parameters_::set(const DictionaryDatum& d)
{
  if ( updateValue<double_t>(d, "freq", om_) )
    om_ *= 2.0 * numerics::pi / 1000.0;

  updateValue<double_t>(d, "phi", phi_);

  if ( updateValue<double_t>(d, "order", order_) )
  {
	  if ( order_ < 1.0 )
	  {
	    throw KernelException("ac_gamma_generator::get_status: The gamma order must be at least 1.");
	  }
  }

  if ( updateValue<double_t>(d, "dc", dc_) )
    dc_ /= 1000.0;           // scale to ms^-1

  if ( updateValue<double_t>(d, "ac", ac_) )
      ac_ /= 1000.0;

  if ( dc_ < 0.0 || dc_ < ac_ )
  {
    throw KernelException("ac_gamma_generator::set_status: Amplitudes must fulfill ac_ >= dc_ >= 0.");
  }
}


/* ----------------------------------------------------------------
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::ac_gamma_generator::ac_gamma_generator()
  : Node(),
    device_(),
    P_(),
    S_(),
    B_(*this)
{
    recordablesMap_.create();
}

nest::ac_gamma_generator::ac_gamma_generator(const ac_gamma_generator&n)
  : Node(n),
    device_(),
    P_(n.P_),
    S_(n.S_),
    B_(n.B_, *this)
{
}

/* ----------------------------------------------------------------
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::ac_gamma_generator::init_node_(const Node& proto)
{
  const ac_gamma_generator& pr = downcast<ac_gamma_generator>(proto);

  device_.init_parameters(pr.device_);

  P_ = pr.P_;
  S_ = pr.S_;
}

void nest::ac_gamma_generator::init_state_(const Node& proto)
{
  const ac_gamma_generator& pr = downcast<ac_gamma_generator>(proto);

  device_.init_state(pr.device_);
  S_ = pr.S_;
}

void nest::ac_gamma_generator::init_buffers_()
{
  device_.init_buffers();
  B_.logger_.reset();

  // thread number cannot change once nodes are created, so we can
  // place this check here.
  //  if ( network()->get_num_threads() > 1 || network()->get_num_processes() > 1 ) {
  //    throw BadProperty("ac_gamma_generator is presently not suitable" +
  //                       " for parallel simulation.");
}

// ----------------------------------------------------

void nest::ac_gamma_generator::calibrate()
{
  B_.logger_.init();  // ensures initialization in case mm connected after Simulate
  device_.calibrate();
}


void nest::ac_gamma_generator::update(Time const& origin, 
				      const long_t from, const long_t to)
{
  assert(to >= 0 && (delay) from < Scheduler::get_min_delay());
  assert(from < to);

  const long_t start = origin.get_steps();

  // time resolution in seconds
  const double h = Time::get_resolution().get_ms();

  // random number generator
  librandom::RngPtr rng = net_->get_rng(get_thread());

  for ( long_t lag = from ; lag < to ; ++lag )
  {
    // times in seconds
    const double_t t = Time(Time::step(start+lag)).get_ms();
    const double_t tl= (S_.last_spike_-Time::step(1)).get_ms();

    // compute hazard---this should be done more efficiently
    const double_t lambda = P_.dc_ + P_.ac_ * std::sin(P_.om_ * t + P_.phi_);
    const double_t Lambda = std::abs(P_.ac_) > 0 && std::abs(P_.om_) > 0
    		? P_.order_ * ( P_.dc_ * (t-tl) - P_.ac_ / P_.om_
    			  * ( std::cos(P_.om_ * t + P_.phi_) - std::cos(P_.om_ * tl + P_.phi_) ))
    		: P_.order_ * P_.dc_ * (t-tl);

    const double_t haz = P_.order_ * lambda * std::pow(Lambda, P_.order_-1)
    		* std::exp(-Lambda) / gsl_sf_gamma_inc(P_.order_, Lambda);

    if ( S_.rate_ < 0 )
      S_.rate_ = 0;

    S_.rate_ = lambda;

    // store rate in Hz
    B_.logger_.record_data(origin.get_steps()+lag);

    // create spikes
    if ( rng->drand() < haz * h && device_.is_active(Time::step(start+lag)) )
    {
      SpikeEvent se;
      network()->send(*this, se, lag);
      S_.last_spike_ = Time::step(origin.get_steps()+lag+1);
    }
  }
}                       

void nest::ac_gamma_generator::handle(DataLoggingRequest& e)
{
  B_.logger_.handle(e);
}

#endif //HAVE_GSL
