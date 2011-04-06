/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
*/

#ifndef EXP_RANDOMDEV_H
#define EXP_RANDOMDEV_H

#include <cmath>
#include "randomgen.h"
#include "randomdev.h"
#include "lockptr.h"

namespace librandom {

/*BeginDocumentation
Name: rdevdict::exponential - exponential random deviate generator
Description: Generates exponentially distributed random numbers.

  p(x) = exp(-x), x >= 0.     
    
Parameters: No parameters.

SeeAlso: CreateRDV, RandomArray, rdevdict
Author: Hans Ekkehard Plesser
*/

/**
 * Class ExpRandomDev Create exponential random numbers
 *
 * @ingroup RandomDeviateGenerators
 */

  class ExpRandomDev : public RandomDev
  {

  public:

    // accept only lockPTRs for initialization,
    // otherwise creation of a lock ptr would 
    // occur as side effect---might be unhealthy
    ExpRandomDev(RngPtr r_in) : RandomDev(r_in) {} ;
    ExpRandomDev() : RandomDev() {} ;                // threaded

    double operator()(void);           // non-threaded
    double operator()(RngPtr rthrd);   // threaded

    //! set distribution parameters from SLI dict
    void set_status(const DictionaryDatum&) {} 

    //! get distribution parameters from SLI dict
    void get_status(DictionaryDatum&) const {} 

  };

  inline
  double ExpRandomDev::operator()(void)
  {
    return -std::log(rng_->drandpos());
  }

  inline
  double ExpRandomDev::operator()(RngPtr rthrd)
  {
    return -std::log(rthrd->drandpos());
  }

}

# endif
