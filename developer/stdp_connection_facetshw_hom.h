/*
 *  stdp_connection_facetshw_hom.h
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

#ifndef STDP_CONNECTION_FACETSHW_HOM_H
#define STDP_CONNECTION_FACETSHW_HOM_H

/* BeginDocumentation
  Name: stdp_facetshw_synapse_hom - Synapse type for spike-timing dependent
   plasticity using homogeneous parameters, i.e. all synapses have the same parameters.

  Description:
   stdp_synapse is a connector to create synapses with spike time 
   dependent plasticity (as defined in [1]). Here the weight dependence
   exponent can be set separately for potentiation and depression.
   The reduced symmetric nearest-neighbor spike pairing scheme [5]
   as implemented in the FACETS (BrainScaleS) hardware systems [6] is used
   instead of an all-to-all spike pairing scheme.
   Consequently tau_minus_ is calculated within this synapse and not
   at the neuron site via Kplus_ like in stdp_connection_hom.
   
  Examples:
   multiplicative STDP [2]  mu_plus = mu_minus = 1.0
   additive STDP       [3]  mu_plus = mu_minus = 0.0
   Guetig STDP         [1]  mu_plus = mu_minus = [0.0,1.0]
   van Rossum STDP     [4]  mu_plus = 0.0 mu_minus = 1.0 

  Parameters:
   Common properties:
    tau_plus        double - Time constant of STDP window, potentiation in ms 
    tau_minus_stdp  double - Time constant of STDP window, depression in ms 
    Wmax            double - Maximum allowed weight

    no_synapses double             long - total number of synapses
    synapses_per_driver            long - number of synapses updated at once
    driver_readout_time          double - time for processing of one synapse row (synapse driver)
    readout_cycle_duration       double - duration between two subsequent updates of same synapse (synapse driver)
    hardware_stage                 long - hardware version: 1 for chip-based
                                          and 2 (and 3 for independent resets) for wafer-scale system
    lookuptable_causal     vector<long> - look-up table, causal
    lookuptable_acausal    vector<long> - look-up table, acausal

   Individual properties:
    a_causal     double - causal and acausal spike pair accumulations
    a_acausal    double
    a_th_causal  double - thresholds for causal and acausal spike pair accumulations
                          no common property, because variation of analog synapse circuitry can be applied here
    a_th_acausal double
    synapse_id   long   - synapse ID, used to assign synapses to groups (synapse drivers)

  Notes:
   The synapse IDs are assigned to each synapse in an ascending order (0,1,2, ...) according their first
   presynaptic activity and is used to group synapses that are updated at once.
   It is possible to avoid activity dependent synapse ID assignments by manually setting the no_synapses
   and the synapse_id(s) before running the simulation.
   The weights will be discretized after the first presynaptic activity at a synapse.

  Transmits: SpikeEvent
   
  References:
   [1] Guetig et al. (2003) Learning Input Correlations through Nonlinear
       Temporally Asymmetric Hebbian Plasticity. Journal of Neuroscience

   [2] Rubin, J., Lee, D. and Sompolinsky, H. (2001). Equilibrium
       properties of temporally asymmetric Hebbian plasticity, PRL
       86,364--367

   [3] Song, S., Miller, K. D. and Abbott, L. F. (2000). Competitive
       Hebbian learning through spike-timing-dependent synaptic
       plasticity, Nature Neuroscience 3:9,919--926

   [4] van Rossum, M. C. W., Bi, G-Q and Turrigiano, G. G. (2000). 
       Stable Hebbian learning from spike timing-dependent
       plasticity, Journal of Neuroscience, 20:23,8812--8821

   [5] Morrison, A., Diesmann, M., and Gerstner, W. (2008).
       Phenomenological models of synaptic plasticity based on
       spike-timing, Biol. Cybern., 98,459--478

   [6] Schemmel, J., Gruebl, A., Meier, K., and Mueller, E. (2006).
       Implementing synaptic plasticity in a VLSI spiking neural
       network model, In Proceedings of the 2006 International
       Joint Conference on Neural Networks, pp.1--6, IEEE Press

  FirstVersion: March 2006
  Author: Moritz Helias, Abigail Morrison, Thomas Pfeil (TP)
  SeeAlso: synapsedict, tsodyks_synapse, static_synapse
*/

#include "connection_het_wd.h"
#include "archiving_node.h"
#include <cmath>

namespace nest
{

  /**
   * Class containing the common properties for all synapses of type STDPFACETSHWConnectionHom.
   */
  class STDPFACETSHWHomCommonProperties : public CommonSynapseProperties
    {
      friend class STDPFACETSHWConnectionHom;

    public:

      /**
       * Default constructor.
       * Sets all property values to defaults.
       */
      STDPFACETSHWHomCommonProperties();

      /**
       * Get all properties and put them into a dictionary.
       */
      void get_status(DictionaryDatum & d) const;

      /**
       * Set properties from the values given in dictionary.
       */
      void set_status(const DictionaryDatum & d, ConnectorModel& cm);

      // overloaded for all supported event types
      void check_event(SpikeEvent&) {}

    private:
      /**
       * Calculate the readout cycle duration
       */
      void calc_readout_cycle_duration_();


      // data members common to all connections
      double_t tau_plus_;
      double_t tau_minus_;
      double_t Wmax_;
      double_t weight_per_lut_entry_;

      //STDP controller parameters
      long_t no_synapses_;
      long_t synapses_per_driver_;
      double_t driver_readout_time_;
      double_t readout_cycle_duration_;
      long_t hardware_stage_;
      std::vector<long_t> lookuptable_causal_; //(not uint_t because of IntVectorDatum)
      std::vector<long_t> lookuptable_acausal_;
    };



  /**
   * Class representing an STDP connection with homogeneous parameters, i.e. parameters are the same for all synapses.
   */
  class STDPFACETSHWConnectionHom : public ConnectionHetWD
  {

  public:
  /**
   * Default Constructor.
   * Sets default values for all parameters. Needed by GenericConnectorModel.
   */
  STDPFACETSHWConnectionHom();

  /**
   * Copy constructor from a property object.
   * Needs to be defined properly in order for GenericConnector to work.
   */
  STDPFACETSHWConnectionHom(const STDPFACETSHWConnectionHom &);

  /**
   * Default Destructor.
   */
  virtual ~STDPFACETSHWConnectionHom() {}

  /*
   * This function calls check_connection on the sender and checks if the receiver
   * accepts the event type and receptor type requested by the sender.
   * Node::check_connection() will either confirm the receiver port by returning
   * true or false if the connection should be ignored.
   * We have to override the base class' implementation, since for STDP
   * connections we have to call register_stdp_connection on the target neuron
   * to inform the Archiver to collect spikes for this connection.
   *
   * \param s The source node
   * \param r The target node
   * \param receptor_type The ID of the requested receptor type
   * \param t_lastspike last spike produced by presynaptic neuron (in ms)
   */
  void check_connection(Node & s, Node & r, port receptor_type, double_t t_lastspike);

  /**
   * Get all properties of this connection and put them into a dictionary.
   */
  void get_status(DictionaryDatum & d) const;

  /**
   * Set properties of this connection from the values given in dictionary.
   */
  void set_status(const DictionaryDatum & d, ConnectorModel &cm);

  /**
   * Set properties of this connection from position p in the properties
   * array given in dictionary.
   */
  void set_status(const DictionaryDatum & d, index p, ConnectorModel &cm);

  /**
   * Create new empty arrays for the properties of this connection in the given
   * dictionary. It is assumed that they are not existing before.
   */
  void initialize_property_arrays(DictionaryDatum & d) const;

  /**
   * Append properties of this connection to the given dictionary. If the
   * dictionary is empty, new arrays are created first.
   */
  void append_properties(DictionaryDatum & d) const;

  /**
   * Send an event to the receiver of this connection.
   * \param e The event to send
   * \param t_lastspike Point in time of last spike sent.
   */
  void send(Event& e, double_t t_lastspike, STDPFACETSHWHomCommonProperties &);

  // overloaded for all supported event types
  using Connection::check_event;
  void check_event(SpikeEvent&) {}

 private:
  // transformation biological weight <-> discrete weight (represented in index of look-up table)
  uint_t weight_to_entry_(double_t weight, double_t weight_per_lut_entry);
  double_t entry_to_weight_(uint_t discrete_weight, double_t weight_per_lut_entry);

  uint_t facilitate_(uint_t discrete_weight_, const STDPFACETSHWHomCommonProperties &cp);
  uint_t depress_(uint_t discrete_weight_, const STDPFACETSHWHomCommonProperties &cp);

  // data members of each connection
  double_t a_causal_;
  double_t a_acausal_;
  double_t a_th_causal_;
  double_t a_th_acausal_;

  bool init_flag_;
  long_t synapse_id_;
  double_t next_readout_time_;
  uint_t discrete_weight_; //TODO: TP: only needed in send, move to common properties or "static"?
  };


inline
uint_t STDPFACETSHWConnectionHom::weight_to_entry_(double_t weight, double_t weight_per_lut_entry)
{
  // returns the discrete weight in terms of the look-up table index
  return round(weight / weight_per_lut_entry);
}

inline
double_t STDPFACETSHWConnectionHom::entry_to_weight_(uint_t discrete_weight, double_t weight_per_lut_entry)
{
  // returns the continuous weight
  return discrete_weight * weight_per_lut_entry;
}

inline
uint_t STDPFACETSHWConnectionHom::facilitate_(uint_t discrete_weight_, const STDPFACETSHWHomCommonProperties &cp)
{
  return cp.lookuptable_causal_[discrete_weight_];
}

inline
uint_t STDPFACETSHWConnectionHom::depress_(uint_t discrete_weight_, const STDPFACETSHWHomCommonProperties &cp)
{
  return cp.lookuptable_acausal_[discrete_weight_];
}

inline
  void STDPFACETSHWConnectionHom::check_connection(Node & s, Node & r, port receptor_type, double_t t_lastspike)
{
  ConnectionHetWD::check_connection(s, r, receptor_type, t_lastspike);
  r.register_stdp_connection(t_lastspike - Time(Time::step(delay_)).get_ms());
}

/**
 * Send an event to the receiver of this connection.
 * \param e The event to send
 * \param p The port under which this connection is stored in the Connector.
 * \param t_lastspike Time point of last spike emitted
 */
inline
void STDPFACETSHWConnectionHom::send(Event& e, double_t t_lastspike, STDPFACETSHWHomCommonProperties &cp)
{
  // synapse STDP depressing/facilitation dynamics

  double_t t_spike = e.get_stamp().get_ms();

  //init the readout time
  if(init_flag_ == false){
    synapse_id_ = cp.no_synapses_;
    cp.no_synapses_++;
    cp.calc_readout_cycle_duration_();
    next_readout_time_ = int(synapse_id_ / cp.synapses_per_driver_) * cp.driver_readout_time_;
    std::cout << "init synapse " << synapse_id_ << " - first readout time: " << next_readout_time_ << std::endl;
    init_flag_ = true;
  }

  //STDP controller is processing this synapse (synapse driver)?
  if(t_spike > next_readout_time_)
  {
    //transform weight to discrete representation
    discrete_weight_ = weight_to_entry_(weight_, cp.weight_per_lut_entry_);

    if(cp.hardware_stage_ == 1){ //chip-based system STDP (common reset)
      //check whether difference of causal and acausal accumulated spike pairs > 0 and abs of difference > threshold
	  //std::cout << "ID: " << synapse_id_ <<  " spike: " << t_spike << ": " << fabs(a_causal_ - a_acausal_) << " >? " << a_th_causal_ << "; c/a " << a_causal_ << " " << a_acausal_ << std::endl;
      if(fabs(a_causal_ - a_acausal_) > a_th_causal_){
        if(a_causal_ - a_acausal_ > 0){ //causal?
          discrete_weight_ = facilitate_(discrete_weight_, cp);
        }else{ //acausal!
          discrete_weight_ = depress_(discrete_weight_, cp);
        }
        //reset both accumulations
        a_causal_ = 0;
        a_acausal_ = 0;
      }
    } else if(cp.hardware_stage_ == 2){ //wafer-scale system STDP (common reset)
      //check whether causal and acausal thresholds are crossed
      if(((a_causal_ > a_th_causal_) == true) && ((a_acausal_ > a_th_acausal_) == false)){
        discrete_weight_ = facilitate_(discrete_weight_, cp);
        //common reset
        a_causal_ = 0;
        a_acausal_ = 0;
      } else if(((a_acausal_ > a_th_acausal_) == true) && ((a_causal_ > a_th_causal_) == false)){
        discrete_weight_ = depress_(discrete_weight_, cp);
        //common reset
        a_causal_ = 0;
        a_acausal_ = 0;
      //if both thresholds are crossed, reset
      } else if(((a_causal_ > a_th_causal_) == true) && ((a_acausal_ > a_th_acausal_) == true)){
        a_causal_ = 0;
        a_acausal_ = 0;
      }
    } else if(cp.hardware_stage_ == 3){ //wafer-scale system STDP (independent reset)
      //check whether causal and acausal thresholds are crossed
      if(((a_causal_ > a_th_causal_) == true) && ((a_acausal_ > a_th_acausal_) == false)){
        discrete_weight_ = facilitate_(discrete_weight_, cp);
        a_causal_ = 0;
      } else if(((a_acausal_ > a_th_acausal_) == true) && ((a_causal_ > a_th_causal_) == false)){
        discrete_weight_ = depress_(discrete_weight_, cp);
        a_acausal_ = 0;
      } else if(((a_causal_ > a_th_causal_) == true) && ((a_acausal_ > a_th_acausal_) == true)){
        a_causal_ = 0;
        a_acausal_ = 0;
      }
    }

    while(t_spike > next_readout_time_){
      next_readout_time_ += cp.readout_cycle_duration_;
    }
    //std::cout << "synapse " << synapse_id_ << " updated at " << t_spike << ", next readout time: " << next_readout_time_ << std::endl;

    //back-transformation to continuous weight space
    weight_ = entry_to_weight_(discrete_weight_, cp.weight_per_lut_entry_);
  }

  // t_lastspike_ = 0 initially

  double_t dendritic_delay = Time(Time::step(delay_)).get_ms();

  //get spike history in relevant range (t1, t2] from post-synaptic neuron
  std::deque<histentry>::iterator start;
  std::deque<histentry>::iterator finish;
  target_->get_history(t_lastspike - dendritic_delay, t_spike - dendritic_delay,
			       &start, &finish);
  //facilitation due to post-synaptic spikes since last pre-synaptic spike
  double_t minus_dt = 0;
  double_t plus_dt = 0;

  if(start != finish) //take only first postspike after last prespike
  {
    minus_dt = t_lastspike - (start->t_ + dendritic_delay);
  }

  if(start != finish) //take only last postspike before current spike
  {
    finish--;
    plus_dt = (finish->t_ + dendritic_delay) - t_spike;
  }

  if(minus_dt != 0){
    a_causal_ += std::exp(minus_dt / cp.tau_plus_);
  }

  if(plus_dt != 0){
    a_acausal_ += std::exp(plus_dt / cp.tau_minus_);
  }

  e.set_receiver(*target_);
  e.set_weight(weight_);
  e.set_delay(delay_);
  e.set_rport(rport_);
  e();

  }
} // of namespace nest

#endif // of #ifndef STDP_CONNECTION_HOM_H
