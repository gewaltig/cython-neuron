/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
*/

#include "randomgen.h"
#include "knuthlfg.h"

const size_t librandom::RandomGen::DEFAULT_BUFFSIZE = 128 * 1024;
const unsigned long librandom::RandomGen::DefaultSeed = 0xd37ca59fUL;  

librandom::RandomGen::RandomGen() :
  next_(0),
  end_(0)
{
  set_buffsize(DEFAULT_BUFFSIZE);  // resets pointers as well

  // The buffer is only filled when the first number is drawn
  // This is crucial, since the constructor of a derived RNG
  // class may seed the RNG at a point beyond the control of
  // this RandomGen constructor.  Therefore, we MUST fill the
  // buffer AFTER the derived classes' constructor has run. 
}

void librandom::RandomGen::set_buffsize(const size_t buffsize)
{
  assert(buffsize > 0);

  // do nothing if buffer size doesn't change
  if ( buffsize != buffer_.size() )
    {
      buffer_.resize(buffsize);
      end_ = buffer_.end();
      next_ = end_;          // force reload on next draw
    }

  return;
}

void librandom::RandomGen::seed(const unsigned long n)
{
  // pass seed on to RNG's seeding function
  seed_(n);

  // force reload on next draw
  next_ = end_;
}

void librandom::RandomGen::refill_(void)
{
  std::vector<double>::iterator i;

  for ( i = buffer_.begin(); i != buffer_.end() ; ++i )
    *i = drand_();

  next_ = buffer_.begin();
}

librandom::RngPtr librandom::RandomGen::create_knuthlfg_rng(unsigned long seed)
{
  return librandom::RngPtr(new librandom::KnuthLFG(seed));
}
