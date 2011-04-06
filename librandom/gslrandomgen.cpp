/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
 */

#include "gslrandomgen.h"

#ifdef HAVE_GSL_1_2

// nothing if GSL 1.2 or later not available

librandom::GslRandomGen::GslRandomGen(const gsl_rng_type *type, unsigned long seed) 
  : RandomGen()
{ 
  rng_ = gsl_rng_alloc(type); 
  assert(rng_ != NULL);
  gsl_rng_set(rng_, seed);  
}

librandom::GslRandomGen::~GslRandomGen() 
{ 
  gsl_rng_free(rng_); 
} 

// function initializing RngList
// add further self-implemented RNG below
void librandom::GslRandomGen::add_gsl_rngs(DictionaryDatum& rngdict)
{
  // add all standard GSL RNG, or those from GSL replacement
  const gsl_rng_type **t0 = gsl_rng_types_setup();
  for (const gsl_rng_type **t = t0; *t != NULL; ++t)
  {
    assert(*t != NULL);
    const std::string name = std::string("gsl_") + (*t)->name;
    
    if ( !rngdict->known( name ) )  // avoid multiple insertion
    {
      GslRNGFactory* f = new GslRNGFactory(*t);
      assert(f != NULL);
    
      Token rngfactory = new librandom::RngFactoryDatum(*f);
      rngdict->insert_move(Name(name), rngfactory);
    }
  }
}

librandom::GslRNGFactory::GslRNGFactory(gsl_rng_type const * const t) : 
  gsl_rng_(t) 
{
  assert(t != 0);
}

librandom::RngPtr librandom::GslRNGFactory::create(unsigned long s) const
{
  return RngPtr(new GslRandomGen(gsl_rng_, s));
}

#endif
