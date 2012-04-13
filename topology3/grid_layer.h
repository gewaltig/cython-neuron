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
    const Position<D> & get_position(index lid) const;
  protected:
    Position<D,int> dims_;   ///< number of nodes in each direction.
  };

} // namespace nest

#endif
