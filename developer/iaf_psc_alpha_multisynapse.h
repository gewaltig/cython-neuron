/*
 *  iaf_psc_alpha_multisynapse.h
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

#ifndef IAF_PSC_ALPHA_MULTISYNAPSE_H
#define IAF_PSC_ALPHA_MULTISYNAPSE_H

#include "nest.h"
#include "event.h"
#include "archiving_node.h"
#include "ring_buffer.h"
#include "connection.h"
#include "analog_data_logger.h"



namespace nest{
  class Network;

  /* BeginDocumentation
Name: iaf_psc_alpha_multisynapse - Leaky integrate-and-fire neuron model with multiple ports.

Description:

  iaf_psc_alpha_multisynapse is a direct extension of iaf_psc_alpha.
  On the postsynapic side, there can be arbitrarily many synaptic
  time constants (iaf_psc_alpha has exactly two: tau_ex and tau_in).
 
  This can be reached by specifying separate receptor ports, each for 
  a different time constant. The port number has to match the respective
  "receptor_type" in the connectors.

Sends: SpikeEvent

Author:  Schrader, adapted from iaf_psc_alpha
SeeAlso: iaf_psc_alpha, iaf_psc_delta, iaf_psc_exp, iaf_cond_exp
*/

  /**
   * Leaky integrate-and-fire neuron with alpha-shaped PSCs.
   */
  class iaf_psc_alpha_multisynapse : public Archiving_Node
  {
    
  public:        
    
    typedef Node base;
    
    iaf_psc_alpha_multisynapse();
    iaf_psc_alpha_multisynapse(const iaf_psc_alpha_multisynapse&);

    /**
     * Import sets of overloaded virtual functions.
     * We need to explicitly include sets of overloaded
     * virtual functions into the current scope.
     * According to the SUN C++ FAQ, this is the correct
     * way of doing things, although all other compilers
     * happily live without.
     */

    using Node::connect_sender;
    using Node::handle;

    port check_connection(Connection&, port);
    
    void handle(SpikeEvent &);
    void handle(CurrentEvent &);
    void handle(PotentialRequest &);
    void handle(SynapticCurrentRequest &);

    port connect_sender(SpikeEvent&, port);
    port connect_sender(CurrentEvent&, port);
    port connect_sender(PotentialRequest&, port);
    port connect_sender(SynapticCurrentRequest &, port);

    void get_status(DictionaryDatum &) const;
    void set_status(const DictionaryDatum &);

 /*  protected: */
    
/*     void init(); */
/*     void calibrate(); */
/*     void update(Time const &, const long_t, const long_t); */
  

  private:
    
    void init_node_(const Node& proto);
    void init_state_(const Node& proto);
    void init_buffers_();
    void calibrate();
    
    void update(Time const &, const long_t, const long_t);

    
    // ---------------------------------------------------------------- 
    
    /** 
     * Independent parameters of the model. 
     */
    struct Parameters_ {
      
      /** Membrane time constant in ms. */
      double_t Tau_; 
      
      /** Membrane capacitance in pF. */
      double_t C_;
      
      /** Refractory period in ms. */
      double_t TauR_;
      
      /** Resting potential in mV. */
      double_t U0_;
      
      /** External current in pA */
      double_t I_e_;
      
      /** reset value of the membrane potential */
      double_t V_reset_;
      
      /** Threshold, RELATIVE TO RESTING POTENTAIL(!).
	  I.e. the real threshold is (U0_+Theta_). */
      double_t Theta_;
      
      /** Lower bound, RELATIVE TO RESTING POTENTAIL(!).
	  I.e. the real lower bound is (LowerBound_+Theta_). */
      double_t LowerBound_;
      
      /** Time constant of excitatory synaptic current in ms. */
      std::vector<double_t> tau_syn_;
      
      // type is long because other types are not put through in GetStatus
      std::vector<long> receptor_types_;
      unsigned int      num_of_receptors_;

      Parameters_();  //!< Sets default parameter values

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      void set(const DictionaryDatum&);  //!< Set values from dicitonary
      
    }; // Parameters_

    // ---------------------------------------------------------------- 

    /**
     * State variables of the model.
     */
    struct State_ {
      
      double_t     y0_; //!< Constant current
      std::vector<double_t>     y1_syn_;  
      std::vector<double_t>     y2_syn_;
      double_t     y3_; //!< This is the membrane potential RELATIVE TO RESTING POTENTIAL.
      
      int_t       r_; //!< Number of refractory steps remaining

      State_();  //!< Default initialization
      
      void get(DictionaryDatum&, const Parameters_&) const;
      void set(const DictionaryDatum&, const Parameters_&);

    }; // State_

    // ---------------------------------------------------------------- 

    /**
     * Buffers of the model.
     */
    struct Buffers_ {
      
      /** buffers and summs up incoming spikes/currents */
      std::vector<RingBuffer> spikes_;
      RingBuffer currents_;  
      
      /** Buffer for membrane potential. */
      AnalogDataLogger<PotentialRequest> potentials_;

      // Buffers for the currents
      AnalogDataLogger<SynapticCurrentRequest> syncurrents_;
      

    }; //Buffers_

    // ---------------------------------------------------------------- 

    /**
     * Internal variables of the model.
     */
    struct Variables_ { 
      
      std::vector<double_t> PSCInitialValues_;
      int_t       RefractoryCounts_;
      
      std::vector<double_t> P11_syn_;
      std::vector<double_t> P21_syn_;
      std::vector<double_t> P22_syn_;
      std::vector<double_t> P31_syn_;
      std::vector<double_t> P32_syn_;
      
      double_t P30_;
      double_t P33_;

      unsigned int      receptor_types_size_;

    }; // Variables
    
    // ---------------------------------------------------------------- 
    
    /**
     * @defgroup iaf_psc_alpha_data
     * Instances of private data structures for the different types
     * of data pertaining to the model.
     * @note The order of definitions is important for speed.
     * @{
     */   
    Parameters_ P_;
    State_      S_;
    Variables_  V_;
    Buffers_    B_;
    /** @} */

  };

inline
port nest::iaf_psc_alpha_multisynapse::check_connection(Connection& c, port receptor_type)
{

  SpikeEvent e;
  e.set_sender(*this);
  c.check_event(e);
  return c.get_target()->connect_sender(e, receptor_type);
}

inline
port iaf_psc_alpha_multisynapse::connect_sender(CurrentEvent&, port receptor_type)
{
  if (receptor_type != 0)
    throw UnknownReceptorType(receptor_type, get_name());
  return 0;
}
 
inline
port iaf_psc_alpha_multisynapse::connect_sender(PotentialRequest& pr, port receptor_type)
{
  if (receptor_type != 0)
    throw UnknownReceptorType(receptor_type, get_name());
  B_.potentials_.connect_logging_device(pr);
  return 0;
}

inline
port iaf_psc_alpha_multisynapse::connect_sender(SynapticCurrentRequest& sr, port receptor_type)
{
  if (receptor_type != 0)
    throw UnknownReceptorType(receptor_type, get_name());
  B_.syncurrents_.connect_logging_device(sr);
  return 0;
}

inline
void iaf_psc_alpha_multisynapse::get_status(DictionaryDatum &d) const
{
  P_.get(d);
  S_.get(d, P_);
  Archiving_Node::get_status(d);
}

inline
void iaf_psc_alpha_multisynapse::set_status(const DictionaryDatum &d)
{
  Parameters_ ptmp = P_;  // temporary copy in case of errors
  ptmp.set(d);                       // throws if BadProperty
  State_      stmp = S_;  // temporary copy in case of errors
  stmp.set(d, ptmp);                 // throws if BadProperty

  // We now know that (ptmp, stmp) are consistent. We do not 
  // write them back to (P_, S_) before we are also sure that 
  // the properties to be set in the parent class are internally 
  // consistent.
  Archiving_Node::set_status(d);

  // if we get here, temporaries contain consistent set of properties
  P_ = ptmp;
  S_ = stmp;
}

} // namespace

#endif /* #ifndef IAF_PSC_ALPHA_MULTISYNAPSE_H */
