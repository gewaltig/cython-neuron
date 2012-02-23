/*
 *  get_mem_bgp.c
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
#ifdef IS_BLUEGENE_P

#include <spi/bgp_SPI.h>

long get_heap_mem_bgp()
{
  uint32_t heap_memory = 0;
  Kernel_GetMemorySize(KERNEL_MEMSIZE_HEAP, &heap_memory);
  return (long)heap_memory;
}

long get_stack_mem_bgp()
{
  uint32_t stack_memory = 0;
  Kernel_GetMemorySize(KERNEL_MEMSIZE_STACK, &stack_memory);
  return (long)stack_memory;
}

#endif /* #ifdef IS_BLUEGENE_P */
