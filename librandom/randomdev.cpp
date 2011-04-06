/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
*/

#include "randomdev.h"

// These two functions must not be inlined
// Otherwise, the Compaq C++ Compiler goes haywire
// HEP, 2004-06-29
unsigned long librandom::RandomDev::uldev(void)
{
  assert(false);
  return 0;
}

unsigned long librandom::RandomDev::uldev(RngPtr)
{
  assert(false);
  return 0;
}

