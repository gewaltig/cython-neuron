/*
 *  modelrangemanager.cpp
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
 * \file modelrangemanager.cpp
 * to manage consecutive ranges of gids of the same model in compact form
 * \Abigail Morrison
 * \date february 2010
 */

#include <assert.h>
#include <iostream>
#include "modelrangemanager.h"

namespace nest {
  // member functions of modelrangemanager

Modelrangemanager::Modelrangemanager() :
  range_idx_(0), first_gid_(0), last_gid_(0), range_misses_(0)
  { } 

void Modelrangemanager::add_range(long_t model, index first_gid, index last_gid)
{
  if (!modelranges_.empty())
    {
      assert(first_gid == last_gid_ + 1);
      if (model == modelranges_.back().get_model_id())
	modelranges_.back().extend_range(last_gid);
      else
	modelranges_.push_back(modelrange(model,first_gid,last_gid));
    }
  else
    {
      modelranges_.push_back(modelrange(model,first_gid,last_gid));
      first_gid_ = first_gid;
    }
  last_gid_ = last_gid;
}

  long_t Modelrangemanager::get_model_id(index gid) 
{
  int left = -1;
  int right = modelranges_.size();
  assert(right >= 1);
  assert(is_in_range(gid));

  while (!modelranges_[range_idx_].is_in_range(gid))
    {
      if (gid > modelranges_[range_idx_].get_last_gid())
	{
	  left = range_idx_;
	  range_idx_ += (right - range_idx_)/2;
	}
      else
	{
	  right = range_idx_;
	  range_idx_ -= (range_idx_ - left)/2;
	}
      range_misses_++;
      assert(left+1 < right);
    }
  return modelranges_[range_idx_].get_model_id();
}

bool Modelrangemanager::model_in_use(index i) const
{
   bool found = false;

   for (std::vector<modelrange>::const_iterator it = modelranges_.begin();
       it != modelranges_.end();
       it++)
     if ( it->get_model_id() == i )
       {
	 found = true;
	 break;
       }
  return found;
}


void Modelrangemanager::clear()
{
  modelranges_.clear();
  range_idx_ = 0;
}

  //just for debug purposes, can be removed later. AM 13/2/2010
void Modelrangemanager::print() const
{
  std::cout << "Total model id search steps: " << range_misses_ << std::endl;
  for (std::vector<modelrange>::const_iterator it = modelranges_.begin();
       it != modelranges_.end();
       it++)
    std::cout << it->get_model_id() << "\t" << it->get_first_gid() << "\t" << it->get_last_gid() << "\n";
}

}
