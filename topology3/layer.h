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
#include "nest.h"
#include "subnet.h"
#include "position.h"
#include "dictutils.h"
#include "topology_names.h"

namespace nest
{

  /**
   * Abstract base class for Layers of unspecified dimension.
   */
  class AbstractLayer: public Subnet
  {
  public:
    /**
     * Virtual destructor
     */
    virtual ~AbstractLayer()
      {}

    /**
     * @returns position of node as std::vector
     */
    virtual std::vector<double_t> get_position_vector(const index lid) const = 0;

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
     * Factory function for layers. The supplied dictionary contains
     * parameters which specify the layer type and type-specific
     * parameters.
     * @returns pointer to new layer
     */
    static index create_layer(const DictionaryDatum&);
  };

  // It is necessary to declare the template for operator<< first in order
  // to get the friend declaration to work
  template <int D>
  class Layer;

  template <int D>
  std::ostream & operator<<(std::ostream & os, const Layer<D> & pos);

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
      {}

    /**
     * Copy constructor.
     */
    Layer(const Layer &)
      {}

    /**
     * Virtual destructor
     */
    ~Layer()
      {}

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
     * @returns a bitmask specifying which directions are periodic
     */
    int get_periodic_mask() const;

    /**
     * @returns position of node identified by Subnet local id value.
     */
    virtual const Position<D> & get_position(index lid) const = 0;

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

    /**
     * Write layer data to stream.
     * For each node in layer, write one line to stream containing:
     * GID x-position y-position [z-position]
     * @param os     output stream
     * @param layer  layer to dump
     */
    friend std::ostream & operator<< <>(std::ostream & os, const Layer & layer);

    /**
     * Layers do not allow entry to the ChangeSubnet command, nodes can not
     * be added by the user.
     * @returns false
     */
    bool allow_entry() const
      { return false; }

  protected:
    Position<D> lower_left_;  ///< lower left corner (minimum coordinates) of layer
    Position<D> extent_;      ///< size of layer
    bool periodic_[D];        ///< periodic b.c.
    int stride_;              ///< number of neurons at each position
  };

  template<int D>
  Position<D> Layer<D>::compute_displacement(const Position<D>& from_pos,
                                             const index to) const
  {
    // FIXME: periodic b.c.
    return get_position(to) - from_pos;
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
      extent_ = getValue<std::vector<double_t> >(d, names::extent);
    }
    if (d->known(names::center)) {
      lower_left_ = getValue<std::vector<double_t> >(d, names::center);
      lower_left_ -= extent_/2;
    }

    Subnet::set_status(d);
  }

  template <int D>
  void Layer<D>::get_status(DictionaryDatum &d) const
  {
    Subnet::get_status(d);

    DictionaryDatum topology_dict(new Dictionary);

    (*topology_dict)[names::extent] = std::vector<double_t>(Layer<D>::extent_);
    (*topology_dict)[names::center] = std::vector<double_t>(Layer<D>::lower_left_ + Layer<D>::extent_/2);

    (*d)[names::topology] = topology_dict;
  }

} // namespace nest

#endif
