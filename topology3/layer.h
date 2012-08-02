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
#include "selector.h"

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
    virtual AbstractNtree<index> * get_global_positions_ntree(Selector filter=Selector()) = 0;

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

    using Subnet::local_begin;
    using Subnet::local_end;

    /**
     * Start of local children at given depth.
     * @param depth layer depth
     * @returns iterator for local nodes pointing to first node at given depth
     */
    std::vector<Node*>::iterator local_begin(int_t depth);

    /**
     * End of local children at given depth.
     * @param depth layer depth
     * @returns iterator for local nodes pointing to the end of the given depth
     */
    std::vector<Node*>::iterator local_end(int_t depth);

    /**
     * Start of local children at given depth.
     * @param depth layer depth
     * @returns iterator for local nodes pointing to first node at given depth
     */
    std::vector<Node*>::const_iterator local_begin(int_t depth) const;

    /**
     * End of local children at given depth.
     * @param depth layer depth
     * @returns iterator for local nodes pointing to the end of the given depth
     */
    std::vector<Node*>::const_iterator local_end(int_t depth) const;

  protected:
    /**
     * GID for the single layer for which we cache global position information
     */
    static index cached_ntree_layer_;

    /**
     * number of neurons at each position
     */
    int_t depth_;

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
    Layer();

    /**
     * Copy constructor.
     */
    Layer(const Layer &l);

    /**
     * Virtual destructor
     */
    ~Layer();

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
     std::bitset<D> get_periodic_mask() const
      { return periodic_; }

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
     * Returns displacement of a position from another position. When using periodic
     * boundary conditions, will return minimum displacement.
     * @param from_pos  position vector in layer
     * @param to_pos    position to which displacement is to be computed
     * @returns vector pointing from from_pos to to_pos
     */
    virtual Position<D> compute_displacement(const Position<D>& from_pos,
                                             const Position<D>& to_pos) const;

    /**
     * Returns displacement of node from given position. When using periodic
     * boundary conditions, will return minimum displacement.
     * @param from_pos  position vector in layer
     * @param to        node in layer to which displacement is to be computed
     * @returns vector pointing from from_pos to node to's position
     */
    Position<D> compute_displacement(const Position<D>& from_pos,
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
    double_t compute_distance(const Position<D>& from_pos,
                                      const index to) const;

    double_t compute_distance(const std::vector<double_t>& from_pos,
                              const index to) const;

    /**
     * Get positions for all nodes in layer, including nodes on other MPI
     * processes. The positions will be cached so that subsequent calls for
     * the same layer are fast. One one layer is cached at the time, so the
     * user should group together all ConnectLayer calls using the same
     * pool layer.
     */
    Ntree<D,index> * get_global_positions_ntree(Selector filter=Selector());

    /**
     * Get positions globally, overriding the dimensions of the layer and
     * the periodic flags. The supplied lower left corner and extent
     * coordinates are only used for the dimensions where the supplied
     * periodic flag is set.
     */
    Ntree<D,index> * get_global_positions_ntree(Selector filter, std::bitset<D> periodic, Position<D> lower_left, Position<D> extent);

    std::vector<std::pair<Position<D>,index> >* get_global_positions_vector(Selector filter=Selector());

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

    Ntree<D,index> * do_get_global_positions_ntree_(const Selector& filter);

    /**
     * Insert global position info into ntree.
     */
    virtual void insert_global_positions_ntree_(Ntree<D,index> & tree, const Selector& filter) = 0;

    /**
     * Insert global position info into vector.
     */
    virtual void insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > &, const Selector& filter) = 0;

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
  inline
  Layer<D>::Layer()
  {
    // Default center (0,0) and extent (1,1)
    for(int i=0;i<D;++i) {
      lower_left_[i] = -0.5;
      extent_[i] = 1.0;
    }
  }

  template<int D>
  inline
  Layer<D>::Layer(const Layer &l) :
      lower_left_(l.lower_left_),
      extent_(l.extent_),
      periodic_(l.periodic_)
  {
  }

  template<int D>
  inline
  Layer<D>::~Layer()
  {
    if (cached_ntree_layer_ == get_gid()) {
      clear_ntree_cache_();
    }

    if (cached_vector_layer_ == get_gid()) {
      clear_vector_cache_();
    }
  }

  template<int D>
  inline
  Position<D> Layer<D>::compute_displacement(const Position<D>& from_pos,
                                             const index to) const
  {
    return compute_displacement(from_pos,get_position(to));
  }

  template<int D>
  inline
  std::vector<double_t> Layer<D>::compute_displacement(const std::vector<double_t>& from_pos,
                                                       const index to) const
  {
    return std::vector<double_t>(compute_displacement(Position<D>(from_pos), to));
  }

  template<int D>
  inline
  double_t Layer<D>::compute_distance(const Position<D>& from_pos,
                                      const index to) const
  {
    return compute_displacement(from_pos, to).length();
  }

  template<int D>
  inline
  double_t Layer<D>::compute_distance(const std::vector<double_t>& from_pos,
                                      const index to) const
  {
    return compute_displacement(Position<D>(from_pos), to).length();
  }

  template<int D>
  inline
  std::vector<double_t> Layer<D>::get_position_vector(const index lid) const
  {
    return std::vector<double_t>(get_position(lid));
  }

  template <int D>
  inline
  void Layer<D>::clear_ntree_cache_() const
  {
    if (cached_ntree_ != 0)
      delete cached_ntree_;
    cached_ntree_ = 0;
    cached_ntree_layer_ = -1;
  }

  template <int D>
  inline
  void Layer<D>::clear_vector_cache_() const
  {
    if (cached_vector_ != 0)
      delete cached_vector_;
    cached_vector_ = 0;
    cached_vector_layer_ = -1;
  }

} // namespace nest

#endif
