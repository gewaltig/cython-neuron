/*
 *  model.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "model.h"
#include "exceptions.h"
#include "dictutils.h"
#include "nestmodule.h"
#include <algorithm>

namespace nest
{

  Model::Model(const std::string& name)
    : name_(name),
      memory_()
  {}
  
  void Model::set_threads()
  {
    set_threads_(NestModule::get_num_threads());
  }
  
  void Model::set_threads_(thread t)
  {
    for (size_t i = 0; i < memory_.size(); ++i)
      if ( memory_[i].get_instantiations() > 0 )
	throw KernelException();

    std::vector<sli::pool> tmp(t); 
    memory_.swap(tmp);

    for (size_t i = 0; i < memory_.size(); ++i)
      init_memory_(memory_[i]);
  }

  void Model::reserve(thread t, size_t s)
  {
    assert((size_t)t < memory_.size());    
    memory_[t].reserve(s);
  }

  void Model::clear()
  {
    std::vector<sli::pool> mem;
    memory_.swap(mem);
    set_threads_(1);
  }

  size_t Model::mem_available()
  {
    size_t result = 0;
    for (size_t t = 0; t < memory_.size(); ++t)
      result += memory_[t].available();
    
    return result;
  }

  size_t Model::mem_capacity()
  {
    size_t result = 0;
    for (size_t t = 0; t < memory_.size(); ++t)
      result += memory_[t].get_total();

    return result;
  }

  void Model::set_status(DictionaryDatum d)
  {
    set_status_(d);
  }

  DictionaryDatum Model::get_status(void)
  {
    DictionaryDatum d=get_status_();
    
    std::vector<long> tmp(memory_.size());
    for(size_t t=0; t< tmp.size(); ++t)
      tmp[t]= memory_[t].get_instantiations();

    (*d)["instantiations"]= Token(tmp);

    for(size_t t=0; t< tmp.size(); ++t)
      tmp[t]= memory_[t].get_total();

    (*d)["capacity"]=Token(tmp);

    for(size_t t=0; t< tmp.size(); ++t)
      tmp[t]= memory_[t].available();

    (*d)["available"]= Token(tmp);

    (*d)["model"]=LiteralDatum(get_name());
    return d;
  }

} // namespace
