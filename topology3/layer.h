#ifndef LAYER_H
#define LAYER_H

/*
 *  layer.h
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

#include <iostream>
#include <utility>
#include <bitset>
#include "nest.h"
#include "subnet.h"
#include "position.h"
#include "dictutils.h"
#include "topology_names.h"
#include "ntree.h"
#include "connection_creator.h"

namespace nest
{

  /**
   * Abstract base class for Layers of unspecified dimension.
   */
  class AbstractLayer: public Subnet
  {
  public:
    /**
     * Constructor.
     */
    AbstractLayer() : depth_(1)
      {}

    /**
     * Virtual destructor
     */
    virtual ~AbstractLayer();

    /**
     * Get position of node. Only possible for local nodes.
     * @param sind subnet index of node
     * @returns position of node as std::vector
     */
    virtual std::vector<double_t> get_position_vector(const index sind) const = 0;

    /**
     * Returns displacement of node from given position. When using periodic
     * boundary conditions, will return minimum displacement.
     * @param from_pos  position vector in layer
     * @param to        node in layer to which displacement is to be computed
     * @returns vector pointing from from_pos to node to's position
     */
    virtual std::vector<double_t> compute_displacement(const std::vector<double_t>& from_pos,
                                                       const index to) const = 0;

    /**
     * Returns distance to node from given position. When using periodic
     * boundary conditions, will return minimum distance.
     * @param from_pos  position vector in layer
     * @param to        node in layer to which displacement is to be computed
     * @returns length of vector pointing from from_pos to node to's position
     */
    virtual double_t compute_distance(const std::vector<double_t>& from_pos,
                                      const index to) const = 0;

    /**
     * Connect this layer to the given target layer. The actual connections
     * are made in class ConnectionCreator.
     * @param target    target layer to connect to. Must have same dimension
     *                  as this layer.
     * @param connector connection properties
     */
    virtual void connect(AbstractLayer& target, ConnectionCreator &connector) = 0;

    /**
     * Factory function for layers. The supplied dictionary contains
     * parameters which specify the layer type and type-specific
     * parameters.
     * @returns pointer to new layer
     */
    static index create_layer(const DictionaryDatum&);

    /**
     * Return an Ntree with the positions and GIDs of the nodes in this
     * layer.
     */
    virtual AbstractNtree<index> * get_global_positions_ntree() = 0;

    /**
     * Write layer data to stream.
     * For each node in layer, write one line to stream containing:
     * GID x-position y-position [z-position]
     * @param os     output stream
     */
    virtual void dump_nodes(std::ostream & os) const = 0;

    /**
     * Dumps information about all connections of the given type having their source in
     * the given layer to the given output stream. For distributed simulations
     * this function will dump the connections with local targets only.
     * @param out output stream
     * @param synapse_id type of connection
     */
    virtual void dump_connections(std::ostream & out, long synapse_id) = 0;

  protected:
    /**
     * GID for the single layer for which we cache global position information
     */
    static index cached_ntree_layer_;

    /**
     * number of neurons at each position
     */
    int depth_;

    /**
     * GID for the single layer for which we cache global position information
     */
    static index cached_vector_layer_;

    /**
     * Clear the cache for global position information
     */
    virtual void clear_ntree_cache_() const = 0;

    /**
     * Clear the cache for global position information
     */
    virtual void clear_vector_cache_() const = 0;

  };


  /**
   * Abstract base class for Layer of given dimension (D=2 or 3).
   */
  template <int D>
  class Layer: public AbstractLayer
  {
  public:
    /**
     * Creates an empty layer.
     */
    Layer()
      {
        // Default center (0,0) and extent (1,1)
        for(int i=0;i<D;++i) {
          lower_left_[i] = -0.5;
          extent_[i] = 1.0;
        }
      }

    /**
     * Copy constructor.
     */
    Layer(const Layer &l) :
      lower_left_(l.lower_left_),
      extent_(l.extent_),
      periodic_(l.periodic_)
      {}

    /**
     * Virtual destructor
     */
    ~Layer()
      {
        if (cached_ntree_layer_ == get_gid()) {
          clear_ntree_cache_();
        }

        if (cached_vector_layer_ == get_gid()) {
          clear_vector_cache_();
        }
      }

    /**
     * Change properties of the layer according to the
     * entries in the dictionary.
     * @param d Dictionary with named parameter settings.
     */
    void set_status(const DictionaryDatum&);

    /**
     * Export properties of the layer by setting
     * entries in the status dictionary.
     * @param d Dictionary.
     */
    void get_status(DictionaryDatum&) const;

    /**
     * @returns The bottom left position of the layer
     */
    const Position<D> & get_lower_left() const
      { return lower_left_; }

    /**
     * @returns extent of layer.
     */
    const Position<D> & get_extent() const
      { return extent_; }

    /**
     * @returns center of layer.
     */
    Position<D> get_center() const
      { return lower_left_ + extent_/2; }

    /**
     * @returns a bitmask specifying which directions are periodic
     */
    int get_periodic_mask() const;

    /**
     * Get position of node. Only possible for local nodes.
     * @param sind subnet index of node
     * @returns position of node identified by Subnet local index value.
     */
    virtual Position<D> get_position(index sind) const = 0;

    /**
     * @returns position of node as std::vector
     */
    std::vector<double_t> get_position_vector(const index lid) const;

    /**
     * Returns displacement of node from given position. When using periodic
     * boundary conditions, will return minimum displacement.
     * @param from_pos  position vector in layer
     * @param to        node in layer to which displacement is to be computed
     * @returns vector pointing from from_pos to node to's position
     */
    virtual Position<D> compute_displacement(const Position<D>& from_pos,
                                             const index to) const;

    std::vector<double_t> compute_displacement(const std::vector<double_t>& from_pos,
                                               const index to) const;

    /**
     * Returns distance to node from given position. When using periodic
     * boundary conditions, will return minimum distance.
     * @param from_pos  position vector in layer
     * @param to        node in layer to which displacement is to be computed
     * @returns length of vector pointing from from_pos to node to's position
     */
    virtual double_t compute_distance(const Position<D>& from_pos,
                                      const index to) const;

    double_t compute_distance(const std::vector<double_t>& from_pos,
                              const index to) const;

    Ntree<D,index> * get_global_positions_ntree();

    std::vector<std::pair<Position<D>,index> >* get_global_positions_vector();

    /**
     * Connect this layer to the given target layer. The actual connections
     * are made in class ConnectionCreator.
     * @param target    target layer to connect to. Must have same dimension
     *                  as this layer.
     * @param connector connection properties
     */
    void connect(AbstractLayer& target, ConnectionCreator &connector);

    /**
     * Write layer data to stream.
     * For each node in layer, write one line to stream containing:
     * GID x-position y-position [z-position]
     * @param os     output stream
     */
    void dump_nodes(std::ostream & os) const;

    /**
     * Dumps information about all connections of the given type having their source in
     * the given layer to the given output stream. For distributed simulations
     * this function will dump the connections with local targets only.
     * @param out output stream
     * @param synapse_id type of connection
     */
    void dump_connections(std::ostream & out, long synapse_id);

    /**
     * Layers do not allow entry to the ChangeSubnet command, nodes can not
     * be added by the user.
     * @returns false
     */
    bool allow_entry() const
      { return false; }

  protected:
    /**
     * Clear the cache for global position information
     */
    void clear_ntree_cache_() const;

    /**
     * Clear the cache for global position information
     */
    void clear_vector_cache_() const;

    /**
     * Insert global position info into ntree.
     */
    virtual void insert_global_positions_ntree_(Ntree<D,index> & tree) = 0;

    /**
     * Insert global position info into vector.
     */
    virtual void insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > &) = 0;

    Position<D> lower_left_;  ///< lower left corner (minimum coordinates) of layer
    Position<D> extent_;      ///< size of layer
    std::bitset<D> periodic_; ///< periodic b.c.

    /**
     * Global position information for a single layer
     */
    static Ntree<D,index> * cached_ntree_;
    static std::vector<std::pair<Position<D>,index> > * cached_vector_;

  };

  template<int D>
  Ntree<D,index> * Layer<D>::cached_ntree_ = 0;

  template<int D>
  std::vector<std::pair<Position<D>,index> > * Layer<D>::cached_vector_ = 0;

  template<int D>
  Position<D> Layer<D>::compute_displacement(const Position<D>& from_pos,
                                             const index to) const
  {
    Position<D> displ = get_position(to) - from_pos;
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
  std::vector<double_t> Layer<D>::compute_displacement(const std::vector<double_t>& from_pos,
                                                       const index to) const
  {
    return std::vector<double_t>(compute_displacement(Position<D>(from_pos), to));
  }

  template<int D>
  double_t Layer<D>::compute_distance(const Position<D>& from_pos,
                                      const index to) const
  {
    return compute_displacement(from_pos, to).length();
  }

  template<int D>
  double_t Layer<D>::compute_distance(const std::vector<double_t>& from_pos,
                                      const index to) const
  {
    return compute_displacement(Position<D>(from_pos), to).length();
  }

  template<int D>
  std::vector<double_t> Layer<D>::get_position_vector(const index lid) const
  {
    return std::vector<double_t>(get_position(lid));
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
    (*topology_dict)[names::extent] = std::vector<double_t>(Layer<D>::extent_);
    (*topology_dict)[names::center] = std::vector<double_t>(Layer<D>::lower_left_ + Layer<D>::extent_/2);

    (*d)[names::topology] = topology_dict;
  }

  template <int D>
  void Layer<D>::connect(AbstractLayer& target_layer, ConnectionCreator &connector)
  {
    Layer<D> &tgt = dynamic_cast<Layer<D>&>(target_layer);
    connector.connect(*this, tgt);
  }

  template <int D>
  Ntree<D,index> * Layer<D>::get_global_positions_ntree()
  {
    if (cached_ntree_layer_ == get_gid()) {
      assert(cached_ntree_);
      return cached_ntree_;
    }

    clear_ntree_cache_();

    cached_ntree_ = new Ntree<D,index>(this->lower_left_, this->extent_, this->periodic_);

    if (cached_vector_layer_ == get_gid()) {
      // Convert from vector to Ntree
    
      typename std::insert_iterator<Ntree<D,index> > to = std::inserter(*cached_ntree_, cached_ntree_->end());
  
      for(typename std::vector<std::pair<Position<D>,index> >::iterator from=cached_vector_->begin();
          from != cached_vector_->end(); ++from) {
        *to = *from;
      }

    } else {

      insert_global_positions_ntree_(*cached_ntree_);

    }

    clear_vector_cache_();

    cached_ntree_layer_ = get_gid();

    return cached_ntree_;
  }

  template <int D>
  std::vector<std::pair<Position<D>,index> >* Layer<D>::get_global_positions_vector()
  {
    if (cached_vector_layer_ == get_gid()) {
      assert(cached_vector_);
      return cached_vector_;
    }

    clear_vector_cache_();

    cached_vector_ = new std::vector<std::pair<Position<D>,index> >;

    if (cached_ntree_layer_ == get_gid()) {
      // Convert from NTree to vector

      typename std::back_insert_iterator<std::vector<std::pair<Position<D>,index> > > to = std::back_inserter(*cached_vector_);

      for(typename Ntree<D,index>::iterator from=cached_ntree_->begin();
          from != cached_ntree_->end(); ++from) {
        *to = *from;
      }

    } else {

      insert_global_positions_vector_(*cached_vector_);

    }

    clear_ntree_cache_();

    cached_vector_layer_ = get_gid();

    return cached_vector_;
  }

  template <int D>
  void Layer<D>::clear_ntree_cache_() const
  {
    if (cached_ntree_ != 0)
      delete cached_ntree_;
    cached_ntree_ = 0;
    cached_ntree_layer_ = -1;
  }

  template <int D>
  void Layer<D>::clear_vector_cache_() const
  {
    if (cached_vector_ != 0)
      delete cached_vector_;
    cached_vector_ = 0;
    cached_vector_layer_ = -1;
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
