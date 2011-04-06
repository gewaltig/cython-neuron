/*
    Copyright (C) 2002-2008 The NEST Initiative.
                
    This file is part of NEST.
*/

#include "uniformint_randomdev.h"
#include "dictutils.h"

#include <cmath>
#include <stdexcept>

// by default, init as exponential density with mean 1
librandom::UniformIntRandomDev::UniformIntRandomDev(RngPtr r_source)
  : RandomDev(r_source), 
    nmin_(0),
    nmax_(0),
    range_(nmax_-nmin_+1)
{ 
  assert(range_ > 0);
}

librandom::UniformIntRandomDev::UniformIntRandomDev() 
: RandomDev(), 
    nmin_(0),
    nmax_(0),
    range_(nmax_-nmin_+1)
{ 
  assert(range_ > 0);
}


void librandom::UniformIntRandomDev::set_status(const DictionaryDatum& d)
{
  updateValue<long>(d, "nmin", nmin_);
  updateValue<long>(d, "nmax", nmax_);
  range_ = nmax_ - nmin_ + 1;

  if ( range_ < 1 )
    throw std::out_of_range("UniformIntRandomDev::set_status: range >= 1 required.");
} 

void librandom::UniformIntRandomDev::get_status(DictionaryDatum &d) const 
{
  def<long>(d, "nmin", nmin_);
  def<long>(d, "nmax", nmax_);
}
