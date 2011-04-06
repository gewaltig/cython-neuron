/*
 *  slifunction.h
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

#ifndef SLIFUNCTION_H
#define SLIFUNCTION_H
/* 
    Base class for all SLI functions.
*/

class SLIInterpreter;

/*
  class SLICommand replaces the old class Function from SYNOD 1.x.
 */

class SLIFunction
{
public:
  SLIFunction(){}
  virtual void execute(SLIInterpreter *) const = 0;
  virtual ~SLIFunction(){}

  /**
   * Show stack backtrace information on error.
   * This function tries to extract and display useful
   * information from the execution stack if an error occurs.
   * This function should be implemented for all functions which
   * store administrative information on the execution stack.
   * Examples are: loops and procedure iterations.
   * backtrace() is only called, if the interpreter flag
   * show_backtrace is set.
   */
  virtual void backtrace(SLIInterpreter *, int) const
  {}
};

#endif

