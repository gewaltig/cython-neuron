/*
 *  oosupport.h
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

#ifndef OOSUPPORT_H
#define OOSUPPORT_H
/* 
    SLI's array access functions
*/
#include "slimodule.h"
#include "slifunction.h"

class OOSupportModule: public SLIModule
{
  class CallMemberFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  CallMemberFunction callmemberfunction;
  public:

  OOSupportModule(void){}

  void init(SLIInterpreter *);
  const std::string  commandstring(void) const;
  const std::string name(void) const;

};

 

#endif
