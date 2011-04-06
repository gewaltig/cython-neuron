/*
 *  booldatum.h
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

#ifndef BOOLDATUM_H
#define BOOLDATUM_H

#include <string>
#include "genericdatum.h"
#include "allocator.h"
#include "interpret.h"


class Name;
class Token;

class BoolDatum: public GenericDatum<bool,&SLIInterpreter::Booltype>
{
 protected:
  static sli::pool memory;

 private:
  Datum *clone(void) const
    {
        return new BoolDatum(*this);    
    }
    
public:
  static const char* true_string;
  static const char* false_string;

  BoolDatum(): GenericDatum<bool,&SLIInterpreter::Booltype>(){}
  BoolDatum(const BoolDatum& val):GenericDatum<bool,&SLIInterpreter::Booltype>(val){}
  BoolDatum(bool val):GenericDatum<bool,&SLIInterpreter::Booltype>(val){}
  BoolDatum(const Name&);

  operator bool() const
    { return d; }

  operator Name() const;

  operator std::string() const;

  void  input_form(std::ostream &) const;
  void  print(std::ostream &) const;
  void  pprint(std::ostream &) const;


  static void * operator new(size_t size);

  static void operator delete(void *p, size_t size);

  /**
   * Accept a DatumConverter as a visitor to this datum.
   * A visitor may be used to make a conversion to a type, which is not known to NEST.
   * (visitor pattern).
   */
  void use_converter(DatumConverter &);

};

#endif
