/*
 *  vose.cpp
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

#include <algorithm>
#include <cassert>
#include "vose.h"

namespace nest
{
  Vose::Vose(std::vector<double_t> dist)
  {
    assert(dist.size() > 0);

    const index n = dist.size();

    dist_.resize(n);

    // Partition distribution into small (<=1/n) and large (>1/n) probabilities
    std::vector<BiasedCoin>::iterator small = dist_.begin();
    std::vector<BiasedCoin>::iterator large = dist_.end();

    index i = 0;

    for(std::vector<double_t>::iterator it = dist.begin(); it != dist.end(); ++it)
    {
      if (*it <= 1.0/n)
        *small++ = BiasedCoin(i++,0,(*it) * n);
      else
        *--large = BiasedCoin(i++,0,(*it) * n);
    }

    // Generate aliases
    for(small = dist_.begin(); (small != large) && (large != dist_.end()); ++small) {

      small->tails = large -> heads;  // 'tails' is the alias

      // The probability to select the alias is 1.0 - small->probability,
      // so we subtract this from large->probability. The following
      // equivalent calculation is more numerically stable:
      large->probability = (large->probability + small->probability) - 1.0;

      if (large->probability <= 1.0)
        large++;

    }

    // Since floating point calculation is not perfect, there may be
    // probabilities left over, which should be very close to 1.0.
    while(small != large)
      (small++) -> probability = 1.0;

    while(large != dist_.end())
      (large++) -> probability = 1.0;

  }

  index Vose::get_random_id(librandom::RngPtr& rng) const
  {
    // Choose random number between 0 and n
    double_t r = rng->drand()*dist_.size();

    // Use integer part to select bin
    index i = static_cast<index>(r);

    // Remainder determines whether to return original value or alias
    r -= i;

    if (r < dist_[i].probability)
      return dist_[i].heads;
    else
      return dist_[i].tails;
  }

} // namespace nest
