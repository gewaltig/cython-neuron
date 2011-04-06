/*
 *  sligraphics.h
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

#ifndef SLIPGM_H
#define SLIPGM_H

/*
 *  sligraphics.h
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

#include <iostream>
#include <string>
#include <vector>
#include "stringdatum.h"
#include "fdstream.h"
#include "slimodule.h"
#include "slifunction.h"
#include "interpret.h"


using std::vector;
using std::string;

class SLIgraphics: public SLIModule
{

  class ReadPGMFunction: public SLIFunction
  {
  private:
    std::istream * openPGMFile(StringDatum*) const;  //!< opens the file
    void readMagicNumber(std::istream*, char[2]) const; //!< reads the magic number into string magic
    void initRead(std::istream*, int&, int&, int &) const;        //!< reads width, height, maxval
    void readImage(std::istream*, char[2], vector<long> &, int, int, int) const;       //!< reads the image
    
  public:
    virtual void execute(SLIInterpreter*) const;

    
  };

  class WritePGMFunction: public SLIFunction
  {
  public:
    virtual void execute(SLIInterpreter*) const;
  };

  ReadPGMFunction  readpgmfunction;
  WritePGMFunction writepgmfunction;

public:

  SLIgraphics(){}
  
  void init(SLIInterpreter *);
  const string name(void) const;
  const string commandstring(void) const;

};


#endif
