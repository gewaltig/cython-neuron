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

namespace nest
{

  // Trick to get a templated typedef
/*
  class UnknownTree;

  template<int D>
  struct Ntree {
    typedef UnknownTree type;
  };

  template<>
  struct Ntree<2> {
    typedef Quadtree type;
  };

  template<>
  struct Ntree<3> {
    typedef Octtree type;
  };
*/
  /** Layer with free positioning of neurons, positions specified by user.
   */
  template <int D>
  class FreeLayer: public Layer<D>
  {
  public:
    const Position<D> & get_position(index lid) const;
    void set_status(const DictionaryDatum&);
    void get_status(DictionaryDatum&) const;

  protected:
    void update_bbox_(); ///< update bounding box (min/max coordinates)

    /// Vector of positions. Should match node vector in Subnet.
    std::vector<Position<D> > positions_;
//    typename Ntree<D>::type tree_;       ///< quad- or octtree with positions
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
  const Position<D> & FreeLayer<D>::get_position(index lid) const
  {
    return positions_[lid];
  }

} // namespace nest

#endif
