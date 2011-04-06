/*
 *  matlabeng.h
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


// ********************************************************************
// * Interface to matlab Engine                             
// * --------------------------
// *
// *  compiles with:
// * 
// *  setenv LD_LIBRARY_PATH /opt/matlab5/extern/lib/lnx86/
// *  g++ -Wall -ansi -I /opt/matlab5/extern/include -o engdemo engdemo.cc  
// *            -L /opt/matlab5/extern/lib/lnx86 -leng -lmx
// *
// * based on:  engdemo.c (Mathworks)
// *
// * History:
// *         (0) first version
// *            3.4.1998, Diesmann, Freiburg
// *
// ********************************************************************


#include <string>
#ifdef HAVE_CONFIG_H
#include "Config.h"
#endif
#ifdef HAVE_MATLAB
#include "engine.h"
#endif

class MatlabEngine
{
 Engine *ep;
 bool alive;

 char *buffer;

 public:

  MatlabEngine(string const & =string(), int =256);
  ~MatlabEngine();
  bool good(void) const;
  operator bool() const;
  bool evalstring(string const &);
  bool setoutputbuffer(int);
  string getoutputbuffer(void) const;
};
