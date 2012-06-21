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
#include "binomial_randomdev.h"

namespace nest
{
  template<int D>
  void ConnectionCreator::connect(Layer<D>& source, Layer<D>& target)
  {
    switch (type_) {
    case Target_driven:

      target_driven_connect_(source, target);
      break;

    case Convergent:

      convergent_connect_(source, target);
      break;

    case Divergent:

      divergent_connect_(source, target);
      break;

    case Source_driven:

      // Reverse the mask and parameters and use target driven connect,
      // which is more efficient.
      if (mask_.valid()) {
        const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask_);
        mask_ = lockPTR<AbstractMask>(new ConverseMask<D>(mask_ref));
      }
      for(ParameterMap::iterator iter=parameters_.begin(); iter != parameters_.end(); ++iter) {
        parameters_[iter->first] = lockPTR<Parameter>(new ConverseParameter(*iter->second));
      }

      type_ = Target_driven;

      target_driven_connect_(source, target);
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
  void ConnectionCreator::target_driven_connect_(Layer<D>& source, Layer<D>& target)
  {
    // Target driven connect
    // For each local target node:
    //  1. Apply Mask to source layer
    //  2. For each source node: Compute probability, draw random number, make
    //     connection conditionally

    DictionaryDatum d = new Dictionary();

    std::vector<Node*>::const_iterator target_begin;
    std::vector<Node*>::const_iterator target_end;
    if (target_filter_.select_depth()) {
      target_begin = target.local_begin(target_filter_.depth);
      target_end = target.local_end(target_filter_.depth);
    } else {
      target_begin = target.local_begin();
      target_end = target.local_end();
    }

    if (mask_.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask_);
      Ntree<D,index> *ntree = source.get_global_positions_ntree(source_filter_);

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        if (kernel_.valid()) {

          for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target_pos); iter!=ntree->masked_end(); ++iter) {

            if (rng->drand() < kernel_->value(iter->first - target_pos, rng)) {
              get_parameters_(iter->first - target_pos, rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }

          }

        } else {

          // no kernel

          for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target_pos); iter!=ntree->masked_end(); ++iter) {
            get_parameters_(iter->first - target_pos, rng, d);
            net_.connect(iter->second,target_id,d,synapse_model_);
          }

        }

      }

    } else {
      // no mask

      std::vector<std::pair<Position<D>,index> >* positions = source.get_global_positions_vector(source_filter_);
      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        if (kernel_.valid()) {

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {
            if (rng->drand() < kernel_->value(iter->first - target_pos, rng)) {
              get_parameters_(iter->first - target_pos, rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }
          }

        } else {

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {
            get_parameters_(iter->first - target_pos, rng, d);
            net_.connect(iter->second,target_id,d,synapse_model_);
          }

        }
      }
    }

  }

  template<int D>
  void ConnectionCreator::convergent_connect_(Layer<D>& source, Layer<D>& target)
  {
    // Convergent connections (fixed fan in)
    //
    // For each local target node:
    // 1. Apply Mask to source layer
    // 2. Compute connection probability for each source position
    // 3. Draw source nodes and make connections

    DictionaryDatum d = new Dictionary();

    std::vector<Node*>::const_iterator target_begin;
    std::vector<Node*>::const_iterator target_end;
    if (target_filter_.select_depth()) {
      target_begin = target.local_begin(target_filter_.depth);
      target_end = target.local_end(target_filter_.depth);
    } else {
      target_begin = target.local_begin();
      target_end = target.local_end();
    }

    if (mask_.valid()) {

      const Mask<D>& mask_ref = dynamic_cast<const Mask<D>&>(*mask_);
      Ntree<D,index> *ntree = source.get_global_positions_ntree(source_filter_);

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        std::vector<index> sources;
        std::vector<Position<D> > positions;

        if (kernel_.valid()) {

          std::vector<double_t> probabilities;

          for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target.get_position((*tgt_it)->get_subnet_index()));iter!=ntree->masked_end();++iter) {
            positions.push_back(iter->first);
            sources.push_back(iter->second);
            probabilities.push_back(kernel_->value(iter->first - target_pos, rng));
          }

          if (sources.size()==0) {

            std::string msg = String::compose("Global target ID %1: No sources found inside mask.", target_id);
            net_.message(SLIInterpreter::M_WARNING, "ConnectLayers", msg.c_str());

          } else {

            Vose lottery(probabilities);

            for(int i=0;i<number_of_connections_;++i) {
              index random_id = lottery.get_random_id(rng);
              index source_id = sources[random_id];
              get_parameters_(positions[random_id] - target_pos, rng, d);
              net_.connect(source_id, target_id, d, synapse_model_);
            }

          }

        } else {

          // no kernel

          for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask_ref,target.get_position((*tgt_it)->get_subnet_index()));iter!=ntree->masked_end();++iter) {
            positions.push_back(iter->first);
            sources.push_back(iter->second);
          }

          for(int i=0;i<number_of_connections_;++i) {
            index random_id = rng->ulrand(sources.size());
            index source_id = sources[random_id];
            get_parameters_(positions[random_id] - target_pos, rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
          }

        }

      }

    } else {
      // no mask

      std::vector<std::pair<Position<D>,index> >* positions = source.get_global_positions_vector(source_filter_);

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        if (kernel_.valid()) {

          std::vector<double_t> probabilities;

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {
            probabilities.push_back(kernel_->value(iter->first - target_pos, rng));
          }

          Vose lottery(probabilities);

          for(int i=0;i<number_of_connections_;++i) {
            index random_id = lottery.get_random_id(rng);
            Position<D> source_pos = (*positions)[random_id].first;
            index source_id = (*positions)[random_id].second;
            get_parameters_(source_pos - target_pos, rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
          }

        } else {

          // no kernel

          for(int i=0;i<number_of_connections_;++i) {
            index random_id = rng->ulrand(positions->size());
            Position<D> source_pos = (*positions)[random_id].first;
            index source_id = (*positions)[random_id].second;
            get_parameters_(source_pos - target_pos, rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
          }

        }

      }
    }
  }


  template<int D>
  void ConnectionCreator::divergent_connect_(Layer<D>& source, Layer<D>& target)
  {
    // Divergent connections (fixed fan out)
    //
    // For each (global) source:
    // 1. Apply mask to local targets
    // 2. If using kernel: Compute connection probability for each local target,
    //    sum and communicate
    // 3. Draw number of connections to make using global rng
    // 4. Draw from local targets and make connections

    std::vector<Node*>::const_iterator target_begin;
    std::vector<Node*>::const_iterator target_end;
    if (target_filter_.select_depth()) {
      target_begin = target.local_begin(target_filter_.depth);
      target_end = target.local_end(target_filter_.depth);
    } else {
      target_begin = target.local_begin();
      target_end = target.local_end();
    }

    std::vector<std::pair<Position<D>,index> >* sources = source.get_global_positions_vector(source_filter_);
    DictionaryDatum d = new Dictionary();

    for (typename std::vector<std::pair<Position<D>,index> >::iterator src_it = sources->begin(); src_it != sources->end(); ++src_it) {

      Position<D> source_pos = src_it->first;
      index source_id = src_it->second;
      std::vector<index> targets;
      std::vector<Position<D> > displacements;
      std::vector<double_t> probabilities;

      // Find potential targets and probabilities

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        Position<D> target_displ = target.compute_displacement(source_pos, (*tgt_it)->get_subnet_index());
          
        if (mask_.valid() && !mask_->inside(target_displ))
          continue;

        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());

        targets.push_back((*tgt_it)->get_gid());
        displacements.push_back(target_displ);

        if (kernel_.valid())
          probabilities.push_back(kernel_->value(target_displ, rng));
        else
          probabilities.push_back(1.0);
      }

      // Find local and global "probability"
      double_t local_probability = std::accumulate(probabilities.begin(),probabilities.end(),0.0);
      std::vector<double_t> global_probabilities;
      Communicator::communicate(local_probability,global_probabilities);
      double_t total_probability = std::accumulate(global_probabilities.begin(),global_probabilities.end(),0.0);

      // Draw how many connections to make on each process
      std::vector<long_t> num_connections(global_probabilities.size());
      long_t total_connections = number_of_connections_;
      for(index i=0;i<num_connections.size()-1;++i) {
        librandom::BinomialRandomDev brng(net_.get_grng(), global_probabilities[i]/total_probability, total_connections);
        num_connections[i] = brng.uldev();
        total_connections -= num_connections[i];
        if (total_connections==0) break;
      }
      num_connections[num_connections.size()-1] = total_connections;

      // Draw targets
      Vose lottery(probabilities);
      for(long_t i=0;i<num_connections[Communicator::get_rank()];++i) {
        index random_id = lottery.get_random_id(net_.get_rng(net_.get_node(targets[0])->get_thread())); // FIXME: Can not use grng here!
        Position<D> target_displ = displacements[random_id];
        index target_id = targets[random_id];
        librandom::RngPtr rng = net_.get_rng(net_.get_node(target_id)->get_thread());
        get_parameters_(target_displ, rng, d);
        net_.connect(source_id, target_id, d, synapse_model_);
      }

    }

  }

} // namespace nest

#endif
