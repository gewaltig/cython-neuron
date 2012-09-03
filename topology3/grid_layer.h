#ifndef GRID_LAYER_H
#define GRID_LAYER_H

/*
 *  grid_layer.h
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

namespace nest
{

  /** Layer with neurons placed in a grid
   */
  template <int D>
  class GridLayer: public Layer<D>
  {
  public:
    GridLayer():
      Layer<D>()
      {}

    GridLayer(const GridLayer &l):
      Layer<D>(l),
      dims_(l.dims_)
      {}

    /**
     * Get position of node. Only possible for local nodes.
     * @param sind subnet index of node
     * @returns position of node identified by Subnet local index value.
     */
    Position<D> get_position(index sind) const;

    /**
     * Get position of node. Also allowed for non-local nodes.
     * @param lid local index of node
     * @returns position of node identified by Subnet local index value.
     */
    Position<D> lid_to_position(index lid) const;

    index gridpos_to_lid(Position<D,int_t> pos) const;

    Position<D> gridpos_to_position(Position<D,int_t> gridpos) const;

    /**
     * Returns nodes at a given discrete layerspace position.
     * @param pos  Discrete position in layerspace.
     * @returns vector of gids at the depth column covering
     *          the input position.
     */
    std::vector<index> get_nodes(Position<D,int_t> pos);

    std::vector<std::pair<Position<D>,index> > get_global_positions_vector(Selector filter, const Mask<D>& mask, const Position<D>& anchor);

    void set_status(const DictionaryDatum &d);
    void get_status(DictionaryDatum &d) const;

  protected:
    Position<D,index> dims_;   ///< number of nodes in each direction.

    template<class Ins>
    void insert_global_positions_(Ins iter, const Selector& filter);
    void insert_global_positions_ntree_(Ntree<D,index> & tree, const Selector& filter);
    void insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec, const Selector& filter);
  };

  template <int D>
  void GridLayer<D>::set_status(const DictionaryDatum &d)
  {
    Position<D,index> new_dims = dims_;
    updateValue<long_t>(d, names::columns, new_dims[0]);
    if (D>=2) updateValue<long_t>(d, names::rows, new_dims[1]);
    if (D>=3) updateValue<long_t>(d, names::layers, new_dims[2]);

    index new_size = this->depth_;
    for(int i=0;i<D;++i) {
      new_size *= new_dims[i];
    }

    if (new_size != this->global_size()) {
      throw BadProperty("Total size of layer must be unchanged.");
    }

    this->dims_ = new_dims;

    Layer<D>::set_status(d);
  }

  template <int D>
  void GridLayer<D>::get_status(DictionaryDatum &d) const
  {
    Layer<D>::get_status(d);

    DictionaryDatum topology_dict = getValue<DictionaryDatum>((*d)[names::topology]);

    (*topology_dict)[names::columns] = dims_[0];
    if (D>=2) (*topology_dict)[names::rows] = dims_[1];
    if (D>=3) (*topology_dict)[names::layers] = dims_[2];
  }

  template <int D>
  Position<D> GridLayer<D>::lid_to_position(index lid) const
  {
    lid %= this->global_size()/this->depth_;
    Position<D,int_t> gridpos;
    for(int i=D-1;i>0;--i) {
      gridpos[i] = lid % dims_[i];
      lid = lid / dims_[i];
    }
    assert(lid < dims_[0]);
    gridpos[0] = lid;
    return gridpos_to_position(gridpos);
  }

  template <int D>
  Position<D> GridLayer<D>::gridpos_to_position(Position<D,int_t> gridpos) const
  {
    // grid layer uses "matrix convention", i.e. reversed y axis
    Position<D> ext = this->extent_;
    Position<D> upper_left = this->lower_left_;
    if (D>1) {
      upper_left[1] += ext[1];
      ext[1] = -ext[1];
    }
    return upper_left + ext/dims_ * gridpos + ext/dims_ * 0.5;
  }
    
  template <int D>
  Position<D> GridLayer<D>::get_position(index sind) const
  {
    return lid_to_position(this->nodes_[sind]->get_lid());
  }

  template <int D>
  index GridLayer<D>::gridpos_to_lid(Position<D,int_t> pos) const
  {
    index lid = 0;

    for(int i=0;i<D;++i) {
      lid *= dims_[i];
      lid += pos[i];
    }

    return lid;
  }

  template <int D>
  std::vector<index> GridLayer<D>::get_nodes(Position<D,int_t> pos)
  {
    std::vector<index> gids;
    index lid = gridpos_to_lid(pos);
    index layer_size = this->global_size()/this->depth_;

    for(int d=0;d<this->depth_;++d) {
      gids.push_back(this->gids_[lid + d*layer_size]);
    }

    return gids;
  }

  template <int D>
  template <class Ins>
  void GridLayer<D>::insert_global_positions_(Ins iter, const Selector& filter)
  {
    index i = 0;
    index lid_end = this->gids_.size();

    if (filter.select_depth()) {
      const index nodes_per_layer = this->gids_.size() / this->depth_;
      i = nodes_per_layer * filter.depth;
      lid_end = nodes_per_layer * (filter.depth+1);
      if ((i >= this->gids_.size()) or (lid_end > this->gids_.size()))
        throw BadProperty("Selected depth out of range");
    }

    Multirange::iterator gi = this->gids_.begin();
    for (index j=0;j<i;++j)  // Advance iterator to first gid at selected depth
      ++gi;

    for(; (gi != this->gids_.end()) && (i < lid_end); ++gi, ++i) {

      if (filter.select_model() && (this->net_->get_model_id_of_gid(*gi) != filter.model))
        continue;

      *iter++ = std::pair<Position<D>,index>(lid_to_position(i), *gi);
    }
  }

  template <int D>
  void GridLayer<D>::insert_global_positions_ntree_(Ntree<D,index> & tree, const Selector& filter)
  {
    insert_global_positions_(std::inserter(tree, tree.end()), filter);
  }

  template <int D>
  void GridLayer<D>::insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec, const Selector& filter)
  {
    insert_global_positions_(std::back_inserter(vec), filter);
  }

  template <int D>
  std::vector<std::pair<Position<D>,index> > GridLayer<D>::get_global_positions_vector(Selector filter, const Mask<D>& mask, const Position<D>& anchor)
  {
    Position<D,int> ll;
    Position<D,int> ur;
    Box<D> bbox=mask.get_bbox();
    bbox.lower_left += anchor;
    bbox.upper_right += anchor;
    // grid layer uses "matrix convention", i.e. reversed y axis
    Position<D> ext = this->extent_;
    Position<D> upper_left = this->lower_left_;
    if (D>1) {
      upper_left[1] += ext[1];
      ext[1] = -ext[1];
    }
    ext /= dims_;
    for(int i=0;i<D;++i) {
      ll[i] = std::min(index(std::max(ceil((bbox.lower_left[i] - upper_left[i])/ext[i] - 0.5), 0.0)), dims_[i]);
      ur[i] = std::min(index(std::max(round((bbox.upper_right[i] - upper_left[i])/ext[i]), 0.0)), dims_[i]);
    }
    if (D>1) {
      std::swap(ll[1],ur[1]);
    }

    std::vector<std::pair<Position<D>,index> > positions;

    // FIXME: selection
    index layer_size = this->global_size()/this->depth_;

    if (filter.select_depth()) {

      const index d = filter.depth;

      for(MultiIndex<D> mi = MultiIndex<D>(ll,ur); mi<ur; ++mi) {
        Position<D> pos = gridpos_to_position(mi);
        if (mask.inside(pos-anchor)) {
          index lid = gridpos_to_lid(mi);
          positions.push_back(std::pair<Position<D>,index>(pos,this->gids_[lid + d*layer_size]));
        }
      }

    } else {

      for(int d=0;d<this->depth_;++d) {
        if (filter.select_model() && (this->net_->get_model_id_of_gid(this->gids_[d*layer_size]) != filter.model))
          continue;
        
        for(MultiIndex<D> mi = MultiIndex<D>(ll,ur); mi<ur; ++mi) {
          Position<D> pos = gridpos_to_position(mi);
          if (mask.inside(pos-anchor)) {
            index lid = gridpos_to_lid(mi);
            positions.push_back(std::pair<Position<D>,index>(pos,this->gids_[lid + d*layer_size]));
          }
        }
      }
    }

    return positions;
  }

} // namespace nest

#endif
