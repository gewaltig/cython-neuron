/*
 *  modelrange.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2007 by
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
 * \file modelrange.cpp
 * to record ranges of gids of the same model in compact form
 * \Abigail Morrison
 * \date february 2010
 */

#include "modelrange.h"

  // member functions of modelrange

nest::modelrange::modelrange(long_t model, index first_gid, index last_gid) :
  model_(model), first_gid_(first_gid), last_gid_(last_gid)
  { } 

void nest::modelrange::extend_range(index new_last_gid)
{
  last_gid_ = new_last_gid;
}
