/*
 *  spikecounter.cpp
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
 * \file spikecounter.cpp
 * Implementation of volume_transmitter to record and manage spike times and
 * multiplicity of neurons releasing a neuromodulator
 * (volume_transmitter is not included in the current release version of NEST)
 * \author Wiebke Potjans
 * \date december 2008
 * \note moved to separate file to avoid circular inclusion in node.h
 */

#include "spikecounter.h"

nest::spikecounter::spikecounter(double_t spike_time, double_t multiplicity) :
    spike_time_(spike_time), multiplicity_(multiplicity)
  { }

