/*
 *  iostreamdatum.h
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

#ifndef IOSTREAMDATUM_H
#define IOSTREAMDATUM_H
/* 
    Datum classes encapsulating c++ streams 
*/

// Include all headers, needed to use token and datum objects
#include <typeinfo>
#include <iostream>

#include "lockptrdatum.h"
#include "interpret.h"

typedef lockPTRDatum<std::istream,&SLIInterpreter::Istreamtype>  IstreamDatum;
typedef lockPTRDatum<std::istream,&SLIInterpreter::XIstreamtype> XIstreamDatum;
typedef lockPTRDatum<std::ostream,&SLIInterpreter::Ostreamtype>  OstreamDatum;
//typedef lockPTRDatum<std::iostream,&SLIInterpreter::IOstreamtype>  IOstreamDatum;

#endif
