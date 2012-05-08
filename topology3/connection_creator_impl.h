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
    switch (type_) {
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
  void ConnectionCreator::get_parameters_(const Position<D> & pos, librandom::RngPtr rng, DictionaryDatum d)
  {
    for(ParameterMap::iterator iter=parameters_.begin(); iter != parameters_.end(); ++iter) {
      def<double_t>(d, iter->first, iter->second->value(pos, rng));
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

    DictionaryDatum d = new Dictionary(); 

    Ntree<D,index> *ntree = Topology3Module::get_global_positions(&source);

    if (mask_.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask_);

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target_pos); iter!=ntree->masked_end(); ++iter) {
          get_parameters_(iter->first - target_pos, rng, d);
          net_.connect(iter->second,target_id,d,synapse_model_);
        }
      }

    } else {
      // no mask

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
          get_parameters_(iter->first - target_pos, rng, d);
          net_.connect(iter->second,target_id,d,synapse_model_);
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

    DictionaryDatum d = new Dictionary(); 

    Ntree<D,index> *ntree = Topology3Module::get_global_positions(&source);
    if (mask_.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask_);

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        std::vector<index> sources;
        std::vector<double_t> probabilities;
        std::vector<Position<D> > positions;

        for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target.get_position((*tgt_it)->get_subnet_index()));iter!=ntree->masked_end();++iter) {
          positions.push_back(iter->first);
          sources.push_back(iter->second);
          // FIXME: compute connection probability
          probabilities.push_back(1.0);
        }

        Vose lottery(probabilities);

        for(int i=0;i<number_of_connections_;++i) {
          index random_id = lottery.get_random_id(rng);
          index source_id = sources[random_id];
          get_parameters_(positions[random_id] - target_pos, rng, d);
          net_.connect(source_id, target_id, d, synapse_model_);
        }

      }

    } else {
      // no mask

      std::vector<index> sources;
      std::vector<Position<D> > positions;
      for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
        positions.push_back(iter->first);
        sources.push_back(iter->second);
      }

      for (std::vector<Node*>::const_iterator tgt_it = target.local_begin();tgt_it != target.local_end();++tgt_it) {

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        std::vector<double_t> probabilities;

        for(typename Ntree<D,index>::iterator iter=ntree->begin();iter!=ntree->end();++iter) {
          // FIXME: compute connection probability
          probabilities.push_back(1.0);
        }

        Vose lottery(probabilities);

        for(int i=0;i<number_of_connections_;++i) {
          index random_id = lottery.get_random_id(rng);
          index source_id = sources[random_id];
          get_parameters_(positions[random_id] - target_pos, rng, d);
          net_.connect(source_id, target_id, d, synapse_model_);
        }

      }
    }
  }

} // namespace nest

#endif
