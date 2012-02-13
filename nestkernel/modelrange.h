/*
 *  modelrange.h
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
 * \file modelrange.h
 * for recording ranges of gids for a given model 
 * \author Abigail Morrison
 * \date February 2010
 */

#ifndef MODELRANGE_H
#define MODELRANGE_H

#include "nest.h"

namespace nest {

  class modelrange
  {
    public:
    modelrange(index model, index first_gid, index last_gid);    
    bool is_in_range(index gid) const {return ((gid >=first_gid_) && (gid <=last_gid_));}
    index get_model_id() const {return model_;}
    index get_first_gid() const {return first_gid_;}
    index get_last_gid() const {return last_gid_;}
    void extend_range(index new_last_gid);

    private:
    index model_;
    index first_gid_;
    index last_gid_;
  };
}

#endif
