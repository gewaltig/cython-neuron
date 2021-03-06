/*
 *  iaf_chxk_2008.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2005-2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef IAF_CHXK_2008_H
#define IAF_CHXK_2008_H

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
Name: iaf_chxk_2008 - Conductance based leaky integrate-and-fire neuron model used in
Casti et al 2008.

Description:
iaf_chxk_2008 is an implementation of a spiking neuron using IAF dynamics with
conductance-based synapses. It is modeled after iaf_cond_alpha with the addition
of after hyperpolarization current instead of a membrane potential reset.
Incoming spike events induce a post-synaptic change
of conductance modeled by an alpha function. The alpha function
is normalized such that an event of weight 1.0 results in a peak current of 1 nS
at t = tau_syn.

Parameters: 
The following parameters can be set in the status dictionary.

V_m        double - Membrane potential in mV 
E_L        double - Leak reversal potential in mV.
C_m        double - Capacity of the membrane in pF
V_th       double - Spike threshold in mV.
E_ex       double - Excitatory reversal potential in mV.
E_in       double - Inhibitory reversal potential in mV.
g_L        double - Leak conductance in nS.
tau_ex     double - Rise time of the excitatory synaptic alpha function in ms.
tau_in     double - Rise time of the inhibitory synaptic alpha function in ms.
I_e        double - Constant input current in pA.
tau_ahp    double - Afterhyperpolarization (AHP) time constant in ms.
E_ahp	   double - AHP potential in mV.
g_ahp	   double - AHP conductance in nS.
ahp_bug	   bool   - Defaults to false. If true, behaves like original model implementation.
Sends: SpikeEvent

Receives: SpikeEvent, CurrentEvent

Author: Heiberg

SeeAlso: iaf_cond_alpha
*/

namespace nest
{
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
  int iaf_chxk_2008_dynamics (double, const double*, double*, void*);

  /**
   * Integrate-and-fire neuron model with two conductance-based synapses.
   */
  class iaf_chxk_2008 : public Archiving_Node
  {
    
    // Boilerplate function declarations --------------------------------

  public:
    
    iaf_chxk_2008();
    iaf_chxk_2008(const iaf_chxk_2008&);
    ~iaf_chxk_2008();

    /*
     * Import all overloaded virtual functions that we
     * override in this class.  For background information, 
     * see http://www.gotw.ca/gotw/005.htm.
     */

    using Node::connect_sender;
    using Node::handle;

    port check_connection(Connection&, port);

    bool is_off_grid() const {return true;}  // uses off_grid events
    port connect_sender(SpikeEvent &, port);
    port connect_sender(CurrentEvent &, port);
    port connect_sender(DataLoggingRequest &, port);
    
    void handle(SpikeEvent &);
    void handle(CurrentEvent &);
    void handle(DataLoggingRequest &); 
        
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
    friend int iaf_chxk_2008_dynamics(double, const double*, double*, void*);

    // The next two classes need to be friends to access the State_ class/member
    friend class RecordablesMap<iaf_chxk_2008>;
    friend class UniversalDataLogger<iaf_chxk_2008>;

  private:

    // Parameters class ------------------------------------------------- 

    //! Model parameters
    struct Parameters_ {
      double_t V_th;        //!< Threshold Potential in mV
      double_t g_L;         //!< Leak Conductance in nS
      double_t C_m;         //!< Membrane Capacitance in pF
      double_t E_ex;        //!< Excitatory reversal Potential in mV
      double_t E_in;        //!< Inhibitory reversal Potential in mV
      double_t E_L;         //!< Leak reversal Potential (a.k.a. resting potential) in mV
      double_t tau_synE;    //!< Synaptic Time Constant Excitatory Synapse in ms
      double_t tau_synI;    //!< Synaptic Time Constant for Inhibitory Synapse in ms
      double_t I_e;         //!< Constant Current in pA
      double_t tau_ahp;		//!< Afterhyperpolarization (AHP) time constant
      double_t g_ahp;		//!< AHP conductance
      double_t E_ahp;		//!< AHP potential
      bool ahp_bug;			//!< If true, discard AHP conductance value from previous spikes
      Parameters_();        //!< Set default parameter values

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      void set(const DictionaryDatum&);  //!< Set values from dictionary
    };
    
    // State variables class -------------------------------------------- 

    /**
     * State variables of the model.
     * 
     * State variables consist of the state vector for the subthreshold
     * dynamics and the refractory count. The state vector must be a
     * C-style array to be compatible with GSL ODE solvers.
     *
     * @note Copy constructor and assignment operator are required because
     *       of the C-style array.
     */
  public:
    struct State_ {
      
      //! Symbolic indices to the elements of the state vector y
      enum StateVecElems { V_M = 0,
			   DG_EXC, G_EXC,     
			   DG_INH, G_INH,
			   DG_AHP, G_AHP,
			   STATE_VEC_SIZE };

      //! state vector, must be C-array for GSL solver
      double_t y[STATE_VEC_SIZE];
  
      //!< number of refractory steps remaining
      int_t    r;

      State_(const Parameters_&);  //!< Default initialization
      State_(const State_&);
      State_& operator=(const State_&);

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary

      /**
       * Set state from values in dictionary.
       * Requires Parameters_ as argument to, e.g., check bounds.'
       */
      void set(const DictionaryDatum&, const Parameters_&);
    };    
  private:

    // Buffers class -------------------------------------------------------- 

    /**
     * Buffers of the model.
     * Buffers are on par with state variables in terms of persistence,
     * i.e., initialized only upon first Simulate call after ResetKernel
     * or ResetNetwork, but are implementation details hidden from the user.
     */
    struct Buffers_ {
      Buffers_(iaf_chxk_2008&); //!<Sets buffer pointers to 0
      Buffers_(const Buffers_&, iaf_chxk_2008&); //!<Sets buffer pointers to 0

      //! Logger for all analog data
      UniversalDataLogger<iaf_chxk_2008> logger_;

      /** buffers and sums up incoming spikes/currents */
      RingBuffer spike_exc_;
      RingBuffer spike_inh_;
      RingBuffer currents_;

      /* GSL ODE stuff */
      gsl_odeiv_step*    s_;    //!< stepping function
      gsl_odeiv_control* c_;    //!< adaptive step size control function
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
      double_t I_stim_;
    };
    
    // Variables class ------------------------------------------------------- 
    
    /**
     * Internal variables of the model.
     * Variables are re-initialized upon each call to Simulate.
     */
    struct Variables_ { 
      /**
       * Impulse to add to DG_EXC on spike arrival to evoke unit-amplitude
       * conductance excursion.
       */
      double_t PSConInit_E; 
      
      /**
       * Impulse to add to DG_INH on spike arrival to evoke unit-amplitude
       * conductance excursion.
       */
      double_t PSConInit_I;    
      
      /**
       * Impulse to add to DG_AHP on spike generation to evoke unit-amplitude
       * conductance excursion.
       */
      double_t PSConInit_AHP;
    };
    
    // Access functions for UniversalDataLogger -------------------------------
    
    //! Read out state vector elements, used by UniversalDataLogger
    template <State_::StateVecElems elem>
    double_t get_y_elem_() const { return S_.y[elem]; }
    
    //! Read out remaining refractory time, used by UniversalDataLogger
    double_t get_r_() const { return Time::get_resolution().get_ms() * S_.r; }

	double_t get_I_syn_exc_() const { return S_.y[State_::G_EXC] * (S_.y[State_::V_M] - P_.E_ex); }
	double_t get_I_syn_inh_() const { return S_.y[State_::G_INH] * (S_.y[State_::V_M] - P_.E_in); }
	double_t get_I_ahp_() const { return S_.y[State_::G_AHP] * (S_.y[State_::V_M] - P_.E_ahp); }


    // Data members ----------------------------------------------------------- 

    // keep the order of these lines, seems to give best performance
    Parameters_ P_;
    State_      S_;
    Variables_  V_;
    Buffers_    B_;

    //! Mapping of recordables names to access functions
    static RecordablesMap<iaf_chxk_2008> recordablesMap_;
  };
  

  // Boilerplate inline function definitions ----------------------------------

  inline
  port iaf_chxk_2008::check_connection(Connection& c, port receptor_type)
  {
    SpikeEvent e;
    e.set_sender(*this);
    c.check_event(e);
    return c.get_target()->connect_sender(e, receptor_type);
  }

  inline
  port iaf_chxk_2008::connect_sender(SpikeEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }
 
  inline
  port iaf_chxk_2008::connect_sender(CurrentEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }
 
  inline
  port iaf_chxk_2008::connect_sender(DataLoggingRequest& dlr,
				      port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return B_.logger_.connect_logging_device(dlr, recordablesMap_);
  }

  inline
  void iaf_chxk_2008::get_status(DictionaryDatum &d) const
  {
    P_.get(d);
    S_.get(d);
    Archiving_Node::get_status(d);

    (*d)[names::recordables] = recordablesMap_.get_list();
  }

  inline
  void iaf_chxk_2008::set_status(const DictionaryDatum &d)
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
#endif //IAF_CHXK_2008_H
