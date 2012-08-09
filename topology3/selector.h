#ifndef SELECTOR_H
#define SELECTOR_H

/*
 *  selector.h
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

#include "nest.h"
#include "dictdatum.h"

namespace nest
{

  /**
   * Contains rules for selecting nodes from a layer when connecting.
   */
  struct Selector {
    Selector(): model(-1), depth(-1)
      {}
    Selector(const DictionaryDatum &);
    bool select_model() const
      { return model>=0; }
    bool select_depth() const
      { return depth>=0; }
    bool operator==(const Selector & other)
      { return (other.model==model) and (other.depth==depth); }
    long_t model;
    long_t depth;
  };

} // namespace nest

#endif
