/*
 *  iaf_cond_exp_sfa_rr.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2005-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef IAF_COND_EXP_SFA_RR_H
#define IAF_COND_EXP_SFA_RR_H

#include "config.h"

#ifdef HAVE_GSL

#include "nest.h"
#include "event.h"
#include "archiving_node.h"
#include "ring_buffer.h"
#include "connection.h"
#include "universal_data_logger.h"
#include "recordables_map.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>

/* BeginDocumentation
Name: iaf_cond_exp_sfa_rr - Simple conductance based leaky integrate-and-fire neuron model.

Description:
iaf_cond_exp_sfa_rr is an iaf_cond_exp_sfa_rr i.e. an implementation of a
spiking neuron using IAF dynamics with conductance-based synapses,
with additional spike-frequency adaptation and relative refractory
mechanisms as described in Dayan+Abbott, 2001, page 166.

As for the iaf_cond_exp_sfa_rr, Incoming spike events induce a post-synaptic
change  of  conductance  modelled  by an  exponential  function.  The
exponential function is  normalised such that an event  of weight 1.0
results in a peak current of 1 nS.

Outgoing spike events induce a change of the adaptation and relative
refractory conductances by q_sfa and q_rr, respectively.  Otherwise
these conductances decay exponentially with time constants tau_sfa
and tau_rr, respectively.

Parameters: 
The following parameters can be set in the status dictionary.

V_m        double - Membrane potential in mV 
E_L        double - Leak reversal potential in mV.
C_m        double - Capacity of the membrane in pF
t_ref      double - Duration of refractory period in ms. 
V_th       double - Spike threshold in mV.
V_reset    double - Reset potential of the membrane in mV.
E_ex       double - Excitatory reversal potential in mV.
E_in       double - Inhibitory reversal potential in mV.
g_L        double - Leak conductance in nS;
tau_syn_ex double - Time constant of the excitatory synaptic exponential function in ms.
tau_syn_in double - Time constant of the inhibitory synaptic exponential function in ms.
q_sfa      double - Outgoing spike activated quantal spike-frequency adaptation conductance increase in nS.
q_rr       double - Outgoing spike activated quantal relative refractory conductance increase in nS.
tau_sfa    double - Time constant of spike-frequency adaptation in ms.
tau_rr     double - Time constant of the relative refractory mechanism in ms.
E_sfa      double - spike-frequency adaptation conductance reversal potential in mV.
E_rr       double - relative refractory mechanism conductance reversal potential in mV.
I_e        double - an external stimulus current in pA.

Sends: SpikeEvent

Receives: SpikeEvent, CurrentEvent, DataLoggingRequest


References: 

Meffin, H., Burkitt, A. N., & Grayden, D. B. (2004). An analytical
model for the large, fluctuating synaptic conductance state typical of
neocortical neurons in vivo. J.  Comput. Neurosci., 16, 159–175.

Dayan, P. and Abbott, L. F. (2001). Theoretical Neuroscience, MIT
Press (p166)

Author: Sven Schrader, Eilif Muller

SeeAlso: iaf_cond_exp_sfa_rr, aeif_cond_alpha, iaf_psc_delta, iaf_psc_exp, iaf_cond_alpha
*/

namespace nest {

  using std::vector;

  /**
   * Function computing right-hand side of ODE for GSL solver.
   * @note Must be declared here so we can befriend it in class.
   * @note Must have C-linkage for passing to GSL. Internally, it is
   *       a first-class C++ function, but cannot be a member function
   *       because of the C-linkage.
   * @note No point in declaring it inline, since it is called
   *       through a function pointer.
   * @param void* Pointer to model neuron instance.
   */
  extern "C"
  int iaf_cond_exp_sfa_rr_dynamics(double, const double*, double*, void*);

  class iaf_cond_exp_sfa_rr: public Archiving_Node
  {
    
  public:
    
    iaf_cond_exp_sfa_rr();
    iaf_cond_exp_sfa_rr(const iaf_cond_exp_sfa_rr&);
    ~iaf_cond_exp_sfa_rr();

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
    void handle(DataLoggingRequest &); 
    
    port connect_sender(SpikeEvent &, port);
    port connect_sender(CurrentEvent &, port);
    port connect_sender(DataLoggingRequest &, port);
    
    void get_status(DictionaryDatum &) const;
    void set_status(const DictionaryDatum &);
    
  private:
    void init_state_(const Node& proto);
    void init_buffers_();
    void calibrate();
    void update(Time const &, const long_t, const long_t);

    // END Boilerplate function declarations ----------------------------

    // Friends --------------------------------------------------------

    // make dynamics function quasi-member
    friend int iaf_cond_exp_sfa_rr_dynamics(double, const double*, double*, void*);

    // The next two classes need to be friends to access the State_ class/member
    friend class RecordablesMap<iaf_cond_exp_sfa_rr>;
    friend class UniversalDataLogger<iaf_cond_exp_sfa_rr>;

  private:

    // ---------------------------------------------------------------- 

    //! Independent parameters
    struct Parameters_ {
      double_t V_th_;       //!< Threshold Potential in mV
      double_t V_reset_;    //!< Reset Potential in mV
      double_t t_ref_;      //!< Refractory period in ms
      double_t g_L;         //!< Leak Conductance in nS
      double_t C_m;         //!< Membrane Capacitance in pF
      double_t E_ex;        //!< Excitatory reversal Potential in mV
      double_t E_in;        //!< Inhibitory reversal Potential in mV
      double_t E_L;         //!< Leak reversal Potential (aka resting potential) in mV
      double_t tau_synE;    //!< Synaptic Time Constant Excitatory Synapse in ms
      double_t tau_synI;    //!< Synaptic Time Constant for Inhibitory Synapse in ms
      double_t I_e;         //!< Constant Current in pA
      double_t tau_sfa;     //!< spike-frequency adaptation (sfa) time constant
      double_t tau_rr;      //!< relative refractory (rr) time constant
      double_t E_sfa;       //!< spike-frequency adaptation (sfa) reversal Potential in mV
      double_t E_rr;        //!<  relative refractory (rr) reversal Potential in mV
      double_t q_sfa;       //!< spike-frequency adaptation (sfa) quantal conductance increase in nS 
      double_t q_rr;        //!< relative refractory (rr) quantal conductance increase in nS
  
      Parameters_();  //!< Sets default parameter values

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      void set(const DictionaryDatum&);  //!< Set values from dicitonary
    };

  public:
    // ---------------------------------------------------------------- 

    /**
     * State variables of the model.
     * @note Copy constructor and assignment operator required because
     *       of C-style array.
     */
    struct State_ {

      //! Symbolic indices to the elements of the state vector y
      enum StateVecElems
	{
	  V_M  = 0,           
	  G_EXC,     
	  G_INH,
	  G_SFA,
	  G_RR,
	  STATE_VEC_SIZE
	};

      double_t y_[STATE_VEC_SIZE];  //!< neuron state, must be C-array for GSL solver
      int_t    r_;     //!< number of refractory steps remaining

      State_(const Parameters_&);  //!< Default initialization
      State_(const State_&);
      State_& operator=(const State_&);

      void get(DictionaryDatum&) const;
      void set(const DictionaryDatum&, const Parameters_&);
    };    

  private:
    // ---------------------------------------------------------------- 

    /**
     * Buffers of the model.
     */
    struct Buffers_ {
      Buffers_(iaf_cond_exp_sfa_rr&);                   //!<Sets buffer pointers to 0
      Buffers_(const Buffers_&, iaf_cond_exp_sfa_rr&);  //!<Sets buffer pointers to 0

      //! Logger for all analog data
      UniversalDataLogger<iaf_cond_exp_sfa_rr> logger_;

      /** buffers and sums up incoming spikes/currents */
      RingBuffer spike_exc_;
      RingBuffer spike_inh_;
      RingBuffer currents_;

      /** GSL ODE stuff */
      gsl_odeiv_step*    s_;    //!< stepping function
      gsl_odeiv_control* c_;    //!< adaptive stepsize control function
      gsl_odeiv_evolve*  e_;    //!< evolution function
      gsl_odeiv_system   sys_;  //!< struct describing system
      
      // IntergrationStep_ should be reset with the neuron on ResetNetwork,
      // but remain unchanged during calibration. Since it is initialized with
      // step_, and the resolution cannot change after nodes have been created,
      // it is safe to place both here.
      double_t step_;           //!< step size in ms
      double   IntegrationStep_;//!< current integration time step, updated by GSL

      /** 
       * Input current injected by CurrentEvent.
       * This variable is used to transport the current applied into the
       * _dynamics function computing the derivative of the state vector.
       * It must be a part of Buffers_, since it is initialized once before
       * the first simulation, but not modified before later Simulate calls.
       */
      double I_stim_;
    };

     // ---------------------------------------------------------------- 

     /**
      * Internal variables of the model.
      */
     struct Variables_ { 
      int_t    RefractoryCounts_;
     };

    // Access functions for UniversalDataLogger -------------------------------
    
    //! Read out state vector elements, used by UniversalDataLogger
    template <State_::StateVecElems elem>
    double_t get_y_elem_() const { return S_.y_[elem]; }

    // ---------------------------------------------------------------- 

    Parameters_ P_;
    State_      S_;
    Variables_  V_;
    Buffers_    B_;

    //! Mapping of recordables names to access functions
    static RecordablesMap<iaf_cond_exp_sfa_rr> recordablesMap_;
  };

  
  inline
  port iaf_cond_exp_sfa_rr::check_connection(Connection& c, port receptor_type)
  {
    SpikeEvent e;
    e.set_sender(*this);
    c.check_event(e);
    return c.get_target()->connect_sender(e, receptor_type);
  }

  inline
  port iaf_cond_exp_sfa_rr::connect_sender(SpikeEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }
 
  inline
  port iaf_cond_exp_sfa_rr::connect_sender(CurrentEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }

  inline
  port iaf_cond_exp_sfa_rr::connect_sender(DataLoggingRequest& dlr, 
				      port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return B_.logger_.connect_logging_device(dlr, recordablesMap_);
  }
 
  inline
  void iaf_cond_exp_sfa_rr::get_status(DictionaryDatum &d) const
  {
    P_.get(d);
    S_.get(d);
    Archiving_Node::get_status(d);

    (*d)[names::recordables] = recordablesMap_.get_list();
  }

  inline
  void iaf_cond_exp_sfa_rr::set_status(const DictionaryDatum &d)
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

#endif //HAVE_GSL
#endif //IAF_COND_EXP_SFA_RR_H
