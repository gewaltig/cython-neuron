/*
 *  doubledatum.cc
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

#include <iomanip>
#include "doubledatum.h"
#include "numericdatum_impl.h"

// explicit template instantiation needed
// because otherwise methods defined in
// numericdatum_impl.h will not be instantiated
// Moritz, 2007-04-16
template class NumericDatum<double,&SLIInterpreter::Doubletype>;


// initialization of static members requires template<>
// see Stroustrup C.13.1 --- HEP 2001-08-09
template<> sli::pool NumericDatum<double,&SLIInterpreter::Doubletype>::memory(
    sizeof(DoubleDatum),1024,1);

template <>
void NumericDatum<double,&SLIInterpreter::Doubletype>::input_form(std::ostream &o) const
{
  o.setf(std::ios::scientific);
  o << this->d;
  o.unsetf(std::ios::scientific);
}

template <>
void NumericDatum<double,&SLIInterpreter::Doubletype>::pprint(std::ostream &o) const
{
  o.setf(std::ios::scientific);
  o << this->d;
  o.unsetf(std::ios::scientific);
}
