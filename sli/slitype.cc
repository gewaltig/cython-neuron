/*
 *  slitype.cc
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

/* 
    class implementing SLI types.
*/
#include "slitype.h"
#include <cstdlib>

void SLIType::settypename(const std::string &s)
{
    if(count == 0)
    {
        assert(Name::lookup(s) == false);
        name = new Name(s);
    }
    else
    {
        assert(Name(s) == *name);
    }
    ++count;
}

void SLIType::deletetypename(void)
{
    assert(count>0);
    
    if(count==1)
        delete name;
    --count;
}


void SLIType::setdefaultaction(SLIFunction &c)
{
    if(defaultaction ==NULL)
        defaultaction = &c;
    else assert(&c == defaultaction);        
}

