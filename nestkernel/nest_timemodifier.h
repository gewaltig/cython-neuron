/*
 *  nest_timemodifier.h
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

#ifndef NEST_TIMEMODIFIER_H
#define NEST_TIMEMODIFIER_H

#include "scheduler.h"

/*
TimeModifier is an interface class which defines the scheduler's
ability to modify the representation of time. It is only safe 
to change the number of tics representing a millisecond if no
Time objects exist or it is guaranteed that all Time objects are
reinitialized before usage. Only the Scheduler can do this.
Therefore the functions 
  set_tics_per_ms
and
  set_tics_per_step_default
are only accessible by specific members of the Scheduler
 (reset, set_status)

Diesmann
*/


namespace nest 
{
 
 class TimeModifier
 {
  // allow Scheduler::set_status to change Time representation
  friend void Scheduler::set_status(DictionaryDatum const &);

  // allow Scheduler::rest to change Time representation
  friend void Scheduler::reset();

 private:

  /**
   * Set the rime represeantation (TICS_PER_MS_, MS_PER_TICS_ and TICS_PER_STEP)
   * \param tics_per_ms number of TICS per ms
   * \param ms_per_step amount of ms per simulation step
   */
  static void set_time_representation(double_t tics_per_ms, double_t ms_per_step);
  
  /**
   * Reset TICS_PER_MS_, MS_PER_TICS_ and TICS_PER_STEP_ to compiled in default values
   */
  static void reset_to_defaults();

 };
    


} // Namespace



#endif
