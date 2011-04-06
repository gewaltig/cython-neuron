/*
 *  connectiondatum.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "connectiondatum.h"
#include "datumconverter.h"
#include "aggregatedatum_impl.h"

// explicit instantiation
template class AggregateDatum<nest::ConnectionID, &nest::NestModule::ConnectionType>;

template<> sli::pool 
 AggregateDatum<nest::ConnectionID, &nest::NestModule::ConnectionType>::memory(
           sizeof(nest::ConnectionID),10240,1);
