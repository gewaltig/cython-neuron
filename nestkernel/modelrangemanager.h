/*
 *  modelrangemanager.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/**
 * \file modelrangemanager.h
 * for managing consecutive ranges of gids for a given model 
 * \author Abigail Morrison
 * \date February 2010
 */

#ifndef MODELRANGEMANAGER_H
#define MODELRANGEMANAGER_H

#include <vector>
#include "nest.h"
#include "modelrange.h"

namespace nest {

  class Modelrangemanager
  {
  public:
    Modelrangemanager();
    void add_range(long_t model, index first_gid, index last_gid);
    bool is_in_range(index gid) const {return ((gid <= last_gid_) && (gid >= first_gid_));}
    long_t get_model_id(index gid);
    bool model_in_use(index i) const;
    void clear();
    void print() const;

  private:
    std::vector<modelrange> modelranges_;
    uint_t range_idx_;
    index first_gid_;
    index last_gid_;
    //just for debug purposes, can be removed later
    int range_misses_;
  };
}
#endif
