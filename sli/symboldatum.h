/*
 *  symboldatum.h
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

#ifndef SYMBOLDATUM_H
#define SYMBOLDATUM_H

// Include all headers, needed to use token and datum objects
#include <typeinfo>

// <string> MUST be included before any STL header, since there are
// some conflicts between this and the g++-2.7.2 STL version

#include <string>
#include "name.h"
#include "aggregatedatum.h"
#include "interpret.h"

/* These are declarations to specialize the static memory pool BEFORE
   we instantiate the AggregateDatum. Note, that this is onlz a declaration, 
   because we do not provide an initializer (see ISO14882 Sec.  14.7.3.15.)
   The definition is given in the *.CC file with the appropriate 
   initializer.

   Note that SUN's Forte 6.2 does not handle this correctly, so we have
   to use a compiler-switch.
*/
#ifndef HAVE_STATIC_TEMPLATE_DECLARATION_FAILS
template<> 
sli::pool AggregateDatum<Name,&SLIInterpreter::Symboltype>::memory;
#endif

class SymbolDatum: public AggregateDatum<Name,&SLIInterpreter::Symboltype>
{
    Datum * clone(void) const
    {
        return new SymbolDatum(*this);
    }
    
public:
    SymbolDatum(const Name &n):
            AggregateDatum<Name,&SLIInterpreter::Symboltype>(n) {}
    SymbolDatum(const SymbolDatum &n):
            AggregateDatum<Name,&SLIInterpreter::Symboltype>(n) {}
    ~SymbolDatum(){}
    
};

#endif

