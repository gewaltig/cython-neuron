/*
 *  event_priority.h
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

#ifndef EVENT_PRIORITY_H
#define EVENT_PRIORITY_H

#include "event.h"

namespace nest
{
  bool operator<(const Event &, const Event &);

  class EventPTRPriority
  {
  public:
    typedef Event const * value_type;
    typedef bool return_type;
    bool operator()(Event const *e1, Event const*e2) const
    {
      return !(*e1 < *e2);
    }
  };

  inline
  bool operator<(const Event &e1, const Event &e2)
  {
   return 
     (e1.get_stamp().get_steps()+e1.get_delay()) < (e2.get_stamp().get_steps()+e2.get_delay());
  }

}
#endif
