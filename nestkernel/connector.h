/*
 *  connector.h
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

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "node.h"
#include "event.h"
#include "exceptions.h"
#include "spikecounter.h"

class Dictionary;

namespace nest
{

class TimeConverter;

/**
 * Pure abstract base class for all Connectors. It constitutes
 * the interface between the ConnectionManager and a Connector.
 */
class Connector
{
 public:
  virtual ~Connector() {}
  virtual void register_connection(Node&, Node&) = 0;
  virtual void register_connection(Node&, Node&, double_t, double_t) = 0;
  virtual void register_connection(Node&, Node&, DictionaryDatum&) = 0;
  virtual std::vector<long>* find_connections(DictionaryDatum) const = 0;
  virtual void get_status(DictionaryDatum & d) const = 0;
  virtual void set_status(const DictionaryDatum & d) = 0;
  virtual void get_synapse_status(DictionaryDatum & d, port p) const = 0;
  virtual void set_synapse_status(const DictionaryDatum & d, port p) = 0;
  virtual void send(Event& e) = 0;
  virtual void calibrate(const TimeConverter &) = 0;
  virtual void trigger_update_weight(const std::vector<spikecounter> &){};
};
 

}

#endif /* #ifndef CONNECTOR_H */
