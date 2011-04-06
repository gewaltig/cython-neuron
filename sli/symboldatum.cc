/*
 *  symboldatum.cc
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

#include "symboldatum.h"
#include "datumconverter.h"
#include "aggregatedatum_impl.h"

// explicit template instantiation needed
// because otherwise methods defined in
// numericdatum_impl.h will not be instantiated
// Moritz, 2007-04-16
template class AggregateDatum<Name,&SLIInterpreter::Symboltype>;


// initialization of static members requires template<>
// see Stroustrup C.13.1 --- HEP 2001-08-09
template<> sli::pool AggregateDatum<Name,&SLIInterpreter::Symboltype>::memory(
    sizeof(SymbolDatum),100,1);

