/*
 *  static_connection.h
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


/* BeginDocumentation
   Name: static_synapse_hom_wd - Synapse type for static connections with homogeneous weight and delay.
   
   Description:
     static_synapse_hom_wd does not support any kind of plasticity. It simply stores
     the parameters target, and receiver port for each connection and uses a common 
     weight and delay for all connections.

   Transmits: SpikeEvent, RateEvent, CurrentEvent, ConductanceEvent, DataLoggingRequest, DoubleDataEvent

   Parameters:
     No Parameters

   References:
     No References
   FirstVersion: April 2008
   Author: Susanne Kunkel, Moritz Helias
   SeeAlso: synapsedict, static_synapse
*/

#ifndef STATICCONNECTION_HOM_WD_H
#define STATICCONNECTION_HOM_WD_H

#include "connection_hom_wd.h"

namespace nest
{

/**
 * Class representing a static connection. A static connection has the properties weight, delay and receiver port.
 * This class also serves as the base class for dynamic synapses (like TsodyksConnection, STDPConnection).
 * A suitale Connector containing these connections can be obtained from the template GenericConnector.
 */
class StaticConnectionHomWD : public ConnectionHomWD
{

 public:

  /**
   * Default Constructor.
   * Sets default values for all parameters. Needed by GenericConnectorModel.
   */
  StaticConnectionHomWD() : ConnectionHomWD() {}

  /**
   * Default Destructor.
   */
  ~StaticConnectionHomWD() {}

  // overloaded for all supported event types
  using Connection::check_event;
  void check_event(SpikeEvent&) {}
  void check_event(RateEvent&) {}
  void check_event(DataLoggingRequest&) {}
  void check_event(CurrentEvent&) {}
  void check_event(ConductanceEvent&) {}
  void check_event(DoubleDataEvent&) {}
  void check_event(DSSpikeEvent&) {}
  void check_event(DSCurrentEvent&) {}

};

} // namespace

#endif /* #ifndef STATICCONNECTION_HOM_WD_H */
