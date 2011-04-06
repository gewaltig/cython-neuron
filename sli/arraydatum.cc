/*
 *  arraydatum.cc
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

//#include "datumconverter.h"


#include "arraydatum.h"
#include "datumconverter.h"
#include "aggregatedatum_impl.h"
#include "lockptrdatum_impl.h"
#include <iterator>

// explicit instantiations
template class AggregateDatum<TokenArray,&SLIInterpreter::Arraytype>;
template class AggregateDatum<TokenArray,&SLIInterpreter::Proceduretype>;
template class AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>;
template class lockPTRDatum<std::vector<long> ,&SLIInterpreter::IntVectortype>;
template class lockPTRDatum<std::vector<double> ,&SLIInterpreter::DoubleVectortype>;


// initialization of static members requires template<>
// see Stroustrup C.13.1 --- HEP 2001-08-09
template<> sli::pool 
 AggregateDatum<TokenArray,&SLIInterpreter::Arraytype>::memory(
           sizeof(ArrayDatum),10240,1);
template<> sli::pool 
 AggregateDatum<TokenArray,&SLIInterpreter::Proceduretype>::memory(
           sizeof(ProcedureDatum),10240,1);
template<> sli::pool 
 AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>::memory(
           sizeof(AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>),10240,1);


template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Arraytype>::pprint(std::ostream &out) const
{
  out << '[';
  Token *i = this->begin();
  while(i!= this->end())
  {
    (*i)->pprint(out); 
    ++i;
    if(i != this->end())
      out << ' ';
  }
  out << ']';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Arraytype>::print(std::ostream &out) const
{
  out << '<' << this->gettypename() << '>';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Proceduretype>::pprint(std::ostream &out) const
{
  out << '{';
  Token *i = this->begin();
  while(i!= this->end())
  {
    (*i)->pprint(out); 
    ++i;
    if(i != this->end())
      out << ' ';
  }
  out << '}';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Proceduretype>::list(std::ostream &out, std::string prefix,
								     int line) const
{
  int lc=0;

  if(line==lc)
    out << "-->" << prefix << '{' << std::endl;
  else
    out << "   " << prefix << '{' << std::endl;
    
  prefix="   "+prefix;
  
  Token *i = this->begin();

  while(i!= this->end())
  {
    if(lc != line)
      (*i)->list(out, prefix, -1);
    else
      (*i)->list(out, prefix, 0);
    out << std::endl;
    ++lc;
    ++i;
  }
  out << prefix << '}';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Proceduretype>::print(std::ostream &out) const
{
  out << '<'<< this->gettypename()<<'>';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>::pprint(std::ostream &out) const
{
  out << "/{";
  Token *i = this->begin();
  while(i!= this->end())
  {
    (*i)->pprint(out); 
    ++i;
    if(i != this->end())
      out << ' ';
  }
  out << '}';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>::list(std::ostream &out, std::string prefix, 
									int line) const
{

  Token *i = this->begin();

  if(line==0)
    out << "-->" << prefix << '{' << std::endl;
  else
    out << "   " << prefix << '{' << std::endl;
  prefix="   "+prefix;

  while(i!= this->end())
  {
    (*i)->list(out, prefix, -1);
    out << std::endl;
    ++i;
  }
  out << prefix << '}';
}

template<>
void AggregateDatum<TokenArray,&SLIInterpreter::Litproceduretype>::print(std::ostream &out) const
{
  out << '<'<< this->gettypename() <<'>';
}

template<>
void lockPTRDatum<std::vector<long> ,&SLIInterpreter::IntVectortype>::pprint(std::ostream &out) const
{
  std::vector<long>* v = this->get();
  out << "[# ";
  std::copy(v->begin(), v->end(), std::ostream_iterator<long>(out, " "));
  out << "#]";
  this->unlock();
}

template<>
void lockPTRDatum<std::vector<double> ,&SLIInterpreter::DoubleVectortype>::pprint(std::ostream &out) const
{
  std::vector<double>* v = this->get();
  out << "[. ";
  std::copy(v->begin(), v->end(), std::ostream_iterator<double>(out, " "));
  out << ".]";
  this->unlock();
}
