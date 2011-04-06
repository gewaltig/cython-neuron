#ifndef GSLRANDOMGEN_H
#define GSLRANDOMGEN_H
/*
    Interface to GSL Random Number Generators
 
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST
*/
#include <cassert>
#include <list>
#include <string>
#include "config.h"
#include "dictdatum.h"
#include "randomgen.h"
#include "random_datums.h"

// essential GSL includes or replacements
// GSL Versions < 1.2 have weak MT seeding
#ifdef HAVE_GSL_1_2

// "Real" version in presence of GSL

#include <gsl/gsl_rng.h>
    
namespace librandom {

  /**
   * class GslRandomGen
   * C++ wrapper for GSL/GSL-style generators.
   * @note
   * This class should only be used within librandom.
   *
   * @ingroup RandomNumberGenerators
   */

  class GslRandomGen : public RandomGen
  {

  public:
    explicit GslRandomGen(const gsl_rng_type *,  //!< given RNG, given seed
		                      unsigned long);
  
    ~GslRandomGen();

    //! Add all GSL RNGs to rngdict
    static void add_gsl_rngs(DictionaryDatum&);

  private:
    void           seed_(unsigned long);
    double         drand_(void);

  private:
    gsl_rng *rng_;

  };

  inline
  void GslRandomGen::seed_(unsigned long s)   
  { 
    gsl_rng_set(rng_, s);     
  }

  inline
  double GslRandomGen::drand_(void)  
  { 
    return gsl_rng_uniform(rng_);     
  }

  //! Factory class for GSL-based random generators
  class GslRNGFactory : public GenericRNGFactory
  {
  public:
    GslRNGFactory(gsl_rng_type const * const);
    RngPtr create(unsigned long) const;
  private:
    //! GSL generator type information
    gsl_rng_type const * const gsl_rng_;
  };

}

#else

// NO GSL Available---Implement class as empty shell
namespace librandom {

  class GslRandomGen : public RandomGen
  {
  public:
    //! Add all GSL RNGs to rngdict
    //! Do nothing if GSL not available
    static void add_gsl_rngs(DictionaryDatum&) {}

  private:
    GslRandomGen() {assert(false);}   
    ~GslRandomGen() {assert(false);}

  private:
    void           seed_(unsigned long s);
    double         drand_(void);

  };

  inline
  void GslRandomGen::seed_(unsigned long s)   
  { 
    (void) s; //remove warning of unused variable
    assert(false);
  }

  inline
  double GslRandomGen::drand_(void)  
  { 
    assert(false);
  }

}

#endif


#endif
