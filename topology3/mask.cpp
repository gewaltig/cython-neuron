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
#include "dictutils.h"
#include "exceptions.h"

namespace nest {

  AbstractMask * AbstractMask::create_mask(const DictionaryDatum& mask_dict)
  {
    AbstractMask *mask = 0;

    if (mask_dict->known(Name("circular"))) {

      mask = new BallMask<2>(getValue<DictionaryDatum>(mask_dict,Name("circular")));

    } else if (mask_dict->known(Name("spherical"))) {

      mask = new BallMask<3>(getValue<DictionaryDatum>(mask_dict,Name("spherical")));

    } else if (mask_dict->known(Name("rectangular"))) {

      mask = new BoxMask<2>(getValue<DictionaryDatum>(mask_dict,Name("rectangular")));

    } else if (mask_dict->known(Name("volume"))) {  // For compatibility with topo 2.0

      mask = new BoxMask<3>(getValue<DictionaryDatum>(mask_dict,Name("volume")));

    } else if (mask_dict->known(Name("box"))) {

      mask = new BoxMask<3>(getValue<DictionaryDatum>(mask_dict,Name("box")));

    } else if (mask_dict->known(Name("doughnut"))) {

      // Doughnut is implemented as the difference of two circular masks

      DictionaryDatum d = getValue<DictionaryDatum>(mask_dict, Name("doughnut"));

      Position<2> center(0,0);
      if (d->known(Name("anchor")))
        center = getValue<std::vector<double_t> >(d, Name("anchor"));

      BallMask<2> outer_circle(center,getValue<double_t>(d, "outer_radius"));
      BallMask<2> inner_circle(center,getValue<double_t>(d, "inner_radius"));

      mask = new MinusMask<2>(outer_circle, inner_circle);

    } else {

      throw BadProperty("Unknown mask type.");

    }

    assert(mask != 0);

    return mask;
  }

} // namespace nest
