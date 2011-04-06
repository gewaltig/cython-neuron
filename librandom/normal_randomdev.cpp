/* 
 * Copyright (C) 2004-2008 The NEST Initiative.
 *
 * This file is part of NEST.
 */

#include <cmath>
#include "config.h"
#include "normal_randomdev.h"

// by default, init as exponential density with mean 1
librandom::NormalRandomDev::NormalRandomDev(RngPtr r_source) 
: RandomDev(r_source)
{}

// threaded
librandom::NormalRandomDev::NormalRandomDev() 
: RandomDev()
{}

double librandom::NormalRandomDev::operator()(RngPtr r)
{
  // Box-Muller algorithm, see Knuth TAOCP, vol 2, 3rd ed, p 122
  // we waste one number
  double V1;
  double V2;
  double S;
  
  do {
    V1 = 2 * r->drand() - 1;
    V2 = 2 * r->drand() - 1;
    S  = V1*V1 + V2*V2;
  } while ( S >= 1 );
  
  if ( S == 0 )
    return 0;
  else
    return V1 * std::sqrt(-2 * std::log(S)/S);  
}
