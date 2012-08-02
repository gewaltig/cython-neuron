/*
 *  mask.cpp
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

#include "mask.h"

namespace nest
{
  template<>
  bool BallMask<2>::inside(const Box<2> &b) const
  {
    Position<2> p = b.lower_left;

    // Test if all corners are inside circle

    if (!inside(p)) return false;       // (0,0)
    p[0] = b.upper_right[0];
    if (!inside(p)) return false;       // (0,1)
    p[1] = b.upper_right[1];
    if (!inside(p)) return false;       // (1,1)
    p[0] = b.lower_left[0];
    if (!inside(p)) return false;       // (1,0)

    return true;
  }

  template<>
  bool BallMask<3>::inside(const Box<3> &b) const
  {
    Position<3> p = b.lower_left;

    // Test if all corners are inside sphere

    if (!inside(p)) return false;       // (0,0,0)
    p[0] = b.upper_right[0];
    if (!inside(p)) return false;       // (0,0,1)
    p[1] = b.upper_right[1];
    if (!inside(p)) return false;       // (0,1,1)
    p[0] = b.lower_left[0];
    if (!inside(p)) return false;       // (0,1,0)
    p[2] = b.upper_right[2];
    if (!inside(p)) return false;       // (1,1,0)
    p[0] = b.upper_right[0];
    if (!inside(p)) return false;       // (1,1,1)
    p[1] = b.lower_left[1];
    if (!inside(p)) return false;       // (1,0,1)
    p[0] = b.lower_left[0];
    if (!inside(p)) return false;       // (1,0,0)

    return true;
  }

} // namespace nest
