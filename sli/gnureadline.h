/*
 *  gnureadline.h
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

#ifndef GNUREADLINE_H
#define GNUREADLINE_H

#include "config.h"
#ifdef HAVE_READLINE

#include "slitype.h"
#include "slimodule.h" 

class GNUReadline: public SLIModule
{
  public:
  
  GNUReadline(){}
  ~GNUReadline();

  const std::string name(void) const
    {
      return "GNUReadline";
    }

  void init(SLIInterpreter *);

  class GNUReadlineFunction: public SLIFunction  
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class GNUAddhistoryFunction: public SLIFunction  
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  GNUReadlineFunction   gnureadlinefunction;
  GNUAddhistoryFunction gnuaddhistoryfunction;
};

#endif //HAVE_READLINE
  
#endif
