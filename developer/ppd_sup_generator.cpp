/*
 *  ppd_sup_generator.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 */

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
 * Draw a binomial random number using the BP algoritm
 * Sampling From the Binomial Distribution on a Computer
 * Author(s): George S. Fishman
 * Source: Journal of the American Statistical Association, Vol. 74, No. 366 (Jun., 1979), pp. 418-423
 * Published by: American Statistical Association
 * Stable URL: http://www.jstor.org/stable/2286346 .
 * ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- 
 * Constructor of FastBinomialRandomDev class
 * ---------------------------------------------------------------- */
    
nest::ppd_sup_generator::FastBinomialRandomDev_::FastBinomialRandomDev_(size_t nmax)
{
    // precompute the table of f
    f_.resize( nmax+2 );
    f_[0] = 0.0;
    f_[1] = 0.0;    
    ulong_t i, j;
    i = 1;
    while (i < f_.size()-1){
        f_[i+1] = 0.0;
        j = 1;
        while (j<=i){
            f_[i+1] += std::log(j);
            j++;
            }
        i++;
        }
}


void nest::ppd_sup_generator::FastBinomialRandomDev_::set_p_n(double_t p, ulong_t n)
{
    n_ = n;
    p_ = p;
}

nest::ulong_t nest::ppd_sup_generator::FastBinomialRandomDev_::uldev(librandom::RngPtr rng)
{
    // BP algorithm 
    ulong_t  X_;
    double_t q_, phi_, theta_, mu_, V_;
    long_t  Y_, m_;

    // 1, 2
    if (p_>0.5) 
        {
        q_ = 1.-p_;
        }
    else
        {
        q_ = p_;
        }
    
    // 3,4
    long_t n1mq = static_cast<long_t> ( static_cast<double_t>(n_) * (1.-q_)); 
    double_t n1mq_dbl = static_cast<double_t>(n1mq);
    if ( static_cast<double_t>(n_)*(1.-q_) - n1mq_dbl  > q_)
        {
        mu_ = q_* (n1mq_dbl + 1.) / (1.-q_);
        }
    else
        {
        mu_ = static_cast<double_t>(n_) - n1mq_dbl;
        }
    
    //5, 6, 7
    theta_ = (1./q_ - 1.) * mu_;
    phi_ = std::log(theta_);
    m_ = static_cast<long_t> (theta_);
    
    bool not_finished = 1;
    poisson_dev_.set_lambda( mu_ );
    while (not_finished)
        {
        //8,9
        X_ = n_+1;
        while( X_ > n_)
            {
            X_ = poisson_dev_.uldev(rng);
            }
        
        //10
        V_ = exp_dev_(rng);
        
        //11
        Y_ = n_ - X_;
        
        //12
        if ( V_ < static_cast<double_t>(m_-Y_)*phi_ - f_[m_+1] + f_[Y_+1] )
            {
            not_finished = 1;
            }
        else
            {
            not_finished = 0;
            }        
        }
    if (p_ <= 0.5)
        {
        return X_;
        }
    else
        {
        return static_cast<ulong_t>(Y_);
        }
}



/* ---------------------------------------------------------------- 
 * Constructor of age distribution class
 * ---------------------------------------------------------------- */
    
nest::ppd_sup_generator::Age_distribution_::Age_distribution_(size_t num_age_bins, ulong_t ini_occ_ref, ulong_t ini_occ_act)
{
    occ_active_ = ini_occ_act;
    occ_refractory_.resize(num_age_bins, ini_occ_ref);
    activate_ = 0;
    bino_dev_ = new FastBinomialRandomDev_( ini_occ_act + num_age_bins*ini_occ_ref );
    
}

/* ---------------------------------------------------------------- 
 * Propagate age distribution one time step and generate spikes
 * ---------------------------------------------------------------- */

nest::ulong_t nest::ppd_sup_generator::Age_distribution_::update(double_t hazard_rate, librandom::RngPtr rng)
{

//    // test the bino generator
//    ulong_t bino_bp = 0;
//    bino_dev_->set_p_n(0.01, 2);
//    for (int i=0; i<100000; i++){
//        bino_bp += bino_dev_->uldev(rng);
//        }
//    ulong_t bino_nest = 0;
//    bino_dev_nest_.set_p_n(0.01, 2);
//    for (int i=0; i<100000; i++){
//        bino_nest += bino_dev_nest_.uldev(rng);
//        }
//    std::cout << bino_bp << ", " << bino_nest << "\n";
//    //
    
    ulong_t n_spikes;
    if (occ_active_>0)
    { 
      //Poisson approximation of binomial deviate
//      poisson_dev_.set_lambda( hazard_rate * occ_active_);
//      n_spikes = poisson_dev_.uldev(rng);
//      if (n_spikes>occ_active_)
//         n_spikes = occ_active_;
      
      // binomial_randomdev deviate from librandom (slow)
//      bino_dev_nest_.set_p_n( hazard_rate, occ_active_);
//      n_spikes = bino_dev_nest_.uldev(rng);
      
//      // binomial variate from custom generator (BP algorithm)
      bino_dev_->set_p_n( hazard_rate, occ_active_);
      n_spikes = bino_dev_->uldev(rng);
    }
    else
      n_spikes = 0;
    
    if (occ_refractory_.size()>0)
    {
      occ_active_ += occ_refractory_[activate_] - n_spikes;
      occ_refractory_[activate_] = n_spikes;
      activate_ = (activate_ + 1) % occ_refractory_.size();
    }
    return n_spikes;
}


/* ---------------------------------------------------------------- 
 * Default constructors defining default parameter
 * ---------------------------------------------------------------- */
    
nest::ppd_sup_generator::Parameters_::Parameters_()
  : rate_(0.0),  // Hz
    dead_time_(0.0),  // ms
    n_proc_(1),
    frequency_(0.0),  // Hz
    amplitude_(0.0),  // percentage
    num_targets_(0)
{}

/* ---------------------------------------------------------------- 
 * Parameter extraction and manipulation functions
 * ---------------------------------------------------------------- */

void nest::ppd_sup_generator::Parameters_::get(DictionaryDatum &d) const
{
  (*d)[names::rate] = rate_;
  (*d)[names::dead_time] = dead_time_;
  (*d)[names::n_proc] = n_proc_;
  (*d)[names::frequency] = frequency_;
  (*d)[names::amplitude] = amplitude_;
  
}  

void nest::ppd_sup_generator::Parameters_::set(const DictionaryDatum& d)
{

  updateValue<double_t>(d, names::dead_time, dead_time_);
  if ( dead_time_ < 0 )
    throw BadProperty("The dead time cannot be negative.");
    
  updateValue<double_t>(d, names::rate, rate_);
  if ( 1000.0 / rate_ <= dead_time_ )
    throw BadProperty("The inverse rate has to be larger than the dead time.");
  
  long n_proc_l = 0;
  updateValue<long_t>(d, names::n_proc, n_proc_l);
  if ( n_proc_l < 1 )
    throw BadProperty("The number of component processes cannot be smaller than one");
  else
    n_proc_ = static_cast<ulong_t> (n_proc_l);
  
  updateValue<double_t>(d, names::frequency, frequency_);
  
  updateValue<double_t>(d, names::amplitude, amplitude_);
  if ( amplitude_ > 1.0 or amplitude_ < 0.0 )
    throw BadProperty("The relative amplitude of the rate modulation must be in [0,1].");


}


/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::ppd_sup_generator::ppd_sup_generator()
  : Node(),
    device_(), 
    P_()
{}

nest::ppd_sup_generator::ppd_sup_generator(const ppd_sup_generator& n)
  : Node(n), 
    device_(n.device_),
    P_(n.P_)
{}


/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::ppd_sup_generator::init_node_(const Node& proto)
{
  const ppd_sup_generator& pr = downcast<ppd_sup_generator>(proto);

  device_.init_parameters(pr.device_);
  
  P_ = pr.P_;
}

void nest::ppd_sup_generator::init_state_(const Node& proto)
{ 
  const ppd_sup_generator& pr = downcast<ppd_sup_generator>(proto);

  device_.init_state(pr.device_);
}

void nest::ppd_sup_generator::init_buffers_()
{ 
  device_.init_buffers();
}

void nest::ppd_sup_generator::calibrate()
{
  device_.calibrate();
  
  double_t h = Time::get_resolution().get_ms();
 
  // compute number of age bins that need to be kept track of
  ulong_t num_age_bins = static_cast<ulong_t> (P_.dead_time_ / h); 
 
  // compute omega to evaluate modulation with, units [rad/ms]
  V_.omega_ = 2.0  * numerics::pi * P_.frequency_ / 1000.0;
  
  // hazard rate in units of the simulation time step.
  V_.hazard_step_ = 1.0/(1000.0/P_.rate_ - P_.dead_time_) * h;
  
  // equilibrium occupation of dead time bins (in case of constant rate)
  ulong_t ini_occ_0 = static_cast<ulong_t> (P_.rate_/1000.0 * P_.n_proc_ * h);
  
  // If new targets have been added during a simulation break, we
  // initialize the new elements in age_distributions with the initial dist. The existing
  // elements are unchanged.
  Age_distribution_ age_distribution0 (num_age_bins, ini_occ_0, P_.n_proc_ - ini_occ_0 * num_age_bins);
  B_.age_distributions_.resize( P_.num_targets_, age_distribution0 );
}



/* ---------------------------------------------------------------- 
 * Update function and event hook
 * ---------------------------------------------------------------- */

void nest::ppd_sup_generator::update(Time const & T, const long_t from, const long_t to)
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

    // get current (time-dependent) hazard rate and store it.
    if ( P_.amplitude_>0.0 && ( P_.frequency_>0.0 ||  P_.frequency_<0.0 ) )
    {
      double_t t_ms = t.get_ms(); 
      V_.hazard_step_t_ = V_.hazard_step_ * (1.0 + P_.amplitude_ * std::sin(V_.omega_ * t_ms));
    }   
    else
      V_.hazard_step_t_ = V_.hazard_step_;    
    
    DSSpikeEvent se;
    network()->send(*this, se, lag);
  }
}


void nest::ppd_sup_generator::event_hook(DSSpikeEvent& e)
{

  // get port number
  const port prt = e.get_port();

  // we handle only one port here, get reference to vector element
  //  std::cout << P_.rate_ << "-" << P_.dead_time_ << "-" << prt << " - " << static_cast<size_t>(prt) << " - " << B_.age_distributions_.size() << "\n";
  // THIS SEEMS TO BREAK WITH rev 9192. Sometimes prt then becomes incredibly large and the assertion fails
  assert(0 <= prt && static_cast<size_t>(prt) < B_.age_distributions_.size() );

  // age_distribution object propagates one time step and returns number of spikes
  ulong_t n_spikes = B_.age_distributions_[prt].update( V_.hazard_step_t_, net_->get_rng(get_thread()) );
  
  if ( n_spikes > 0 ) // we must not send events with multiplicity 0
  {
    e.set_multiplicity(n_spikes);
    e.get_receiver().handle(e);
  }


}
