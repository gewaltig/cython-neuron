/*
 *  bg_get_mem.c
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  First Version: January 2012
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "config.h"

#if defined IS_BLUEGENE_P || defined IS_BLUEGENE_Q

#if defined IS_BLUEGENE_P
  #include <spi/bgp_SPI.h>
  typedef uint32_t bgmemsize_t;
#elif defined IS_BLUEGENE_Q
  #include <kernel/memory.h>
  typedef uint64_t bgmemsize_t;
#endif

unsigned long bg_get_heap_mem()
{
  bgmemsize_t memory = 0;
  Kernel_GetMemorySize(KERNEL_MEMSIZE_HEAP, &memory);
  return (unsigned long)memory;
}

unsigned long bg_get_stack_mem()
{
  bgmemsize_t memory = 0;
  Kernel_GetMemorySize(KERNEL_MEMSIZE_STACK, &memory);
  return (unsigned long)memory;
}

#endif
