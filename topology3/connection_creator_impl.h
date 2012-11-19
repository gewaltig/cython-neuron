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

      source_driven_connect_(source, target);
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

      MaskedLayer<D> masked_layer(source,source_filter_,*mask_,true,allow_oversized_);

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        if (kernel_.valid()) {

          for(typename Ntree<D,index>::masked_iterator iter=masked_layer.begin(target_pos); iter!=masked_layer.end(); ++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            if (rng->drand() < kernel_->value(source.compute_displacement(target_pos,iter->first), rng)) {
              get_parameters_(source.compute_displacement(target_pos,iter->first), rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }

          }

        } else {

          // no kernel

          for(typename Ntree<D,index>::masked_iterator iter=masked_layer.begin(target_pos); iter!=masked_layer.end(); ++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            get_parameters_(source.compute_displacement(target_pos,iter->first), rng, d);
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

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            if (rng->drand() < kernel_->value(source.compute_displacement(target_pos,iter->first), rng)) {
              get_parameters_(source.compute_displacement(target_pos,iter->first), rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }
          }

        } else {

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            get_parameters_(source.compute_displacement(target_pos,iter->first), rng, d);
            net_.connect(iter->second,target_id,d,synapse_model_);
          }

        }
      }
    }

  }

  template<int D>
  void ConnectionCreator::source_driven_connect_(Layer<D>& source, Layer<D>& target)
  {
    // Source driven connect is actually implemented as target driven,
    // but with displacements computed in the target layer. The Mask has been
    // reversed so that it can be applied to the source instead of the target.
    // For each local target node:
    //  1. Apply (Converse)Mask to source layer
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

      MaskedLayer<D> masked_layer(source,source_filter_,*mask_,true,allow_oversized_,target);

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        if (kernel_.valid()) {

          for(typename Ntree<D,index>::masked_iterator iter=masked_layer.begin(target_pos); iter!=masked_layer.end(); ++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            if (rng->drand() < kernel_->value(target.compute_displacement(iter->first, target_pos), rng)) {
              get_parameters_(target.compute_displacement(iter->first, target_pos), rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }

          }

        } else {

          // no kernel

          for(typename Ntree<D,index>::masked_iterator iter=masked_layer.begin(target_pos); iter!=masked_layer.end(); ++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            get_parameters_(target.compute_displacement(iter->first,target_pos), rng, d);
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

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            if (rng->drand() < kernel_->value(target.compute_displacement(iter->first,target_pos), rng)) {
              get_parameters_(target.compute_displacement(iter->first,target_pos), rng, d);
              net_.connect(iter->second,target_id,d,synapse_model_);
            }
          }

        } else {

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {

            if ((not allow_autapses_) and (iter->second == target_id))
              continue;

            get_parameters_(target.compute_displacement(iter->first,target_pos), rng, d);
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

      for (std::vector<Node*>::const_iterator tgt_it = target_begin;tgt_it != target_end;++tgt_it) {

        if (target_filter_.select_model() && ((*tgt_it)->get_model_id() != target_filter_.model))
          continue;

        index target_id = (*tgt_it)->get_gid();
        librandom::RngPtr rng = net_.get_rng((*tgt_it)->get_thread());
        Position<D> target_pos = target.get_position((*tgt_it)->get_subnet_index());

        std::vector<std::pair<Position<D>,index> > positions =
            source.get_global_positions_vector(source_filter_, mask_ref,
                                               target.get_position((*tgt_it)->get_subnet_index()),
                                               allow_oversized_);

        if (kernel_.valid()) {

          std::vector<double_t> probabilities;

          
          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions.begin();iter!=positions.end();++iter) {

              probabilities.push_back(kernel_->value(source.compute_displacement(target_pos,iter->first), rng));

          }

          if ((positions.size()==0) or
              ((not allow_autapses_) and (positions.size()==1) and (positions[0].second==target_id)) or
              ((not allow_multapses_) and (positions.size()<number_of_connections_)) ) {
            std::string msg = String::compose("Global target ID %1: Not enough sources found inside mask", target_id);
            throw KernelException(msg.c_str());
          }

          Vose lottery(probabilities);

          std::vector<bool> is_selected(positions.size());

          for(int i=0;i<number_of_connections_;++i) {
            index random_id = lottery.get_random_id(rng);
            if ((not allow_multapses_) and (is_selected[random_id])) {
              --i;
              continue;
            }

            index source_id = positions[random_id].second;
            if ((not allow_autapses_) and (source_id == target_id)) {
              --i;
              continue;
            }

            get_parameters_(source.compute_displacement(target_pos,positions[random_id].first), rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
            is_selected[random_id] = true;
          }

        } else {

          // no kernel

          if ((positions.size()==0) or
              ((not allow_autapses_) and (positions.size()==1) and (positions[0].second==target_id)) or
              ((not allow_multapses_) and (positions.size()<number_of_connections_)) ) {
            std::string msg = String::compose("Global target ID %1: Not enough sources found inside mask", target_id);
            throw KernelException(msg.c_str());
          }

          std::vector<bool> is_selected(positions.size());
          for(int i=0;i<number_of_connections_;++i) {
            index random_id = rng->ulrand(positions.size());
            if ((not allow_multapses_) and (is_selected[random_id])) {
              --i;
              continue;
            }
            index source_id = positions[random_id].second;
            get_parameters_(source.compute_displacement(target_pos,positions[random_id].first), rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
            is_selected[random_id] = true;
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

        if ( (positions->size()==0) or
             ((not allow_autapses_) and (positions->size()==1) and ((*positions)[0].second==target_id)) or
             ((not allow_multapses_) and (positions->size()<number_of_connections_)) ) {
          std::string msg = String::compose("Global target ID %1: Not enough sources found", target_id);
          throw KernelException(msg.c_str());
        }

        if (kernel_.valid()) {

          std::vector<double_t> probabilities;

          for(typename std::vector<std::pair<Position<D>,index> >::iterator iter=positions->begin();iter!=positions->end();++iter) {
            probabilities.push_back(kernel_->value(source.compute_displacement(target_pos,iter->first), rng));
          }

          Vose lottery(probabilities);

          std::vector<bool> is_selected(positions->size());
          for(int i=0;i<number_of_connections_;++i) {
            index random_id = lottery.get_random_id(rng);
            if ((not allow_multapses_) and (is_selected[random_id])) {
              --i;
              continue;
            }

            index source_id = (*positions)[random_id].second;
            if ((not allow_autapses_) and (source_id == target_id)) {
              --i;
              continue;
            }

            Position<D> source_pos = (*positions)[random_id].first;
            get_parameters_(source.compute_displacement(target_pos,source_pos), rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
            is_selected[random_id] = true;
          }

        } else {

          // no kernel

          std::vector<bool> is_selected(positions->size());
          for(int i=0;i<number_of_connections_;++i) {
            index random_id = rng->ulrand(positions->size());
            if ((not allow_multapses_) and (is_selected[random_id])) {
              --i;
              continue;
            }

            index source_id = (*positions)[random_id].second;
            if ((not allow_autapses_) and (source_id == target_id)) {
              --i;
              continue;
            }

            Position<D> source_pos = (*positions)[random_id].first;
            get_parameters_(source.compute_displacement(target_pos,source_pos), rng, d);
            net_.connect(source_id, target_id, d, synapse_model_);
            is_selected[random_id] = true;
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

    MaskedLayer<D> masked_target(target,target_filter_,*mask_,false,allow_oversized_);

    std::vector<std::pair<Position<D>,index> >* sources = source.get_global_positions_vector(source_filter_);
    DictionaryDatum d = new Dictionary();

    for (typename std::vector<std::pair<Position<D>,index> >::iterator src_it = sources->begin(); src_it != sources->end(); ++src_it) {

      Position<D> source_pos = src_it->first;
      index source_id = src_it->second;
      std::vector<std::vector<index> > targets(net_.get_num_threads());
      std::vector<std::vector<Position<D> > > displacements(net_.get_num_threads());
      std::vector<std::vector<double_t> > probabilities(net_.get_num_threads());

      // Find potential targets and probabilities

      for(typename Ntree<D,index>::masked_iterator tgt_it=masked_target.begin(source_pos); tgt_it!=masked_target.end(); ++tgt_it) {

        if ((not allow_autapses_) and (source_id == tgt_it->second))
          continue;

        Position<D> target_displ = target.compute_displacement(source_pos, tgt_it->first);
        index target_thread = net_.get_node(tgt_it->second)->get_thread();
        librandom::RngPtr rng = net_.get_rng(target_thread);

        targets[target_thread].push_back(tgt_it->second);
        displacements[target_thread].push_back(target_displ);

        if (kernel_.valid())
          probabilities[target_thread].push_back(kernel_->value(target_displ, rng));
        else
          probabilities[target_thread].push_back(1.0);
      }

      // Find local and global "probability"
      std::vector<double_t> local_probability(net_.get_num_threads());
      for(int i=0;i<net_.get_num_threads();++i)
        local_probability[i] = std::accumulate(probabilities[i].begin(),probabilities[i].end(),0.0);
      std::vector<int> disp(Communicator::get_num_processes());
      for(std::vector<int>::iterator disp_it = disp.begin()+1;disp_it != disp.end();++disp_it)
        *disp_it = *(disp_it-1) + net_.get_num_threads();
      std::vector<double_t> global_probabilities;
      Communicator::communicate(local_probability,global_probabilities,disp);
      double_t total_probability = std::accumulate(global_probabilities.begin(),global_probabilities.end(),0.0);

      // Draw how many connections to make on each virtual process
      std::vector<long_t> num_connections(global_probabilities.size());
      long_t total_connections = number_of_connections_;
      for(index vp=0;vp<num_connections.size()-1;++vp) {
        index i = (vp%Communicator::get_num_processes())*net_.get_num_threads() + vp/Communicator::get_num_processes();
        librandom::BinomialRandomDev brng(net_.get_grng(), global_probabilities[i]/total_probability, total_connections);
        num_connections[vp] = brng.uldev();
        total_connections -= num_connections[vp];
        total_probability -= global_probabilities[i];
        if (total_connections<=0) break;
      }
      num_connections[num_connections.size()-1] = total_connections;

      for(int thr=0;thr<net_.get_num_threads();++thr) {

        if (num_connections[net_.thread_to_vp(thr)]==0)
          continue;

        if ((targets[thr].size()==0) or
            ((not allow_multapses_) and (targets[thr].size()<num_connections[net_.thread_to_vp(thr)])) ) {
          std::string msg = String::compose("Global source ID %1: Not enough targets found", source_id);
          throw KernelException(msg.c_str());
        }

        // Draw targets
        Vose lottery(probabilities[thr]);
        std::vector<bool> is_selected(targets[thr].size());
        for(long_t i=0;i<num_connections[net_.thread_to_vp(thr)];++i) {
          index random_id = lottery.get_random_id(net_.get_rng(thr));
          if ((not allow_multapses_) and (is_selected[random_id])) {
            --i;
            continue;
          }
          Position<D> target_displ = displacements[thr][random_id];
          index target_id = targets[thr][random_id];
          librandom::RngPtr rng = net_.get_rng(thr);
          get_parameters_(target_displ, rng, d);
          net_.connect(source_id, target_id, d, synapse_model_);
          is_selected[random_id] = true;
        }

      }

    }

  }

} // namespace nest

#endif
