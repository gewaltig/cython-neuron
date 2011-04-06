/*
 *  functiondatum.cc
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "functiondatum.h"

sli::pool FunctionDatum::memory(sizeof(FunctionDatum),1024,1);

void FunctionDatum::backtrace(SLIInterpreter *i, int p) const
{
  action->backtrace(i,p);
}
