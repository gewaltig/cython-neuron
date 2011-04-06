/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
*/

#ifndef NORMAL_RANDOMDEV_H
#define NORMAL_RANDOMDEV_H

#include <cmath>
#include "randomgen.h"
#include "randomdev.h"
#include "lockptr.h"


namespace librandom {

/*BeginDocumentation
Name: rdevdict::normal - normal random deviate generator
Description: Generates normally distributed random numbers.

  p(x) = 1 / \sqrt{2 pi} * exp (-x^2 / 2)    
    
Parameters: No parameters.

SeeAlso: CreateRDV, RandomArray, rdevdict
Author: Hans Ekkehard Plesser
*/

/**
 * Create normal (Gaussian) random numbers with uniform variance.
 * The numbers are generated using the polar method.
 *
 * @note We cannot keep the second deviate until the next call,
 *       since the generator may be called by a different thread
 *       with its own RNG.7
 *
 * @ingroup RandomDeviateGenerators
 */

  class NormalRandomDev : public RandomDev
  {

  public:

    // accept only lockPTRs for initialization,
    // otherwise creation of a lock ptr would 
    // occur as side effect---might be unhealthy
    NormalRandomDev(RngPtr);
    NormalRandomDev();          // threaded

    double operator()(void);
    double operator()(RngPtr);  // threaded

    //! set distribution parameters from SLI dict
    void set_status(const DictionaryDatum&) {}

    //! get distribution parameters from SLI dict
    void get_status(DictionaryDatum&) const {} 

  };

  inline 
  double NormalRandomDev::operator()(void)
  {
    return (*this)(rng_);
  }

}

# endif


















