/*
 *  tokenarray.cc
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

#include "tokenarray.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "stringdatum.h"
#include "tokenutils.h"


const TokenArray & TokenArray::operator=(const TokenArray &a)
{
    ++a.data->refs; // protect from a=a
    if( -- data->refs == 0)
        delete data;
    data = a.data;

    return *this;
}


TokenArray::TokenArray(const std::vector<long> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token idt(new IntegerDatum(a[i]));
    (*data)[i].move(idt);
  }
}

TokenArray::TokenArray(const std::vector<size_t> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token idt(new IntegerDatum(a[i]));
    (*data)[i].move(idt);
  }
}

TokenArray::TokenArray(const std::vector<double> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token ddt(new DoubleDatum(a[i]));
    (*data)[i].move(ddt);
  }
}

TokenArray::TokenArray(const std::valarray<long> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token ddt(new IntegerDatum(a[i]));
    (*data)[i].move(ddt);
  }
}

TokenArray::TokenArray(const std::valarray<double> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token ddt(new DoubleDatum(a[i]));
    (*data)[i].move(ddt);
  }
}

TokenArray::TokenArray(const std::valarray<float> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token ddt(new DoubleDatum(a[i]));
    (*data)[i].move(ddt);
  }
}

TokenArray::TokenArray(const std::vector<float> &a)
  : data( new TokenArrayObj(a.size(),Token(),0))
{
  assert(data != NULL);
  for(size_t i=0; i< a.size(); ++i)
  {
    Token ddt(new DoubleDatum(a[i]));
    (*data)[i].move(ddt);
  }
}



void TokenArray::toVector(std::vector<long> &a) const
{
  a.clear();
  a.reserve(size());
  for(Token *idx=begin(); idx!= end(); ++idx)
    {
      IntegerDatum *targetid = dynamic_cast<IntegerDatum *>(idx->datum());
      if(targetid == NULL)
        {
	  IntegerDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
      
      a.push_back(targetid->get());
    }
}

void TokenArray::toVector(std::vector<size_t> &a) const
{
  a.clear();
  a.reserve(size());
  for(Token *idx=begin(); idx!= end(); ++idx)
    {
      IntegerDatum *targetid = dynamic_cast<IntegerDatum *>(idx->datum());
      if(targetid == NULL)
        {
	  IntegerDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
	   
      a.push_back(targetid->get());
    }
}

void TokenArray::toVector(std::vector<double> &a) const
{
  a.clear();
  a.reserve(size());
  for(Token *idx=begin(); idx!= end(); ++idx)
    {
      DoubleDatum *targetid = dynamic_cast<DoubleDatum *>(idx->datum());
      if(targetid == NULL)
        {
	  DoubleDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
      a.push_back(targetid->get());
    }
}

void TokenArray::toVector(std::vector<std::string> &a) const
{
  a.clear();
  a.reserve(size());
  for(Token *idx=begin(); idx!= end(); ++idx)
    {
      std::string *target = dynamic_cast<std::string*>(idx->datum());
      if(target == NULL)
        {
	  StringDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
      a.push_back(*target);
    }
}

void TokenArray::toValarray(std::valarray<long> &a) const
{
  a.resize(size());
  size_t i=0;

  for(Token *idx=begin(); idx!= end(); ++idx, ++i)
    {
      IntegerDatum *targetid = dynamic_cast<IntegerDatum *>(idx->datum());
      if(targetid == NULL)
        {
	  IntegerDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
      a[i]=targetid->get();
    }
}

void TokenArray::toValarray(std::valarray<double> &a) const
{
  a.resize(size());
  size_t i=0;

  for(Token *idx=begin(); idx!= end(); ++idx, ++i)
    {
      DoubleDatum *targetid = dynamic_cast<DoubleDatum *>(idx->datum());
      if(targetid == NULL)
        {
	  DoubleDatum const d;
	  throw TypeMismatch(d.gettypename().toString(), 
			     idx->datum()->gettypename().toString());
        }   
      a[i]=targetid->get();
    }
}

bool TokenArray::valid(void) const
{
  if(data == NULL)
  {
    return false;
  }
  return data->valid();
}
  

std::ostream & operator<<(std::ostream& out, const TokenArray&a)
{
    
    for(Token *t = a.begin(); t < a.end() ; ++t)
        out << *t << ' ';

    return out;
    
}
  
