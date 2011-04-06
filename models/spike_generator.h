/*
 *  spike_generator.h
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

#ifndef SPIKE_GENERATOR_H
#define SPIKE_GENERATOR_H


#include <vector>
#include "nest.h"
#include "event.h"
#include "node.h"
#include "scheduler.h"
#include "stimulating_device.h"
#include "connection.h"

namespace nest
{
  /*BeginDocumentation
    Name: spike_generator - A device which generates spikes from an array with spike-times.

    Synopsis: spike_generator Create -> gid

    Description: 
    A spike generator can be used to generate spikes at specific times which
    are given to the spike generator as an array.
    
    Spike-times are given in milliseconds, and must be sorted with the
    earliest spike first.

    If the flag precise_times is set to false, then the spike times 
    need to be conform to the currently set simulation resolution. Spike times that
    deviate from this time grid will be rounded to the grid if they are closer
    than tic/2 otherwise they cause an exception.
    If the flag precise_times is set to true, spike times are converted into
    step and offset. No exceptions are generated.

    Multiple occurences of the same time indicate that more than one event is to be 
    generated at this particular time.

    Additionally, spike_weights can be set. This also is an array, which contains one
    weight value per spike time. If set, the spikes are delivered with the respective
    weight instead of the weight of the connection. To disable this functionality, the
    spike_weights array can be set to an empty array.

    Example:
    spikegenerator << /spike_times [1.0 2.0 3.0] >> SetStatus
    
    Instructs the spike generator to generate events at 1.0, 2.0, and 3.0 milliseconds, 
    relative to the device-timer origin. 

    spikegenerator << /spike_times [1.0 2.0] /spike_weights [5.0 -8.0] >> SetStatus

    Instructs the spike generator to generate an event with weight 5.0 at 1.0 ms, and
    an event with weight -8.0 at 2.0 ms, relative to the device-timer origin.

    spikegenerator << /spike_weights [] >> SetStatus

    Instructs the spike generator to generate events at 1.0, 2.0, and 3.0 milliseconds, 
    and use the weight of the connection. 

    Parameters:
    The following properties can be set in the status dictionary.

	 origin         double - Time origin for device timer in ms
	 start          double - earliest possible time stamp of a spike to be emitted in ms
	 stop           double - earliest time stamp of a potential spike event that is not emitted in ms 
	 spike_times    double array - spike-times in ms
         spike_weights  double array - corrsponding spike-weights, the unit depends on the receiver.

    Sends: SpikeEvent
         
    Author: Gewaltig, Diesmann, Eppler

    SeeAlso: Device, StimulatingDevice, testsuite::test_spike_generator
  */
  
  /**
   * Emit spikes at given times.
   *
   * This emits spikes with offsets, i.e., is useful also for precise spikes.
   *
   * @ingroup Devices
   */
  class spike_generator : public Node
  {
    
  public:    
    spike_generator();
    spike_generator(const spike_generator&);

    bool has_proxies() const {return false;} 

    port check_connection(Connection&, port);
    
    void get_status(DictionaryDatum &) const;
    void set_status(const DictionaryDatum &);

    using Node::event_hook;
    void event_hook(DSSpikeEvent&);

  private:
    
    void init_node_(const Node&);
    void init_state_(const Node&);
    void init_buffers_();
    void calibrate();
    
    void update(Time const &, const long_t, const long_t);

    // ------------------------------------------------------------

    struct State_ {
      size_t position_;  //!< index of next spike to deliver

      State_();  //!< Sets default state value
    };

    // ------------------------------------------------------------

    struct Parameters_ {
      bool precise_times_;           //!< Interpret spike times as precise, i.e. send as step and offset
      
      std::vector<double> spike_times_; //!< Spike times as Time object and precise offset, rel to origin_
      std::vector<double> spike_weights_;  //!< Spike weights as double

      Parameters_();  //!< Sets default parameter values
      Parameters_(const Parameters_&);  //!< Recalibrate all times

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      
      /**
       * Set values from dicitonary.
       * @note State is passed so that the position can be reset if the 
       *       spike_times_ or spike_weights_ vector has been filled with
       *       new data, or if the origin was reset.
       */
      void set(const DictionaryDatum&, State_&);  
    };
        
    // ------------------------------------------------------------

    StimulatingDevice<SpikeEvent> device_;
    
    Parameters_ P_;
    State_      S_;
  };

inline  
port spike_generator::check_connection(Connection& c, port receptor_type)
{
  SpikeEvent e;
  e.set_sender(*this);
  c.check_event(e);
  return c.get_target()->connect_sender(e, receptor_type);
}

inline
void spike_generator::get_status(DictionaryDatum &d) const
{
  P_.get(d);
  device_.get_status(d);
}

inline
void spike_generator::set_status(const DictionaryDatum &d)
{
  Parameters_ ptmp = P_;  // temporary copy in case of errors
  ptmp.set(d, S_);        // throws if BadProperty

  // We now know that ptmp is consistent. We do not write it back
  // to P_ before we are also sure that the properties to be set
  // in the parent class are internally consistent.
  device_.set_status(d);

  // if we get here, temporary contains consistent set of properties
  P_ = ptmp;
}


} // namespace

#endif /* #ifndef SPIKE_GENERATOR_H */
