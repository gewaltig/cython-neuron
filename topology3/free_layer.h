#ifndef FREE_LAYER_H
#define FREE_LAYER_H

/*
 *  free_layer.h
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

#include <limits>
#include <sstream>
#include "layer.h"
#include "topology_names.h"
#include "dictutils.h"
#include "ntree_impl.h"

namespace nest
{

  /**
   * Layer with free positioning of neurons, positions specified by user.
   */
  template <int D>
  class FreeLayer: public Layer<D>
  {
  public:
    Position<D> get_position(index lid) const;
    void set_status(const DictionaryDatum&);
    void get_status(DictionaryDatum&) const;

  protected:
    void update_bbox_(); ///< update bounding box (min/max coordinates)
    void insert_global_positions_ntree_(Ntree<D,index> & tree);
    void insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec);

    /// Vector of positions. Should match node vector in Subnet.
    std::vector<Position<D> > positions_;
  };

  template <int D>
  void FreeLayer<D>::update_bbox_()
  {
    // Find max and min coordinates
    Position<D> max_pos, min_pos;

    for (int i=0; i<D; ++i) {
      max_pos[i] = std::numeric_limits<double_t>::min();
      min_pos[i] = std::numeric_limits<double_t>::max();
    }

    for (typename std::vector<Position<D> >::iterator p = positions_.begin();
        p != positions_.end();
        ++p) {
      for (int i=0; i<D; ++i) {

        if ((*p)[i] < min_pos[i])
          min_pos[i] = (*p)[i];

        if ((*p)[i] > max_pos[i])
          max_pos[i] = (*p)[i];

      }
    }

    Layer<D>::lower_left_ = min_pos;
    Layer<D>::extent_ = max_pos - min_pos;
  }

  template <int D>
  void FreeLayer<D>::set_status(const DictionaryDatum &d)
  {
    // Read positions from dictionary
    if(d->known(names::positions)) {
      TokenArray pos = getValue<TokenArray>(d, names::positions);
      if(this->global_size() != pos.size()) {
        std::stringstream expected;
        std::stringstream got;
        expected << "position array with length " << this->global_size();
        got << "position array with length" << pos.size();
        throw TypeMismatch(expected.str(), got.str());
      }

      positions_.clear();
      positions_.reserve(this->local_size());

      for(vector<Node*>::iterator i = this->local_begin(); i != this->local_end(); ++i) {
        std::vector<double_t> point =
          getValue<std::vector<double_t> >(pos[(*i)->get_lid()]);

        positions_.push_back(Position<D>(point));
      }

      update_bbox_();
      // extent and center will get overwritten by Layer::set_status
      // if given by the user
    }

    Layer<D>::set_status(d);
  }

  template <int D>
  void FreeLayer<D>::get_status(DictionaryDatum &d) const
  {
    Layer<D>::get_status(d);

    DictionaryDatum topology_dict = getValue<DictionaryDatum>((*d)[names::topology]);

    TokenArray points;
    for(typename std::vector<Position<D> >::const_iterator it = positions_.begin();
        it != positions_.end(); ++it) {
      points.push_back(it->getToken());
    }
    def2<TokenArray, ArrayDatum>(topology_dict, names::positions, points);

  }

  template <int D>
  Position<D> FreeLayer<D>::get_position(index lid) const
  {
    return positions_[lid];
  }

  template <int D,class Ins>
  static
  void communicate_positions(const std::vector<Node*>& nodes, const std::vector<Position<D> >& positions, Ins iter)
  {
    assert(nodes.size() == positions.size());
    
    std::vector<double_t> local_gid_pos((D+1)*nodes.size());

    for(index i = 0; i<nodes.size(); ++i) {
      local_gid_pos[(D+1)*i] = nodes[i]->get_gid();
      for(int j=0;j<D;++j)
        local_gid_pos[(D+1)*i+j+1] = positions[i][j];
    }

    std::vector<double_t> global_gid_pos;
    std::vector<int> displacements;
    Communicator::communicate(local_gid_pos,global_gid_pos,displacements);

    for(index i = 0; i<global_gid_pos.size(); i+=D+1) {
      *iter++ = std::pair<Position<D>,index>(&global_gid_pos[i+1], global_gid_pos[i]);
    }

  }

  template <int D>
  void FreeLayer<D>::insert_global_positions_ntree_(Ntree<D,index> & tree)
  {

    communicate_positions(this->nodes_, positions_, std::inserter(tree, tree.end()));

  }

  template <int D>
  void FreeLayer<D>::insert_global_positions_vector_(std::vector<std::pair<Position<D>,index> > & vec)
  {

    communicate_positions(this->nodes_, positions_, std::back_inserter(vec));

    // should we sort the vector here?

  }

} // namespace nest

#endif
