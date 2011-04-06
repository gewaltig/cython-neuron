/*
 *  utils.cc
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

#include "utils.h"
#include "integerdatum.h"

bool array2vector(std::vector<long> &v, const TokenArray &a)
{
  bool status=true;

  v.reserve(a.size());
  for(Token *t=a.begin(); t != a.end(); ++t)
  {
    IntegerDatum *id= dynamic_cast<IntegerDatum *>(t->datum());
    if( id==NULL )
    {
      status =false;
      break;
    }
    v.push_back(id->get());
  }
  return status;
}
