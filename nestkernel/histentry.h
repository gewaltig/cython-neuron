/*
 *  histentry.h
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
 * \file histentry.h
 * Part of definition of Archiving_Node which is capable of
 * recording and managing a spike history. 
 * \author Moritz Helias, Abigail Morrison
 * \note moved to separate file to avoid circular inclusion in node.h
 * \date april 2006
 */

#ifndef HISTENTRY_H
#define HISTENTRY_H

#include "nest.h"

namespace nest {

// entry in the spiking history
  class histentry
  {
    public:
      histentry(double_t t, double_t Kminus, double_t triplet_Kminus, size_t access_counter);

      double_t t_;              // point in time when spike occurred (in ms)
      double_t Kminus_;         // value of Kminus at that time
      double_t triplet_Kminus_; // value of triplet STDP Kminus at that time
      size_t access_counter_;   // how often this entry was accessed (to enable removal,
                                // once read by all neurons which need it)
  };

}

#endif
