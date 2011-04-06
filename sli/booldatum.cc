/*
 *  booldatum.cc
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

#include "booldatum.h"
#include "token.h"
#include "name.h"
#include "datumconverter.h"

sli::pool  BoolDatum::memory(sizeof(BoolDatum),1024,1);

const char* BoolDatum::true_string="true";
const char* BoolDatum::false_string="false";

BoolDatum::BoolDatum(const Name& val)
{
  d = (val==Name(true_string));
}

BoolDatum::operator Name() const
{
  return (d ? Name(true_string):Name(false_string));
}

BoolDatum::operator std::string() const
{
  return (d ? std::string(true_string) : std::string(false_string));
}

void BoolDatum::input_form(std::ostream &out) const
{
  print(out);
}

void BoolDatum::pprint(std::ostream &out) const
{
  print(out);
}

void BoolDatum::print(std::ostream &out) const
{
  out << (d ? true_string : false_string);
}

void BoolDatum::use_converter(DatumConverter &converter)
{
  converter.convert_me(*this);
}

void * BoolDatum::operator new(size_t size)
{
  if(size != memory.size_of())
    return ::operator new(size);
  return memory.alloc();
}

void BoolDatum::operator delete(void *p, size_t size)
{
  if(p == NULL)
    return;
  if(size != memory.size_of())
    {
      ::operator delete(p);
      return;
    }
  memory.free(p);
}
