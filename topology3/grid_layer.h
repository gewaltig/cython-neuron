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

    Position<D> get_position(index lid) const;
    void set_status(const DictionaryDatum &d);
    void get_status(DictionaryDatum &d) const;
    void insert_global_positions_ntree_(Ntree<D,index> & tree);
    void insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec);

  protected:
    Position<D,index> dims_;   ///< number of nodes in each direction.

    template<class Ins>
    void insert_global_positions_(Ins iter);
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
  Position<D> GridLayer<D>::get_position(index sind) const
  {
    index lid = this->nodes_[sind]->get_lid();
    Position<D,int_t> gridpos;
    for(int i=D-1;i>0;--i) {
      gridpos[i] = lid % dims_[i];
      lid = lid / dims_[i];
    }
    assert(lid < dims_[0]);
    gridpos[0] = lid;
    return this->lower_left_ + this->extent_/dims_ * gridpos + this->extent_/dims_ * 0.5;
  }

  template <int D>
  template <class Ins>
  void GridLayer<D>::insert_global_positions_(Ins iter)
  {
    LocalChildList localnodes(*this);
    vector<Communicator::NodeAddressingData> globalnodes;
    nest::Communicator::communicate(localnodes,globalnodes,true);

    index i = 0;
    for(vector<Communicator::NodeAddressingData>::iterator n = globalnodes.begin(); n != globalnodes.end(); ++n) {
      *iter++ = std::pair<Position<D>,index>(get_position(i), n->get_gid());
      ++i;
    }
  }

  template <int D>
  void GridLayer<D>::insert_global_positions_ntree_(Ntree<D,index> & tree)
  {
    insert_global_positions_(std::inserter(tree, tree.end()));
  }

  template <int D>
  void GridLayer<D>::insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec)
  {
    insert_global_positions_(std::back_inserter(vec));
  }

} // namespace nest

#endif
