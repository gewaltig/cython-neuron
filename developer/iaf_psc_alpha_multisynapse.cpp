/*
 *  iaf_psc_alpha_multisynapse.cpp
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

#include "exceptions.h"
#include "iaf_psc_alpha_multisynapse.h"
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

nest::iaf_psc_alpha_multisynapse::Parameters_::Parameters_()
  : Tau_                 ( 10.0    ),  // ms
    C_                   (250.0    ),  // pF
    TauR_                (  2.0    ),  // ms
    U0_                  (-70.0    ),  // mV
    I_e_                 (  0.0    ),  // pA
    V_reset_             (-70.0-U0_),  // mV, rel to U0_
    Theta_               (-55.0-U0_),  // mV, rel to U0_
    LowerBound_          (-std::numeric_limits<double_t>::infinity()),
    num_of_receptors_    (0)
{
  receptor_types_.clear();
  tau_syn_.clear();  
}


nest::iaf_psc_alpha_multisynapse::State_::State_()
  : y0_   (0.0),  
    y3_   (0.0),
    r_    (0)
{
  y1_syn_.clear();
  y2_syn_.clear();
}


/* ---------------------------------------------------------------- 
 * Parameter and state extractions and manipulation functions
 * ---------------------------------------------------------------- */

void nest::iaf_psc_alpha_multisynapse::Parameters_::get(DictionaryDatum &d) const
{
  //def<double>(d, names::V_m, y3_+U0_); // Membrane potential
  def<double>(d, names::E_L, U0_);   // Resting potential
  def<double>(d, names::I_e, I_e_);
  def<double>(d, names::V_th, Theta_+U0_); // threshold value
  def<double>(d, names::V_min, LowerBound_+U0_);
  def<double>(d, names::V_reset, V_reset_+U0_);
  def<double>(d, names::C_m, C_);
  def<double>(d, names::tau_m, Tau_);
  def<double>(d, names::t_ref, TauR_);
  def<int>(d,"n_synapses", num_of_receptors_);
  
  ArrayDatum tau_syn_ad(tau_syn_);
  def<ArrayDatum>(d,"tau_syn", tau_syn_ad);
  
  (*d)["receptor_types"] = IntVectorDatum(new std::vector<long>(receptor_types_));

}

void nest::iaf_psc_alpha_multisynapse::Parameters_::set(const DictionaryDatum& d)
{

  updateValue<double>(d,names::E_L,U0_);

  if(updateValue<double>(d,names::V_reset,V_reset_))
    V_reset_ -=U0_;

  updateValue<double>(d,names::I_e,I_e_);

  if (updateValue<double>(d,names::V_th,Theta_)) 
    Theta_ -= U0_;

  if (updateValue<double>(d,names::V_min,LowerBound_)) 
    LowerBound_ -= U0_;

  updateValue<double>(d,names::C_m,C_);
  updateValue<double>(d,names::tau_m,Tau_);
  updateValue<double>(d,names::t_ref,TauR_);

  if (updateValue<long>(d,"n_synapses",num_of_receptors_))
    tau_syn_.resize(num_of_receptors_, 2.0);

  std::vector<double> tau_tmp;
  if (updateValue<std::vector<double> >(d, "tau_syn", tau_tmp))
  {
    if(tau_tmp.size() != num_of_receptors_)
      throw DimensionMismatch(num_of_receptors_, tau_tmp.size());
    tau_syn_ = tau_tmp;
  }

  //  std::vector<long> recept_tmp;
  updateValue<std::vector<long> >(d, "receptor_types", receptor_types_);

  

}

void nest::iaf_psc_alpha_multisynapse::State_::get(DictionaryDatum &d, const Parameters_& p) const
{
  def<double>(d, names::V_m, y3_ + p.U0_); // Membrane potential
}

void nest::iaf_psc_alpha_multisynapse::State_::set(const DictionaryDatum& d, const Parameters_& p)
{
  if ( updateValue<double>(d, names::V_m, y3_) )
    y3_ -= p.U0_;
}


/* ---------------------------------------------------------------- 
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */

nest::iaf_psc_alpha_multisynapse::iaf_psc_alpha_multisynapse()
  : Archiving_Node(), 
    P_(), 
    S_()
{}

nest::iaf_psc_alpha_multisynapse::iaf_psc_alpha_multisynapse(const iaf_psc_alpha_multisynapse& n)
  : Archiving_Node(n), 
    P_(n.P_), 
    S_(n.S_)
{}

/* ---------------------------------------------------------------- 
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::iaf_psc_alpha_multisynapse::init_state_(const Node& proto)
{
  const iaf_psc_alpha_multisynapse& pr = downcast<iaf_psc_alpha_multisynapse>(proto);
  S_ = pr.S_;
}

void nest::iaf_psc_alpha_multisynapse::init_buffers_()
{

  
  B_.currents_.clear();        // includes resize
  B_.potentials_.clear_data(); // includes resize
  Archiving_Node::clear_history();
}

void nest::iaf_psc_alpha_multisynapse::calibrate()
{
  const double h = Time::get_resolution().get_ms(); 

  V_.receptor_types_size_ = P_.receptor_types_.size();

  // if n_synapses has been DEcreased with SetStatus, force new dimension.
  if (P_.num_of_receptors_ < V_.receptor_types_size_){
    V_.receptor_types_size_ = P_.num_of_receptors_;
    P_.receptor_types_.resize(V_.receptor_types_size_);
  }

  V_.P11_syn_.resize(V_.receptor_types_size_);
  V_.P21_syn_.resize(V_.receptor_types_size_);
  V_.P22_syn_.resize(V_.receptor_types_size_);
  V_.P31_syn_.resize(V_.receptor_types_size_);
  V_.P32_syn_.resize(V_.receptor_types_size_);
  
  S_.y1_syn_.resize(V_.receptor_types_size_);
  S_.y2_syn_.resize(V_.receptor_types_size_);
  
  V_.PSCInitialValues_.resize(V_.receptor_types_size_);

  B_.spikes_.resize(V_.receptor_types_size_);

  V_.P33_ = std::exp(-h/P_.Tau_);
  V_.P30_ = 1/P_.C_*(1-V_.P33_)*P_.Tau_;

  for (unsigned int i=0; i < V_.receptor_types_size_; i++)
  {
    V_.P11_syn_[i] = V_.P22_syn_[i] =std::exp(-h/P_.tau_syn_[i]);
    V_.P21_syn_[i] = h*V_.P11_syn_[i];
    V_.P31_syn_[i] = 1/P_.C_ * ((V_.P11_syn_[i]-V_.P33_)/(-1/P_.tau_syn_[i]- -1/P_.Tau_)- h*V_.P11_syn_[i])
      /(-1/P_.Tau_ - -1/P_.tau_syn_[i]);
    V_.P32_syn_[i] = 1/P_.C_*(V_.P33_-V_.P11_syn_[i])/(-1/P_.Tau_ - -1/P_.tau_syn_[i]);

    V_.PSCInitialValues_[i] = 1.0 * numerics::e/P_.tau_syn_[i];
    B_.spikes_[i].resize();
  }
  
  Time r=Time::ms(P_.TauR_);
  V_.RefractoryCounts_=r.get_steps();
  
  assert(V_.RefractoryCounts_ > 0); // better throw an exception

  B_.currents_.resize();

  B_.potentials_.calibrate(); 
  B_.syncurrents_.calibrate();

}

void nest::iaf_psc_alpha_multisynapse::update(Time const & origin, const long_t from, const long_t to)
{
  assert(to >= 0 && (delay) from < Scheduler::get_min_delay());
  assert(from < to);
  static double tmpcurrent;

  for ( long_t lag = from ; lag < to ; ++lag )
  {
    
    if ( S_.r_ == 0 )
    {
      // neuron not refractory
      S_.y3_ = V_.P30_*(S_.y0_ + P_.I_e_) + V_.P33_*S_.y3_;

      tmpcurrent=0.0;
      for (unsigned int i=0; i < V_.receptor_types_size_; i++){
	S_.y3_ += V_.P31_syn_[i]*S_.y1_syn_[i] + V_.P32_syn_[i]*S_.y2_syn_[i];
	tmpcurrent+=S_.y2_syn_[i];
      }

      // lower bound of membrane potential
      S_.y3_ = ( S_.y3_<P_.LowerBound_ ? P_.LowerBound_ : S_.y3_); 
    }
    else // neuron is absolute refractory
     --S_.r_;

    for (unsigned int i=0; i < V_.receptor_types_size_; i++)
    {      
      // alpha shape PSCs
      S_.y2_syn_[i] = V_.P21_syn_[i] * S_.y1_syn_[i] + V_.P22_syn_[i] * S_.y2_syn_[i];
      S_.y1_syn_[i] *= V_.P11_syn_[i];

      // collect spikes
      S_.y1_syn_[i] += V_.PSCInitialValues_[i] * B_.spikes_[i].get_value(lag);   
    }


    // threshold crossing
    if (S_.y3_ >= P_.Theta_  )
    {
      S_.r_ = V_.RefractoryCounts_;
      S_.y3_=P_.V_reset_; 
      // A supra-threshold membrane potential should never be observable.
      // The reset at the time of threshold crossing enables accurate integration
      // independent of the computation step size, see [2,3] for details.   

      set_spiketime(Time::step(origin.get_steps()+lag+1));
      SpikeEvent se;
      network()->send(*this, se, lag);
    }

    // set new input current
    S_.y0_ = B_.currents_.get_value(lag);

    // voltage logging
    B_.potentials_.record_data(origin.get_steps()+lag, S_.y3_ + P_.U0_);
    
    B_.syncurrents_.record_data(origin.get_steps()+lag, tmpcurrent);

  }  
}                           

port nest::iaf_psc_alpha_multisynapse::connect_sender(SpikeEvent&, port receptor_type)
{
  bool new_rp = true;
  
  // look if new port is encountered
  for(std::vector<long>::const_iterator pii = P_.receptor_types_.begin(); pii != P_.receptor_types_.end(); ++pii)
  {
    if (*pii == receptor_type)
    {
      new_rp = false;
      break;
    }
  }

  if (new_rp)
  {
    
    if (P_.num_of_receptors_ <= P_.receptor_types_.size())
    {
      // space has not been pre-allocated
      ++P_.num_of_receptors_;

      RingBuffer spiketmp;
      spiketmp.clear();
      B_.spikes_.push_back(spiketmp); 

      P_.tau_syn_.push_back(2.0); 

      V_.PSCInitialValues_.push_back(0.0);
      S_.y1_syn_.push_back(0.0);
      S_.y2_syn_.push_back(0.0);
    }

    P_.receptor_types_.push_back(receptor_type);
    V_.receptor_types_size_ = P_.receptor_types_.size();
  }
  return receptor_type;
}

void nest::iaf_psc_alpha_multisynapse::handle(SpikeEvent & e)
{
  assert(e.get_delay() > 0);

  for (unsigned int i=0; i < V_.receptor_types_size_; i++)
  {
    if (P_.receptor_types_[i] == e.get_rport()){
      B_.spikes_[i].add_value(e.get_rel_delivery_steps(network()->get_slice_origin()),
			   e.get_weight() * e.get_multiplicity());
    }
  }
}

void nest::iaf_psc_alpha_multisynapse::handle(CurrentEvent& e)
{
  assert(e.get_delay() > 0);

  const double_t c=e.get_current();
  const double_t w=e.get_weight();

  // add weighted current; HEP 2002-10-04
  B_.currents_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()), 
		      w *c);
}

void nest::iaf_psc_alpha_multisynapse::handle(PotentialRequest& e)
{
  B_.potentials_.handle(*this, e);
}

void nest::iaf_psc_alpha_multisynapse::handle(SynapticCurrentRequest& e)
{
  B_.syncurrents_.handle(*this, e);
}

} // namespace
