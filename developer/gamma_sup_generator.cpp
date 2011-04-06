/*
 *  gamma_sup_generator.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2011 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 */

#include "gamma_sup_generator.h"
#include "ppd_sup_generator.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "numerics.h"
#include "datum.h"
#include <algorithm>
#include <limits>


/* ---------------------------------------------------------------- 
 * Constructor of internal states class
 * ---------------------------------------------------------------- */
    
nest::gamma_sup_generator::Internal_states_::Internal_states_(size_t num_bins, ulong_t ini_occ_ref, ulong_t ini_occ_act)
{
    occ_.resize(num_bins, ini_occ_ref);
    occ_.back() += ini_occ_act;
    bino_dev_ = new ppd_sup_generator::FastBinomialRandomDev_( ini_occ_act + num_bins*ini_occ_ref );
}

/* ---------------------------------------------------------------- 
 * Propagate internal states one time step and generate spikes
 * ---------------------------------------------------------------- */

nest::ulong_t nest::gamma_sup_generator::Internal_states_::update(double_t transition_prob, librandom::RngPtr rng)
{
    std::vector<ulong_t> n_trans;
    n_trans.resize( occ_.size() );
    
    // go through all states and draw number of transiting components
    for (ulong_t i=0; i<occ_.size(); i++)
        {
        if (occ_[i]>0) 
            {
            poisson_dev_.set_lambda( transition_prob * occ_[i]);
            n_trans[i] = poisson_dev_.uldev(rng);
            if (n_trans[i]>occ_[i])
               n_trans[i] = occ_[i];   
//            bino_dev_->set_p_n( transition_prob, occ_[i]);
//            n_trans[i] = bino_dev_->uldev(rng); 
            }
        else
            n_trans[i] = 0;
        }
    
    // according to above numbers, changes the occupation vector
    for (ulong_t i=0; i<occ_.size(); i++)
        {
        if (n_trans[i]>0) 
            {
            occ_[i] -= n_trans[i];
            if (i==occ_.size()-1)
                occ_.front() += n_trans[i];
            else
                occ_[i+1] += n_trans[i]; 
            }
        }
    return n_trans.back();
}


/* ---------------------------------------------------------------- 
 * Default constructors defining default parameter
 * ---------------------------------------------------------------- */
    
nest::gamma_sup_generator::Parameters_::Parameters_()
  : rate_(0.0),  // Hz
    gamma_shape_(1),
    n_proc_(1),
    num_targets_(0)
{}

/* ---------------------------------------------------------------- 
 * Parameter extraction and manipulation functions
 * ---------------------------------------------------------------- */

void nest::gamma_sup_generator::Parameters_::get(DictionaryDatum &d) const
{
  (*d)[names::rate] = rate_;
  (*d)[names::gamma_shape] = gamma_shape_;
  (*d)[names::n_proc] = n_proc_;
}  

void nest::gamma_sup_generator::Parameters_::set(const DictionaryDatum& d)
{
  updateValue<long_t>(d, names::gamma_shape, gamma_shape_);
  if ( gamma_shape_ < 1 )
    throw BadProperty("The shape must be larger or equal 1");
    
  updateValue<double_t>(d, names::rate, rate_);
  if ( rate_ < 0.0 )
    throw BadProperty("The rate must be larger than 0.");
  
  long n_proc_l = 0;
  updateValue<long_t>(d, names::n_proc, n_proc_l);
  if ( n_proc_l < 1 )
    throw BadProperty("The number of component processes cannot be smaller than one");
  else
    n_proc_ = static_cast<ulong_t> (n_proc_l);
}


/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::gamma_sup_generator::gamma_sup_generator()
  : Node(),
    device_(), 
    P_()
{}

nest::gamma_sup_generator::gamma_sup_generator(const gamma_sup_generator& n)
  : Node(n), 
    device_(n.device_),
    P_(n.P_)
{}


/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::gamma_sup_generator::init_node_(const Node& proto)
{
  const gamma_sup_generator& pr = downcast<gamma_sup_generator>(proto);

  device_.init_parameters(pr.device_);
  
  P_ = pr.P_;
}

void nest::gamma_sup_generator::init_state_(const Node& proto)
{ 
  const gamma_sup_generator& pr = downcast<gamma_sup_generator>(proto);

  device_.init_state(pr.device_);
}

void nest::gamma_sup_generator::init_buffers_()
{ 
  device_.init_buffers();
}

void nest::gamma_sup_generator::calibrate()
{
  device_.calibrate();
  
  double_t h = Time::get_resolution().get_ms();
 
  // transition probability in each time step
  V_.transition_prob_ = P_.rate_ * P_.gamma_shape_ * h / 1000.0;
    
  // approximate equilibrium occupation to initialize to
  ulong_t ini_occ_0 = static_cast<ulong_t> (P_.n_proc_ / P_.gamma_shape_)  ;
  
  // If new targets have been added during a simulation break, we
  // initialize the new elements in Internal_states with the initial dist. The existing
  // elements are unchanged.
  Internal_states_ internal_states0 (P_.gamma_shape_, ini_occ_0, P_.n_proc_ - ini_occ_0 * P_.gamma_shape_);
  B_.internal_states_.resize( P_.num_targets_, internal_states0 );
}



/* ---------------------------------------------------------------- 
 * Update function and event hook
 * ---------------------------------------------------------------- */

void nest::gamma_sup_generator::update(Time const & T, const long_t from, const long_t to)
{
  assert(to >= 0 && (delay) from < Scheduler::get_min_delay());
  assert(from < to);

  if ( P_.rate_ <= 0 || P_.num_targets_ == 0 ) 
    return;

  for ( long_t lag = from ; lag < to ; ++lag )
  {
    Time t = T + Time::step(lag); 
    
    if ( !device_.is_active( t ) )
      continue;  // no spike at this lag
    
    DSSpikeEvent se;
    network()->send(*this, se, lag);
  }
}


void nest::gamma_sup_generator::event_hook(DSSpikeEvent& e)
{

  // get port number
  const port prt = e.get_port();

  // we handle only one port here, get reference to vector elem
  assert(0 <= prt && static_cast<size_t>(prt) < B_.internal_states_.size() );

  // age_distribution object propagates one time step and returns number of spikes
  ulong_t n_spikes = B_.internal_states_[prt].update( V_.transition_prob_, net_->get_rng(get_thread()) );
  
  if ( n_spikes > 0 ) // we must not send events with multiplicity 0
  {
    e.set_multiplicity(n_spikes);
    e.get_receiver().handle(e);
  }


}
