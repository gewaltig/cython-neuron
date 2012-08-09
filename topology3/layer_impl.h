#ifndef LAYER_IMPL_H
#define LAYER_IMPL_H

/*
 *  layer_impl.h
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

#include "layer.h"

namespace nest {

  template<int D>
  Ntree<D,index> * Layer<D>::cached_ntree_ = 0;

  template<int D>
  std::vector<std::pair<Position<D>,index> > * Layer<D>::cached_vector_ = 0;

  template<int D>
  Selector Layer<D>::cached_selector_;

  template<int D>
  Position<D> Layer<D>::compute_displacement(const Position<D>& from_pos,
                                             const Position<D>& to_pos) const
  {
    Position<D> displ = to_pos - from_pos;
    for(int i=0;i<D;++i) {
      if (periodic_[i]) {
        displ[i] = -0.5*extent_[i] + std::fmod(displ[i]+0.5*extent_[i], extent_[i]);
        if (displ[i]<-0.5*extent_[i])
          displ[i] += extent_[i];
      }
    }
    return displ;
  }

  template<int D>
  void Layer<D>::set_status(const DictionaryDatum & d)
  {
    if (d->known(names::extent)) {
      Position<D> center = get_center();
      extent_ = getValue<std::vector<double_t> >(d, names::extent);
      lower_left_ = center - extent_/2;
    }
    if (d->known(names::center)) {
      lower_left_ = getValue<std::vector<double_t> >(d, names::center);
      lower_left_ -= extent_/2;
    }
    if (d->known(names::edge_wrap)) {
      if (getValue<bool>(d, names::edge_wrap)) {
        periodic_ = (1<<D) - 1;  // All dimensions periodic
      }
    }

    Subnet::set_status(d);
  }

  template <int D>
  void Layer<D>::get_status(DictionaryDatum &d) const
  {
    Subnet::get_status(d);

    DictionaryDatum topology_dict(new Dictionary);

    (*topology_dict)[names::depth] = depth_;
    (*topology_dict)[names::extent] = std::vector<double_t>(extent_);
    (*topology_dict)[names::center] = std::vector<double_t>(lower_left_ + extent_/2);

    if (periodic_.none())
      (*topology_dict)[names::edge_wrap] = BoolDatum(false);
    else if (periodic_.count()==D)
      (*topology_dict)[names::edge_wrap] = true;

    (*d)[names::topology] = topology_dict;
  }

  template <int D>
  void Layer<D>::connect(AbstractLayer& target_layer, ConnectionCreator &connector)
  {
    Layer<D> &tgt = dynamic_cast<Layer<D>&>(target_layer);
    connector.connect(*this, tgt);
  }

  template <int D>
  Ntree<D,index> * Layer<D>::get_global_positions_ntree(Selector filter)
  {
    if ((cached_ntree_layer_ == get_gid()) and (cached_selector_ == filter)) {
      assert(cached_ntree_);
      return cached_ntree_;
    }

    clear_ntree_cache_();

    cached_ntree_ = new Ntree<D,index>(this->lower_left_, this->extent_, this->periodic_);

    return do_get_global_positions_ntree_(filter);
  }

  template <int D>
  Ntree<D,index> * Layer<D>::get_global_positions_ntree(Selector filter, std::bitset<D> periodic, Position<D> lower_left, Position<D> extent)
  {
    clear_ntree_cache_();
    clear_vector_cache_();

    // Keep layer geometry for non-periodic dimensions
    for(int i=0;i<D;++i) {
      if (not periodic[i]) {
        extent[i] = extent_[i];
        lower_left[i] = lower_left_[i];
      }
    }

    cached_ntree_ = new Ntree<D,index>(this->lower_left_, extent, periodic);

    do_get_global_positions_ntree_(filter);

    cached_ntree_layer_ = -1; // Do not use cache since the periodic bits and extents were altered.

    return cached_ntree_;
  }

  template <int D>
  Ntree<D,index> * Layer<D>::do_get_global_positions_ntree_(const Selector& filter)
  {
    if ((cached_vector_layer_ == get_gid()) and (cached_selector_ == filter)) {
      // Convert from vector to Ntree
    
      typename std::insert_iterator<Ntree<D,index> > to = std::inserter(*cached_ntree_, cached_ntree_->end());
  
      for(typename std::vector<std::pair<Position<D>,index> >::iterator from=cached_vector_->begin();
          from != cached_vector_->end(); ++from) {
        *to = *from;
      }

    } else {

      insert_global_positions_ntree_(*cached_ntree_, filter);

    }

    clear_vector_cache_();

    cached_ntree_layer_ = get_gid();
    cached_selector_ = filter;

    return cached_ntree_;
  }

  template <int D>
  std::vector<std::pair<Position<D>,index> >* Layer<D>::get_global_positions_vector(Selector filter)
  {
    if ((cached_vector_layer_ == get_gid()) and (cached_selector_ == filter)) {
      assert(cached_vector_);
      return cached_vector_;
    }

    clear_vector_cache_();

    cached_vector_ = new std::vector<std::pair<Position<D>,index> >;

    if ((cached_ntree_layer_ == get_gid()) and (cached_selector_ == filter)) {
      // Convert from NTree to vector

      typename std::back_insert_iterator<std::vector<std::pair<Position<D>,index> > > to = std::back_inserter(*cached_vector_);

      for(typename Ntree<D,index>::iterator from=cached_ntree_->begin();
          from != cached_ntree_->end(); ++from) {
        *to = *from;
      }

    } else {

      insert_global_positions_vector_(*cached_vector_, filter);

    }

    clear_ntree_cache_();

    cached_vector_layer_ = get_gid();
    cached_selector_ = filter;

    return cached_vector_;
  }

  template <int D>
  std::vector<std::pair<Position<D>,index> > Layer<D>::get_global_positions_vector(Selector filter, const Mask<D>& mask, const Position<D>& anchor)
  {
    Ntree<D,index> *ntree = get_global_positions_ntree(filter);
    std::vector<std::pair<Position<D>,index> > positions;

    for(typename Ntree<D,index>::masked_iterator iter=ntree->masked_begin(mask,anchor);iter!=ntree->masked_end();++iter) {
      positions.push_back(*iter);
    }

    return positions;
  }

  template <int D>
  void Layer<D>::dump_nodes(std::ostream & out) const
  {
    for(index i=0;i<nodes_.size();++i) {
      const index gid = nodes_[i]->get_gid();
      out << gid << ' ';
      get_position(i).print(out);
      out << std::endl;
    }
  }

  template <int D>
  void Layer<D>::dump_connections(std::ostream & out, long synapse_id)
  {
    std::vector<std::pair<Position<D>,index> >* src_vec = get_global_positions_vector();

    for(typename std::vector<std::pair<Position<D>,index> >::iterator src_iter=src_vec->begin();
        src_iter != src_vec->end(); ++src_iter) {

      const index source_gid = src_iter->second;
      const Position<D> source_pos = src_iter->first;

      DictionaryDatum dict = net_->get_connector_status(source_gid, synapse_id);

      TokenArray targets = getValue<TokenArray>(dict, names::targets);
      TokenArray weights = getValue<TokenArray>(dict, names::weights);
      TokenArray delays  = getValue<TokenArray>(dict, names::delays);

      assert(targets.size() == weights.size());
      assert(targets.size() == delays.size());

      // Print information about all local connections for current source
      for ( size_t i = 0; i < targets.size(); ++i ) {
        Node const * const target = net_->get_node(targets[i]);
        assert(target);

        // Print source, target, weight, delay, rports
        out << source_gid << ' ' << targets[i] << ' '
            << weights[i] << ' ' << delays[i];

        Layer<D>* tgt_layer = dynamic_cast<Layer<D>*>(target->get_parent());
        if (tgt_layer==0) {

          // Happens if target does not belong to layer, eg spike_detector.
          // We then print NaNs for the displacement.
          for ( int n = 0 ; n < D ; ++n )
            out << " NaN";

        } else {

          out << ' ';
          tgt_layer->compute_displacement(source_pos, target->get_subnet_index()).print(out);

        }

        out << '\n';

      }
    }
  }

} // namespace nest

#endif
