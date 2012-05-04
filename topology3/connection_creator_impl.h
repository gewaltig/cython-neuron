#ifndef CONNECTION_CREATOR_IMPL_H
#define CONNECTION_CREATOR_IMPL_H

/*
 *  connection_creator_impl.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include <vector>
#include "connection_creator.h"

namespace nest
{
  template<int D>
  void ConnectionCreator::connect(const Layer<D>& source, const Layer<D>& target)
  {
    switch (type) {
    case Source_driven:

      source_driven_connect_(source, target);
      break;

    case Convergent:

      convergent_connect_(source, target);
      break;

    default:
      throw BadProperty("Unknown connection type.");
    }
        
  }

  template<int D>
  void ConnectionCreator::source_driven_connect_(const Layer<D>& source, const Layer<D>& target)
  {
    // Source driven connect
    // For each local target node:
    //  1. Apply Mask to source layer
    //  2. For each source node: Compute probability, draw random number, make
    //     connection conditionally

    Ntree<D,index> *ntree = Topology3Module::get_global_positions(&source);

    if (mask.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask);

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        //librandom::RngPtr rng = Topology3Module::get_network().get_rng((*tgt_it)->get_thread());

        for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target.get_position((*tgt_it)->get_subnet_index()));iter!=ntree->masked_end();++iter) {
          Topology3Module::get_network().connect(iter->second,target_id,synapse_model);
        }
      }

    } else {
      // no mask

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();

        for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
          Topology3Module::get_network().connect(iter->second,target_id,synapse_model);
        }
      }

    }

  }

  template<int D>
  void ConnectionCreator::convergent_connect_(const Layer<D>& source, const Layer<D>& target)
  {
    // Convergent connections (fixed fan in)
    //
    // For each local target node:
    // 1. Apply Mask to source layer
    // 2. Compute connection probability for each source position
    // 3. Draw source nodes and make connections

    Ntree<D,index> *ntree = Topology3Module::get_global_positions(&source);
    if (mask.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask);

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = Topology3Module::get_network().get_rng((*tgt_it)->get_thread());

        std::vector<index> sources;
        std::vector<double_t> probabilities;

        for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target.get_position((*tgt_it)->get_subnet_index()));iter!=ntree->masked_end();++iter) {
          sources.push_back(iter->second);
          // FIXME: compute connection probability
          probabilities.push_back(1.0);
        }

        Vose lottery(probabilities);

        for(int i=0;i<number_of_connections;++i) {
          index source_id = sources[lottery.get_random_id(rng)];
          Topology3Module::get_network().connect(source_id,target_id,synapse_model);
        }

      }

    } else {
      // no mask

      std::vector<index> sources;
      for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
        sources.push_back(iter->second);
      }

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = Topology3Module::get_network().get_rng((*tgt_it)->get_thread());

        std::vector<double_t> probabilities;

        for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
          // FIXME: compute connection probability
          probabilities.push_back(1.0);
        }

        Vose lottery(probabilities);

        for(int i=0;i<number_of_connections;++i) {
          index source_id = sources[lottery.get_random_id(rng)];
          Topology3Module::get_network().connect(source_id,target_id,synapse_model);
        }

      }
    }
  }

} // namespace nest

#endif
