/*
 *  iaf_psc_delta_currents.cpp
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

/* iaf_psc_delta is a neuron where the potential jumps on each spike arrival. */

#include "exceptions.h"
#include "iaf_psc_delta_currents.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"  
#include "doubledatum.h"
#include "dictutils.h"
#include "numerics.h"
#include "analog_data_logger_impl.h"

#include <limits>
namespace nest
{

/* ---------------------------------------------------------------- 
 * Default constructors defining default parameters and state
 * ---------------------------------------------------------------- */

nest::iaf_psc_delta_currents::Parameters_::Parameters_()
  : tau_m_     ( 10.0     ),  // ms
    c_m_       (250.0     ),  // pF
    t_ref_     (  2.0     ),  // ms
    E_L_       (-70.0     ),  // mV
    I_e_       (  0.0     ),  // pA
    V_th_      (-55.0-E_L_),  // mV, rel to U0_
    V_min_     (-std::numeric_limits<double_t>::max()),
                              // relative U0_-55.0-U0_),  // mV, rel to U0_
    V_reset_   (-70.0-E_L_),
    with_refr_input_(false)
{}

nest::iaf_psc_delta_currents::State_::State_()
  : y0_   (0.0),
    y3_   (0.0),  
    r_    (0),
    refr_spikes_buffer_(0.0)
{}

/* ---------------------------------------------------------------- 
 * Parameter and state extractions and manipulation functions
 * ---------------------------------------------------------------- */

void nest::iaf_psc_delta_currents::Parameters_::get(DictionaryDatum &d) const
{
  def<double>(d, names::E_L, E_L_);   // Resting potential
  def<double>(d, names::I_e, I_e_);
  def<double>(d, names::V_th, V_th_+E_L_); // threshold value
  def<double>(d, names::V_reset, V_reset_+E_L_);
  def<double>(d, names::V_min, V_min_+E_L_);
  def<double>(d, names::C_m, c_m_);
  def<double>(d, names::tau_m, tau_m_);
  def<double>(d, names::t_ref, t_ref_);
  def<bool>(d, "refractory_input", with_refr_input_);
}

void nest::iaf_psc_delta_currents::Parameters_::set(const DictionaryDatum& d)
{
  updateValue<double>(d, names::E_L, E_L_);

  if(updateValue<double>(d, names::V_reset, V_reset_))
    V_reset_ -= E_L_;

  if (updateValue<double>(d, names::V_th, V_th_)) 
    V_th_ -= E_L_;

  if (updateValue<double>(d, names::V_min, V_min_))
    V_min_ -= E_L_;
    
  updateValue<double>(d, names::I_e, I_e_);
  updateValue<double>(d, names::C_m, c_m_);
  updateValue<double>(d, names::tau_m, tau_m_);
  updateValue<double>(d, names::t_ref, t_ref_);

    
  if ( c_m_ <= 0 )
    throw BadProperty("Capacitance must be strictly positive.");

  if ( t_ref_ < 0 )
    throw BadProperty("Refractory time must not be negative.");
    
  if ( tau_m_ <= 0 )
    throw BadProperty("All time constants must be strictly positive.");

  updateValue<bool>(d, "refractory_input", with_refr_input_);
}

void nest::iaf_psc_delta_currents::State_::get(DictionaryDatum &d, const Parameters_& p) const
{
  def<double>(d, names::V_m, y3_ + p.E_L_); // Membrane potential
}

void nest::iaf_psc_delta_currents::State_::set(const DictionaryDatum& d, const Parameters_& p)
{
  if ( updateValue<double>(d, names::V_m, y3_) )
    y3_ -= p.E_L_;
}

/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::iaf_psc_delta_currents::iaf_psc_delta_currents()
  : Archiving_Node(), 
    P_(), 
    S_()
{}

nest::iaf_psc_delta_currents::iaf_psc_delta_currents(const iaf_psc_delta_currents& n)
  : Archiving_Node(n), 
    P_(n.P_), 
    S_(n.S_)
{}

/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::iaf_psc_delta_currents::init_state_(const Node& proto)
{
  const iaf_psc_delta_currents& pr = downcast<iaf_psc_delta_currents>(proto);
  S_ = pr.S_;
}

void nest::iaf_psc_delta_currents::init_buffers_()
{
  B_.spikes_.clear();            // includes resize
  B_.currents_.clear();          // includes resize
  B_.potentials_.clear_data();   // includes resize
  B_.syncurrents_.clear_data();  //includes resize
  Archiving_Node::clear_history();
}

void nest::iaf_psc_delta_currents::calibrate()
{
  const double h = Time::get_resolution().get_ms(); 

 
  V_.P33_ = std::exp(-h/P_.tau_m_);
  V_.P30_ = 1/P_.c_m_*(1-V_.P33_)*P_.tau_m_;


  // TauR specifies the length of the absolute refractory period as 
  // a double_t in ms. The grid based iaf_psp_delta_currents can only handle refractory
  // periods that are integer multiples of the computation step size (h).
  // To ensure consistency with the overall simulation scheme such conversion
  // should be carried out via objects of class nest::Time. The conversion 
  // requires 2 steps:
  //     1. A time object r is constructed defining  representation of 
  //        TauR in tics. This representation is then converted to computation time
  //        steps again by a strategy defined by class nest::Time.
  //     2. The refractory time in units of steps is read out get_steps(), a member
  //        function of class nest::Time.
  //
  // The definition of the refractory period of the iaf_psc_delta_currents is consistent 
  // the one of iaf_neuron_ps.                                         
  //
  // Choosing a TauR that is not an integer multiple of the computation time 
  // step h will leed to accurate (up to the resolution h) and self-consistent
  // results. However, a neuron model capable of operating with real valued spike
  // time may exhibit a different effective refractory time.
  //
  
  V_.RefractoryCounts_ = Time(Time::ms(P_.t_ref_)).get_steps();
  assert(V_.RefractoryCounts_ >= 0);  // since t_ref_ >= 0, this can only fail in error
}

/* ---------------------------------------------------------------- 
 * Update and spike handling functions
 */

void nest::iaf_psc_delta_currents::update(Time const & origin, 
				 const long_t from, const long_t to)
{
  assert(to >= 0 && (delay) from < Scheduler::get_min_delay());
  assert(from < to);

  for ( long_t lag = from ; lag < to ; ++lag )
  {
    double_t currentspike = B_.spikes_.get_value(lag);
    if ( S_.r_ == 0 )
    {
      // neuron not refractory
      S_.y3_ = V_.P30_*(S_.y0_ + P_.I_e_) + V_.P33_*S_.y3_ + currentspike;

      // if we have accumulated spikes from refractory period, 
      // add and reset accumulator
      if ( P_.with_refr_input_ && S_.refr_spikes_buffer_ != 0.0 )
      {
	S_.y3_ += S_.refr_spikes_buffer_;
	S_.refr_spikes_buffer_ = 0.0;
      }
      
      // lower bound of membrane potential
      S_.y3_ = ( S_.y3_<P_.V_min_ ? P_.V_min_ : S_.y3_); 	 
    }
    else // neuron is absolute refractory
    {
      // read spikes from buffer and accumulate them, discounting
      // for decay until end of refractory period
      if ( P_.with_refr_input_ )
	S_.refr_spikes_buffer_ += currentspike
	  * std::exp(-S_.r_ * Time::get_resolution().get_ms() / P_.tau_m_);
      else

      --S_.r_;
    }
   
    // threshold crossing
    if (S_.y3_ >= P_.V_th_)
    {
      S_.r_ = V_.RefractoryCounts_;
      S_.y3_ = P_.V_reset_;
        
      // EX: must compute spike time
      set_spiketime(Time::step(origin.get_steps()+lag+1));
        
      SpikeEvent se;
      network()->send(*this, se, lag);
    }

    // set new input current
    S_.y0_ = B_.currents_.get_value(lag);

    // voltage logging
    B_.potentials_.record_data(origin.get_steps()+lag, S_.y3_ + P_.E_L_);
    // currents logging
    B_.syncurrents_.record_data(origin.get_steps()+lag, currentspike);
  }  
}                           
                     
void nest::iaf_psc_delta_currents::handle(SpikeEvent & e)
{
  assert(e.get_delay() > 0);

  // EX: We must compute the arrival time of the incoming spike
  //     explicity, since it depends on delay and offset within
  //     the update cycle.  The way it is done here works, but
  //     is clumsy and should be improved.
  B_.spikes_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()),
                    e.get_weight() * e.get_multiplicity() );
}

void nest::iaf_psc_delta_currents::handle(CurrentEvent& e)
{
  assert(e.get_delay() > 0);

  const double_t c=e.get_current();
  const double_t w=e.get_weight();

  // add weighted current; HEP 2002-10-04
  B_.currents_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()), 
		      w *c);
}

void nest::iaf_psc_delta_currents::handle(PotentialRequest& e)
{
  B_.potentials_.handle(*this, e);
}

void nest::iaf_psc_delta_currents::handle(SynapticCurrentRequest& e)
{
  B_.syncurrents_.handle(*this, e);
}
 
} // namespace


