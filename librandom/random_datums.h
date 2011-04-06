#ifndef RANDOM_DATUMS_H
#define RANDOM_DATUMS_H

/*
 * SLI Datum types related to the NEST random library.
 
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
 */

#include "lockptrdatum.h"

#include "random_numbers.h"
#include "randomgen.h"
#include "randomdev.h"

namespace librandom {
  
/** Encapsulates random number generators in SLI.
 *  @ingroup RandomNumberGenerators
 */
typedef lockPTRDatum<librandom::RandomGen, &RandomNumbers::RngType> RngDatum;

/** Encapsulates random number generator factories in SLI.
 *  @ingroup RandomNumberGenerators
 */
typedef lockPTRDatum<librandom::GenericRNGFactory, 
     		             &RandomNumbers::RngFactoryType> RngFactoryDatum;

/** Encapsulates random deviate generators in SLI.
 *  @ingroup RandomNumberGenerators
 */
typedef lockPTRDatum<librandom::RandomDev, &RandomNumbers::RdvType> RdvDatum;

/** Encapsulates random deviate generator factories in SLI.
 *  @ingroup RandomNumberGenerators
 */
typedef lockPTRDatum<librandom::GenericRandomDevFactory, 
    		             &RandomNumbers::RdvFactoryType> RdvFactoryDatum;
  
}

#endif
