/*
 *  numericdatum_impl.h
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

#ifndef NUMERICDATUMIMPL_H
#define NUMERICDATUMIMPL_H

#include "datumconverter.h"
#include "numericdatum.h"


template<class D, SLIType *slt>
void NumericDatum<D,slt>::input_form(std::ostream &o) const
{
  pprint(o);
}

template<class D, SLIType *slt>
void NumericDatum<D,slt>::pprint(std::ostream &o) const
{
  this->print(o);
}

/**
 * Accept a DatumConverter to this datum.
 * A DatumConverter (visitor) may be used to make a conversion to a type, which is not known to NEST.
 * (visitor pattern).
 */
template<class D, SLIType *slt>
void NumericDatum<D,slt>::use_converter(DatumConverter &converter)
{
  converter.convert_me(*this); // call visit with our own type here
                  // this will call the approproate implementation of the derived class
}


#endif
