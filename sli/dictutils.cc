/*
 *  dictutils.cc
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

#include "dictutils.h"

void initialize_property_array(DictionaryDatum &d, Name propname)
{
  Token t = d->lookup(propname);
  if (t == d->getvoid())
  {
    ArrayDatum arrd;
    def<ArrayDatum>(d, propname, arrd);
  }
}

void initialize_property_doublevector(DictionaryDatum &d, Name propname)
{
  Token t = d->lookup(propname);
  if (t == d->getvoid())
  {
    DoubleVectorDatum arrd(new std::vector<double>);
    def<DoubleVectorDatum>(d, propname, arrd);
  }
}

void initialize_property_intvector(DictionaryDatum &d, Name propname)
{
  Token t = d->lookup(propname);
  if (t == d->getvoid())
  {
    IntVectorDatum arrd(new std::vector<long>);
    def<IntVectorDatum>(d, propname, arrd);
  }
}



