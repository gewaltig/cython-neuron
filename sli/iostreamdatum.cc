/*
 *  iostreamdatum.cc
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


#include "lockptrdatum_impl.h"
#include "interpret.h"

template class lockPTRDatum<std::istream,&SLIInterpreter::Istreamtype>;
template class lockPTRDatum<std::istream,&SLIInterpreter::XIstreamtype>;
template class lockPTRDatum<std::ostream,&SLIInterpreter::Ostreamtype>;
