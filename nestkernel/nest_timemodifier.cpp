/*
 *  nest_timemodifier.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "nest_timemodifier.h"
#include "time.h"


void nest::TimeModifier::set_time_representation(nest::double_t tics_per_ms, double_t ms_per_step)
{
  nest::Time::TICS_PER_MS_  = tics_per_ms;
  nest::Time::MS_PER_TIC_   = 1.0 / nest::Time::TICS_PER_MS_;
  nest::Time::set_resolution(ms_per_step); // set TICS_PER_STEP
}

void nest::TimeModifier::reset_to_defaults()
{
  // reset the TICS_PER_MS to compiled in default values
  nest::Time::TICS_PER_MS_ = nest::Time::TICS_PER_MS_DEFAULT_;
  nest::Time::MS_PER_TIC_ = 1.0 / nest::Time::TICS_PER_MS_DEFAULT_;
								      
  // reset TICS_PER_STEP_ to compiled in default values
  nest::Time::TICS_PER_STEP_ = nest::Time::TICS_PER_STEP_DEFAULT_;
}

