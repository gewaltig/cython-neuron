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
  Name: static_synapse - Synapse type for static connections.

  Description:
   static_synapse does not support any kind of plasticity. It simply stores
   the parameters target, weight, delay and receiver port for each connection.

  FirstVersion: October 2005
  Author: Jochen Martin Eppler, Moritz Helias

  Transmits: SpikeEvent, RateEvent, CurrentEvent, ConductanceEvent, DoubleDataEvent, DataLoggingRequest
  
  Remarks: Refactored for new connection system design, March 2007

  SeeAlso: synapsedict, tsodyks_synapse, stdp_synapse
*/

#ifndef STATICCONNECTION_H
#define STATICCONNECTION_H

#include "connection_het_wd.h"

namespace nest
{

/**
 * Class representing a static connection. A static connection has the properties weight, delay and receiver port.
 * This class also serves as the base class for dynamic synapses (like TsodyksConnection, STDPConnection).
 * A suitale Connector containing these connections can be obtained from the template GenericConnector.
 */
class StaticConnection : public ConnectionHetWD
{

 public:

  /**
   * Default Constructor.
   * Sets default values for all parameters. Needed by GenericConnectorModel.
   */
  StaticConnection() : ConnectionHetWD() {}

  /**
   * Default Destructor.
   */
  ~StaticConnection() {}

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

#endif /* #ifndef STATICCONNECTION_H */
