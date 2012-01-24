/*
 *  numericdatum.h
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

#ifndef NUMERICDATUM_H
#define NUMERICDATUM_H
/* 
    Datum template for numeric data types
*/

#include "genericdatum.h"
#include "allocator.h"



// prefixed all references to members of GenericDatum with this->,
// since HP's aCC otherwise complains about them not being declared
// according to ISO Standard Sec. 14.6.2(3) [temp.dep]
// HEP, 2001-08-08

template<class D, SLIType *slt>
class NumericDatum: public GenericDatum<D,slt>
{
 protected:
  static sli::pool memory;
  using GenericDatum<D,slt>::d;

 private:
  Datum *clone(void) const
    {
      return new NumericDatum<D,slt>(*this);    
    }

public:
    
  NumericDatum() { d = (D) 0;}
  NumericDatum(const D& d_s) {d=d_s;}
    virtual ~NumericDatum() {}
    
    operator D() const
    {return d;}

    operator D& () 
    {return d;}

  void  input_form(std::ostream &) const;
  void  pprint(std::ostream &) const;


  static void * operator new(size_t size)
    {
      if(size != memory.size_of())
	return ::operator new(size);
      return memory.alloc();
    }

  static void operator delete(void *p, size_t size)
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


  /**
   * Accept a DatumVisitor as a visitor to the datum (visitor pattern).
   * This member has to be overridden in the derived classes
   * to call visit and passing themselves as an argument.
   */
  void use_converter(DatumConverter &);

};



#endif
