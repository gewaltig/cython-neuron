/*
 *  connectiondatum.h
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

#ifndef CONNECTIONDATUM_H
#define CONNECTIONDATUM_H

/**
 * SLI Datum types related to the NEST kernel.
 */

#include "nestmodule.h"
#include "connection_id.h"
#include "aggregatedatum.h"

#ifndef HAVE_STATIC_TEMPLATE_DECLARATION_FAILS
template<>
sli::pool AggregateDatum<nest::ConnectionID, &nest::NestModule::ConnectionType>::memory;
#endif

typedef AggregateDatum<nest::ConnectionID, &nest::NestModule::ConnectionType> ConnectionDatum;


#endif
