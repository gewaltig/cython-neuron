
/*
 *  nest_timeconverter.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/*
 * first version
 * date: april 2008
 * author: Moritz Helias
 *
 */

#ifndef NEST_TIME_CONVERTER_H
#define NEST_TIME_CONVERTER_H

namespace nest
{

/**
 * Class to convert times from one representation to another.
 * Createing an object of TimeCOnverter at a current time representation
 * saves the current values of TICS_PER_MS and TICS_PER_STEP.
 * After having changed the time representation,
 * the members from_old_steps and from_old_tics can be used
 * to convert steps or tics given with respect to the old representation
 * in the new representation.
 */
class TimeConverter 
{

 private:
  double_t OLD_TICS_PER_MS;
  double_t OLD_TICS_PER_STEP;

 public:

  /**
   * Constructor saves current TICS_PER_MS and TICS_PER_STEP in
   * members OLD_TICS_PER_MS, OLD_TICS_PER_STEP.
   */
  TimeConverter();


  /**
   * Converts a given number of steps with respect to old representation
   * into a time object in current representation.
   */
   Time from_old_steps(long_t s_old) const;

  /**
   * Converts a given number of tics with respect to old representation
   * into a time object in current representation.
   */
   Time from_old_tics(tic_t t_old) const;
};

} // of namespace nest

#endif
