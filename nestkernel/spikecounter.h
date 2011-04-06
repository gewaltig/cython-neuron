/*
 *  spikecounter.h
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
 * \file spikecounter.h
 * Part of definition of volume_transmitter to record and manage 
 * spike times and multiplicity of neurons releasing a 
 * neuromodulator, which is needed for neuromodulated synaptic plasticity
 * (volume transmitter and neuromodulated synapses 
 * are not included in the release version of NEST at the moment). 
 * \author Wiebke Potjans
 * \note moved to separate file to avoid circular inclusion in node.h
 * \date december 2008
 */

#ifndef SPIKECOUNTER_H
#define SPIKECOUNTER_H

#include "nest.h"

namespace nest {

// entry in the spiking history
  class spikecounter
  {
    public:
      spikecounter(double_t spike_time, double_t multiplicity);

      double_t spike_time_;              // point in time when spike occurred (in ms)
      double_t multiplicity_;
  };

}

#endif
