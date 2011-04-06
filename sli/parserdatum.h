/*
 *  parserdatum.h
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

#ifndef PARSERDATUM_H
#define PARSERDATUM_H

#include "aggregatedatum.h"
#include "aggregatedatum_impl.h"
#include "interpret.h"
#include "parser.h"

class ParserDatum: public AggregateDatum<Parser,&SLIInterpreter::Parsertype>
{
    Datum * clone(void) const
    {
        return new ParserDatum(*this);
    }

    ParserDatum(void);
    
public:
    ParserDatum(Scanner *s):
            AggregateDatum<Parser,&SLIInterpreter::Parsertype>(Parser(s)) {}
    
};

#endif
