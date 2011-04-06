/*
 *  matlabeng.cc
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


#include "matlabeng.h"
#include <cassert>


MatlabEngine::MatlabEngine(string const & s, int n)
  : buffer(NULL)
{
 ep = engOpen(s.c_str());
 alive = (ep!=NULL);
 
 if (alive)
  setoutputbuffer(n);
}

MatlabEngine::~MatlabEngine()
{
 if (alive)
  engClose(ep);
 delete [] buffer;
}

bool MatlabEngine::good(void) const
{
 return alive;
}

MatlabEngine::operator bool() const
{
 return good();
}

bool MatlabEngine::evalstring(string const & s)
{
 assert(alive);

 alive = (engEvalString(ep,s.c_str())==0);
 return good();
}

bool MatlabEngine::setoutputbuffer(int n)
{
 assert(alive);

 delete [] buffer;
 buffer = new char[n];
 alive = (buffer!=NULL);
 if (alive)
  engOutputBuffer(ep,buffer,n);

 return alive;
}

string MatlabEngine::getoutputbuffer(void) const
{
 assert(alive);

 return string(buffer);
}




