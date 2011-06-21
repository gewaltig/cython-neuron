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

void provide_property(DictionaryDatum &d, Name propname, const std::vector<double> &prop)
{
  Token t = d->lookup(propname);
  assert (t != d->getvoid());

  DoubleVectorDatum* arrd = dynamic_cast<DoubleVectorDatum*>(t.datum());
  assert(arrd != 0);

  if ( (*arrd)->empty() && not prop.empty() ) // not data from before, add
    (*arrd)->insert((*arrd)->end(), prop.begin(), prop.end());

  assert(prop.empty() || **arrd == prop); // not testing for **arrd.empty() since that implies prop.empty()
}


void provide_property(DictionaryDatum &d, Name propname, const std::vector<long> &prop)
{
  Token t = d->lookup(propname);
  assert (t != d->getvoid());

  IntVectorDatum* arrd = dynamic_cast<IntVectorDatum*>(t.datum());
  assert(arrd != 0);

  if ( (*arrd)->empty() && not prop.empty() ) // not data from before, add
    (*arrd)->insert((*arrd)->end(), prop.begin(), prop.end());

  assert(prop.empty() || **arrd == prop); // not testing for **arrd.empty() since that implies prop.empty()
}

void accumulate_property(DictionaryDatum &d, Name propname, const std::vector<double> &prop)
{
  Token t = d->lookup(propname);
  assert (t != d->getvoid());

  DoubleVectorDatum* arrd = dynamic_cast<DoubleVectorDatum*>(t.datum());
  assert(arrd != 0);

  if ( (*arrd)->empty() ) // first data, copy
    (*arrd)->insert((*arrd)->end(), prop.begin(), prop.end());
  else 
  {
    assert((*arrd)->size() == prop.size());

    // add contents of prop to **arrd elementwise
    std::transform((*arrd)->begin(), (*arrd)->end(), prop.begin(), (*arrd)->begin(), std::plus<double>());
  }
}
