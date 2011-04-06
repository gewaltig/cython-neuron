/*
 *  step_current_generator.h
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


/*BeginDocumentation
  Name: step_current_generator - provides a piecewise constant DC input current
  
  Description:
  The dc_generator provides a piecewise constant DC input to the
  connected node(s).  The amplitude of the current is changed at the
  specified times. The unit of the current is pA.

 Parameters: 
     The following parameters can be set in the status dictionary:
     amplitude_times   list of doubles - Times at which current changes in ms
     amplitude_values  list of doubles - Amplitudes of step current current in pA 

  Examples:
    The current can be altered in the following way:
    /step_current_generator Create /sc Set
    sc << /amplitude_times [0.2 0.5] /amplitude_values [2.0 4.0] >> SetStatus

    The amplitude of the DC will be 0.0 pA in the time interval [0, 0.2),
    2.0 pA in the interval [0.2, 0.5) and 4.0 from then on.

  Sends: CurrentEvent
    
  Author: Jochen Martin Eppler, Jens Kremkow

  SeeAlso: ac_generator, dc_generator, step_current_generator, Device, StimulatingDevice
*/ 

#ifndef STEP_CURRENT_GENERATOR_H
#define STEP_CURRENT_GENERATOR_H

#include <vector>
#include "nest.h"
#include "event.h"
#include "node.h"
#include "ring_buffer.h"
#include "connection.h"
#include "stimulating_device.h"

namespace nest
{
  class step_current_generator : public Node
  {
    
  public:        
    
    step_current_generator();
    step_current_generator(const step_current_generator&);

    bool has_proxies() const {return false;}


    port check_connection(Connection&, port);
    
    void get_status(DictionaryDatum &) const;
    void set_status(const DictionaryDatum &);

  private:
    
    void init_node_(const Node&);
    void init_state_(const Node&);
    void init_buffers_();
    void calibrate();
    
    void update(Time const &, const long_t, const long_t);
    
    struct Buffers_;
    
    /**
     * Store independent parameters of the model.
     */
    struct Parameters_ {
      std::vector<double_t> amp_times_;
      std::vector<double_t> amp_values_;
      
      Parameters_();  //!< Sets default parameter values
      Parameters_(const Parameters_&, Buffers_&);

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      void set(const DictionaryDatum&, Buffers_&);  //!< Set values from dicitonary
    };

    // ------------------------------------------------------------
    
    struct Buffers_ {
      size_t   idx_;  //!< index of current amplitude
      double_t amp_;  //!< current amplitude
    };
    
    // ------------------------------------------------------------

    StimulatingDevice<CurrentEvent> device_;
    Parameters_ P_;
    Buffers_    B_;
  };
  
  inline  
  port step_current_generator::check_connection(Connection& c, port receptor_type)
  {
    CurrentEvent e;
    e.set_sender(*this);
    c.check_event(e);
    return c.get_target()->connect_sender(e, receptor_type);
  }
  
  inline
  void step_current_generator::get_status(DictionaryDatum &d) const
  {
    P_.get(d);
    device_.get_status(d);
  }

  inline
  void step_current_generator::set_status(const DictionaryDatum &d)
  {
    Parameters_ ptmp = P_;  // temporary copy in case of errors
    ptmp.set(d, B_);               // throws if BadProperty

    // We now know that ptmp is consistent. We do not write it back
    // to P_ before we are also sure that the properties to be set
    // in the parent class are internally consistent.
    device_.set_status(d);

    // if we get here, temporaries contain consistent set of properties
    P_ = ptmp;
  }
  
  
} // namespace

#endif /* #ifndef STEP_CURRENT_GENERATOR_H */
