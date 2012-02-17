/*
 *  connection_manager.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2005 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <vector>
#include <limits>

#include "nest.h"
#include "model.h"
#include "dictutils.h"
#include "connector.h"
#include "nest_time.h"
#include "nest_timeconverter.h"
#include "arraydatum.h"

#include "sparsetable.h"

namespace nest
{

class Network;
class ConnectorModel;

/**
 * Manages the available connection prototypes and connections. It provides
 * the interface to establish and modify connections between nodes.
 */
class ConnectionManager
{
  struct syn_id_connector
  {
    index syn_id;
    Connector* connector;
  };

  typedef std::vector< syn_id_connector > tVConnector;
  typedef google::sparsetable< tVConnector > tVVConnector;
  typedef std::vector< tVVConnector > tVVVConnector;

public:
  ConnectionManager(Network& net);
  ~ConnectionManager();

  void init(Dictionary*);
  void reset();
  
  /**
   * Register a synapse type. This is called by Network::register_synapse_prototype.
   * Returns an id, which is needed to unregister the prototype later.
   */
  index register_synapse_prototype(ConnectorModel * cf);

  /**
   * Checks, whether connections of the given type were created
   */
  bool synapse_prototype_in_use(index syn_id);

  /**
   * Unregister a previously registered synapse prototype.
   */
  void unregister_synapse_prototype(index syn_id);

  /**
   * Try, if it is possible to unregister synapse prototype.
   * Throw exception, if not possible.
   */
  void try_unregister_synapse_prototype(index syn_id);

  /** 
   * Add ConnectionManager specific stuff to the root status dictionary
   */
  void get_status(DictionaryDatum& d) const;

  // aka SetDefaults for synapse models
  void set_prototype_status(index syn_id, const DictionaryDatum& d);
  // aka GetDefaults for synapse models
  DictionaryDatum get_prototype_status(index syn_id) const;

  // aka conndatum GetStatus
  DictionaryDatum get_synapse_status(index gid, index syn_id, port p, thread tid);
  // aka conndatum SetStatus
  void set_synapse_status(index gid, index syn_id, port p, thread tid, const DictionaryDatum& d);

  DictionaryDatum get_connector_status(const Node& node, index syn_id);
  DictionaryDatum get_connector_status(index gid, index syn_id);
  void set_connector_status(Node& node, index syn_id, thread tid, const DictionaryDatum& d);
  
  ArrayDatum find_connections(DictionaryDatum params);
  void find_connections(ArrayDatum& connectome, thread t, index source, index syn_id, DictionaryDatum params);

  // aka CopyModel for synapse models
  index copy_synapse_prototype(index old_id, std::string new_name);

  bool has_user_prototypes() const;

  bool get_user_set_delay_extrema() const;
  
  size_t get_num_connections() const;

  const Time get_min_delay() const;
  const Time get_max_delay() const;

  /**
   * Connect is used to establish a connection between a sender and
   * receiving node.
   * \param s A reference to the sending Node.
   * \param r A reference to the receiving Node.
   * \param t The thread of the target node.
   * \param syn The synapse model to use.
   * \returns The receiver port number for the new connection
   */ 
  void connect(Node& s, Node& r, index s_gid, thread tid, index syn);
  void connect(Node& s, Node& r, index s_gid, thread tid, double_t w, double_t d, index syn);
  void connect(Node& s, Node& r, index s_gid, thread tid, DictionaryDatum& p, index syn);
  /** 
   * Experimental bulk connector. see documentation in network.h
   */
  bool connect(DictionaryDatum &d);

  void send(thread t, index sgid, Event& e);

  /**
   * Resize the structures for the Connector objects if necessary.
   * This function should be called after number of threads, min_delay, max_delay, 
   * and time representation have been changed in the scheduler. 
   * The TimeConverter is used to convert times from the old to the new representation.
   * It is also forwarding the calibration
   * request to all ConnectorModel objects.
   */  
  void calibrate(const TimeConverter &);

private:

  std::vector<ConnectorModel*> pristine_prototypes_; //!< The list of clean synapse prototypes
  std::vector<ConnectorModel*> prototypes_;          //!< The list of available synapse prototypes

  Network& net_;            //!< The reference to the network
  Dictionary* synapsedict_; //!< The synapsedict (owned by the network)

  /**
   * A 3-dim structure to hold the Connector objects which in turn hold the connection
   * information.
   * - First dim: A std::vector for each local thread
   * - Second dim: A std::vector for each node on each thread
   * - Third dim: A std::vector for each synapse prototype, holding the Connector objects
   */
  tVVVConnector connections_;
  
  void init_();
  void delete_connections_();
  void clear_prototypes_();
  
  index validate_connector(thread tid, index gid, index syn_id);

  /**
   * Return pointer to protoype for given synapse id.
   * @throws UnknownSynapseType
   */
  const ConnectorModel& get_synapse_prototype(index syn_id) const;

  /**
   * Asserts validity of synapse index, otherwise throws exception.
   * @throws UnknownSynapseType
   */
  void assert_valid_syn_id(index syn_id) const;

  /**
   * For a given thread, source gid and synapse id, return the correct
   * index (syn_vec_index) into the connection store, so that one can
   * access the corresponding connector using
   * \code
       connections_[tid][gid][syn_vec_index].
     \endcode
   * @returns the index of the Connector or -1 if it does not exist.
   */  
  int get_syn_vec_index(thread tid, index gid, index syn_id) const;
};

inline
const ConnectorModel& ConnectionManager::get_synapse_prototype(index syn_id) const
{
  assert_valid_syn_id(syn_id);
  return *(prototypes_[syn_id]);
}

inline
void ConnectionManager::assert_valid_syn_id(index syn_id) const
{
  if (syn_id >= prototypes_.size() || prototypes_[syn_id] == 0)
    throw UnknownSynapseType(syn_id);
}

inline
bool ConnectionManager::has_user_prototypes() const
{
  return prototypes_.size() > pristine_prototypes_.size();
}

inline
int ConnectionManager::get_syn_vec_index(thread tid, index gid, index syn_id) const
{
  if (static_cast<size_t>(tid) >= connections_.size() || gid >= connections_[tid].size() || connections_[tid][gid].size() == 0)
    return -1;

  index syn_vec_index = 0;
  while ( syn_vec_index < connections_[tid][gid].size() && connections_[tid][gid][syn_vec_index].syn_id != syn_id )
    syn_vec_index++;

  if (syn_vec_index == connections_[tid][gid].size())
    return -1;
  
  return syn_vec_index;  
}

} // namespace

#endif /* #ifndef CONNECTION_MANAGER_H */
