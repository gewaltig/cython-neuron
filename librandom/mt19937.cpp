/*
    This file is part of NEST.
*/

#include "mt19937.h"

const unsigned int  librandom::MT19937::N = 624;
const unsigned int  librandom::MT19937::M = 397;
const unsigned long librandom::MT19937::MATRIX_A = 0x9908b0dfUL;  
const unsigned long librandom::MT19937::UPPER_MASK = 0x80000000UL;
const unsigned long librandom::MT19937::LOWER_MASK = 0x7fffffffUL;
const double        librandom::MT19937::I2DFactor_ = 1.0/4294967296.0;

librandom::MT19937::MT19937(unsigned long s) :
  mt(N),
  mti(N+1)  
{
  init_genrand(s);
}

void librandom::MT19937::seed_(unsigned long s)
{
  init_genrand(s);
}

void librandom::MT19937::init_genrand(unsigned long s)
{
  mt[0]= s & 0xffffffffUL;
  for (mti=1; static_cast<unsigned int>(mti)<N; mti++) {
      mt[mti] = 
    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
      /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
      /* In the previous versions, MSBs of the seed affect   */
      /* only MSBs of the array mt[].                        */
      /* 2002/01/09 modified by Makoto Matsumoto             */
      mt[mti] &= 0xffffffffUL;
      /* for >32 bit machines */
  }
}

unsigned long librandom::MT19937::genrand_int32()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (static_cast<unsigned int>(mti) >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;static_cast<unsigned int>(kk)<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;static_cast<unsigned int>(kk)<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}


