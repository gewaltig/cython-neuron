/*
 *  connection_manager.cpp
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

#include "connection_manager.h"
#include "connector_model.h"
#include "network.h"
#include "nest_time.h"
#include "connectiondatum.h"

namespace nest
{

ConnectionManager::ConnectionManager(Network& net)
        : net_(net)
{}

ConnectionManager::~ConnectionManager()
{
  delete_connections_();
  clear_prototypes_();

  for (std::vector<ConnectorModel*>::iterator i = pristine_prototypes_.begin(); i != pristine_prototypes_.end(); ++i)
    if (*i != 0)
        delete *i;
}

void ConnectionManager::init(Dictionary* synapsedict)
{
  synapsedict_ = synapsedict;
  init_();
}

void ConnectionManager::init_()
{
  synapsedict_->clear();

  // (re-)append all synapse prototypes
  for (std::vector<ConnectorModel*>::iterator i = pristine_prototypes_.begin(); i != pristine_prototypes_.end(); ++i )
    if (*i != 0)
    {
      std::string name = (*i)->get_name();
      prototypes_.push_back((*i)->clone(name));
      synapsedict_->insert(name, prototypes_.size() - 1);
    }

  std::vector< google::sparsetable< std::vector< syn_id_connector > > > tmp(
  net_.get_num_threads(), google::sparsetable< std::vector< syn_id_connector > >());

  connections_.swap(tmp);
}

void ConnectionManager::delete_connections_()
{
  for (tVVVConnector::iterator it = connections_.begin(); it != connections_.end(); ++it)
    for (tVVConnector::nonempty_iterator iit = it->nonempty_begin(); iit != it->nonempty_end(); ++iit)
      for ( tVConnector::iterator iiit = iit->begin(); iiit != iit->end(); ++iiit)
	delete (*iiit).connector;
}

void ConnectionManager::clear_prototypes_()
{
  for (std::vector<ConnectorModel*>::iterator pt = prototypes_.begin(); pt != prototypes_.end(); ++pt)
    if (*pt != 0)
      delete *pt;
  prototypes_.clear();
}
  
void ConnectionManager::reset()
{
  delete_connections_();
  clear_prototypes_();
  init_();
}

bool ConnectionManager::synapse_prototype_in_use(index syn_id)
{
  if (syn_id < prototypes_.size() && prototypes_[syn_id] != 0)
    return prototypes_[syn_id]->get_num_connectors() > 0;
  else
    throw UnknownSynapseType(syn_id);

  return false;
}

index ConnectionManager::register_synapse_prototype(ConnectorModel * cf)
{
  std::string name = cf->get_name();
  
  if ( synapsedict_->known(name) )
  {
    delete cf;
    throw NamingConflict("A synapse type called '" + name + "' already exists.\n"
                         "Please choose a different name!");
  }
  

  pristine_prototypes_.push_back(cf);
  prototypes_.push_back(cf->clone(name));
  const index id = prototypes_.size() - 1;

  synapsedict_->insert(name, id);

  return id;
}

void ConnectionManager::unregister_synapse_prototype(index syn_id)
{
  const std::string name = get_synapse_prototype(syn_id).get_name();

  if (synapse_prototype_in_use(syn_id))
    throw ModelInUse(name); 
  
  synapsedict_->erase(name);

  // unregister the synapse prototype from the pristine_prototypes_ list
  delete pristine_prototypes_[syn_id];
  pristine_prototypes_[syn_id] = 0;

  // unregister the synapse prototype from the prototypes_ list
  delete prototypes_[syn_id];
  prototypes_[syn_id] = 0;
}

void ConnectionManager::try_unregister_synapse_prototype(index syn_id)
{
  const std::string name = get_synapse_prototype(syn_id).get_name();

  if ( synapse_prototype_in_use(syn_id) )
    throw ModelInUse(name); 
}

void ConnectionManager::calibrate(const TimeConverter & tc)
{
  for (std::vector<ConnectorModel*>::iterator pt = prototypes_.begin(); pt != prototypes_.end(); ++pt)
    if (*pt != 0)
      (*pt)->calibrate(tc);
}

const Time ConnectionManager::get_min_delay() const
{
  Time min_delay = Time::pos_inf();

  std::vector<ConnectorModel*>::const_iterator it;
  for (it = prototypes_.begin(); it != prototypes_.end(); ++it)
    if (*it != 0 && (*it)->get_num_connections() > 0)
      min_delay = std::min( min_delay, (*it)->get_min_delay() );

  if (min_delay == Time::pos_inf())
    min_delay = Time::get_resolution();

  return min_delay;
} 

const Time ConnectionManager::get_max_delay() const
{
  Time max_delay = Time::get_resolution();

  std::vector<ConnectorModel*>::const_iterator it;
  for (it = prototypes_.begin(); it != prototypes_.end(); ++it)
    if (*it != 0 && (*it)->get_num_connections() > 0)
      max_delay = std::max( max_delay, (*it)->get_max_delay() );

  return max_delay;
}

index ConnectionManager::validate_connector(thread tid, index gid, index syn_id)
{
  assert_valid_syn_id(syn_id);

  if (connections_[tid].size() < net_.size())
    connections_[tid].resize(net_.size());

  int syn_vec_index = get_syn_vec_index(tid, gid, syn_id);
  if ( syn_vec_index == -1 )
  {
    struct syn_id_connector sc = {syn_id, prototypes_[syn_id]->get_connector()};

    connections_[tid].mutating_get(gid).push_back(sc);
    syn_vec_index = connections_[tid].get(gid).size() - 1;
  }
  return static_cast<index>(syn_vec_index);
}

index ConnectionManager::copy_synapse_prototype(index old_id, std::string new_name)
{
  // we can assert here, as nestmodule checks this for us
  assert (! synapsedict_->known(new_name));

  ConnectorModel* new_prototype = get_synapse_prototype(old_id).clone(new_name);
  prototypes_.push_back(new_prototype);
  int new_id = prototypes_.size() - 1;
  synapsedict_->insert(new_name, new_id);
  return new_id;
}

void ConnectionManager::get_status(DictionaryDatum& d) const
{
  def<long>(d, "num_connections", get_num_connections());
}

void ConnectionManager::set_prototype_status(index syn_id, const DictionaryDatum& d)
{
  assert_valid_syn_id(syn_id);
  prototypes_[syn_id]->set_status(d);
}

DictionaryDatum ConnectionManager::get_prototype_status(index syn_id) const
{
  assert_valid_syn_id(syn_id);

  DictionaryDatum dict(new Dictionary);
  prototypes_[syn_id]->get_status(dict);
  return dict;
}

DictionaryDatum ConnectionManager::get_synapse_status(index gid, index syn_id, port p, thread tid)
{
  assert_valid_syn_id(syn_id);

  DictionaryDatum dict(new Dictionary);
  connections_[tid].get(gid)[syn_id].connector->get_synapse_status(dict, p);
  (*dict)[names::source] = gid;
  (*dict)[names::synapse_type] = LiteralDatum(get_synapse_prototype(syn_id).get_name());

  return dict;
}

void ConnectionManager::set_synapse_status(index gid, index syn_id, port p, thread tid, const DictionaryDatum& dict)
{
  assert_valid_syn_id(syn_id);
  connections_[tid].get(gid)[syn_id].connector->set_synapse_status(dict, p);
}


DictionaryDatum ConnectionManager::get_connector_status(const Node& node, index syn_id)
{
  assert_valid_syn_id(syn_id);

  DictionaryDatum dict(new Dictionary);
  index gid = node.get_gid();
  for (thread tid = 0; tid < net_.get_num_threads(); tid++)
  {
    index syn_vec_index = validate_connector(tid, gid, syn_id);
    connections_[tid].get(gid)[syn_vec_index].connector->get_status(dict);
  }
  return dict;
}

void ConnectionManager::set_connector_status(Node& node, index syn_id, thread tid, const DictionaryDatum& dict)
{
  assert_valid_syn_id(syn_id);

  index gid = node.get_gid();
  index syn_vec_index = validate_connector(tid, gid, syn_id);
  connections_[tid].get(gid)[syn_vec_index].connector->set_status(dict);
}

ArrayDatum ConnectionManager::find_connections(DictionaryDatum params)
{
  ArrayDatum connectome;
  
  long source;
  bool have_source = updateValue<long>(params, names::source, source);
  if (have_source)
    net_.get_node(source); // This throws if the node does not exist
  else
    throw UndefinedName(names::source.toString());
  
  long target;
  bool have_target = updateValue<long>(params, names::target, target);
  if (have_target)
    net_.get_node(target); // This throws if the node does not exist

  size_t syn_id = 0;
  Name synmodel_name;
  bool have_synmodel = updateValue<std::string>(params, names::synapse_type, synmodel_name);

  if (have_synmodel)
  {
    const Token synmodel = synapsedict_->lookup(synmodel_name);
    if (!synmodel.empty())
      syn_id = static_cast<long>(synmodel);
    else
      throw UnknownModelName(synmodel_name.toString());
  }

  for (thread t = 0; t < net_.get_num_threads(); ++t)
  {
    if (have_synmodel)
    {
      int syn_vec_index = get_syn_vec_index(t, source, syn_id);
      if (source < connections_[t].size() && syn_vec_index != -1)
        find_connections(connectome, t, source, syn_vec_index, params);
    }
    else
    {
      if (static_cast<size_t>(source) < connections_[t].size())
        for (syn_id = 0; syn_id < prototypes_.size(); ++syn_id)
        {
          int syn_vec_index = get_syn_vec_index(t, source, syn_id);
          if (syn_vec_index != -1)
            find_connections(connectome, t, source, syn_vec_index, params);
        }
    }
  }
  
  return connectome;
}

void ConnectionManager::find_connections(ArrayDatum& connectome, thread t, index source, index syn_id, DictionaryDatum params)
{
  std::vector<long>* p = connections_[t].get(source)[syn_id].connector->find_connections(params);
  for (size_t i = 0; i < p->size(); ++i)
    connectome.push_back(ConnectionDatum(ConnectionID(source, t, syn_id, (*p)[i])));
  delete p;
}

void ConnectionManager::connect(Node& s, Node& r, index s_gid, thread tid, index syn)
{
  index syn_vec_index = validate_connector(tid, s_gid, syn);
  connections_[tid].get(s_gid)[syn_vec_index].connector->register_connection(s, r);
}

void ConnectionManager::connect(Node& s, Node& r, index s_gid, thread tid, double_t w, double_t d, index syn)
{
  index syn_vec_index = validate_connector(tid, s_gid, syn);
  connections_[tid].get(s_gid)[syn_vec_index].connector->register_connection(s, r, w, d);
}

void ConnectionManager::connect(Node& s, Node& r, index s_gid, thread tid, DictionaryDatum& p, index syn)
{
  index syn_vec_index = validate_connector(tid, s_gid, syn);
  connections_[tid].get(s_gid)[syn_vec_index].connector->register_connection(s, r, p);
}

void ConnectionManager::send(thread t, index sgid, Event& e)
{
  if (sgid < connections_[t].size())
    for (size_t i = 0; i < connections_[t].get(sgid).size(); ++i)
      connections_[t].get(sgid)[i].connector->send(e);
}

size_t ConnectionManager::get_num_connections() const
{
  size_t num_connections = 0;
  std::vector<ConnectorModel*>::const_iterator iter;
  for (iter = prototypes_.begin(); iter != prototypes_.end(); ++iter)
    num_connections += (*iter)->get_num_connections();

  return num_connections;
} 

} // namespace
