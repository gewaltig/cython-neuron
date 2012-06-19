/*
 *  multirange.cpp
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

#include <stdexcept>
#include "multirange.h"

nest::index nest::Multirange::operator[](index n) const
{
  for(RangeVector::const_iterator iter = ranges_.begin(); iter!= ranges_.end(); ++iter) {
    if (n <= iter->second-iter->first)
      return iter->first + n;

    n -= 1 + iter->second - iter->first;
  }
  throw std::out_of_range("Multirange::operator[]: index out of range.");
}

