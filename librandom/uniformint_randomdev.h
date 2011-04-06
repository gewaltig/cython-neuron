/*
    Copyright (C) 2002-2008 The NEST Initiative.
                
    This file is part of NEST.
*/

#ifndef UNIFORMINT_RANDOMDEV_H
#define UNIFORMINT_RANDOMDEV_H

#include <cmath>
#include "randomgen.h"
#include "randomdev.h"
#include "lockptr.h"

/************************************************************/
/* Class UniformIntRNG                                      */
/*                                                          */
/* Generates an RNG which returns integer random numbers    */
/* uniformly distributed between two limits.                */
/*                                                          */
/* Arguments:                                               */
/*  - pointer to an RNG                                     */
/*                                                          */
/* Author:                                                  */
/*  Hans Ekkehard Plesser                                   */
/*                                                          */
/* History:                                                 */
/*  HEP, 2004-08-05                                         */
/*                                                          */
/************************************************************/

namespace librandom {

/*BeginDocumentation
Name: rdevdict::uniformint - uniform integer random deviate generator
Description: Generates uniformly distributed integers between two given limits

  p(n) = 1 / (Nmax - Nmin + 1),   n = Nmin, Nmin+1, ..., Nmax
    
Parameters: 
  Nmin - smallest allowed random number
  Nmax - largest allowed random number

SeeAlso: CreateRDV, RandomArray, rdevdict
Author: Hans Ekkehard Plesser
*/

/**
 * Class UniformIntRandomDev 
 * Create uniformly distributed random integers from a given range
 *
 * @ingroup RandomDeviateGenerators
 */

  class UniformIntRandomDev : public RandomDev
  {

  public:

    // accept only lockPTRs for initialization,
    // otherwise creation of a lock ptr would 
    // occur as side effect---might be unhealthy
    UniformIntRandomDev(RngPtr r_in);
    UniformIntRandomDev();                // threaded

    /**
     * Import sets of overloaded virtual functions.
     * We need to explicitly include sets of overloaded
     * virtual functions into the current scope.
     * According to the SUN C++ FAQ, this is the correct
     * way of doing things, although all other compilers
     * happily live without.
     */
    using RandomDev::uldev;
    
    unsigned long uldev(void);     //!< draw integer
    unsigned long uldev(RngPtr);   //!< draw integer, threaded
    bool has_uldev() const { return true; }

    double operator()(void);           // non-threaded
    double operator()(RngPtr rthrd);   // threaded

    //! set distribution parameters from SLI dict
    void set_status(const DictionaryDatum&);

    //! get distribution parameters from SLI dict
    void get_status(DictionaryDatum&) const; 

  private:
    long nmin_;   //!< smallest permissible number
    long nmax_;   //!< largest permissible number
    long range_;  //!< nmax_ - nmin_ + 1
  };

  inline
  double UniformIntRandomDev::operator()(void)
  {
    return static_cast<double>(uldev());
  }

  inline
  double UniformIntRandomDev::operator()(RngPtr rthrd)
  {
    return static_cast<double>(uldev(rthrd)); 
  }

  inline
  unsigned long UniformIntRandomDev::uldev(void)
  {
    return uldev(rng_);
  }

  inline 
  unsigned long UniformIntRandomDev::uldev(RngPtr r_s)
  {
    return nmin_ + r_s->ulrand(range_);
  }
}

# endif


















