/*
 *  dictdatum.h
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

#ifndef DICTDATUM_H
#define DICTDATUM_H
/* 
    Defines class DictionaryDatum
*/
#include "interpret.h"
#include "dict.h"
#include "lockptrdatum.h"

typedef lockPTRDatum<Dictionary,&SLIInterpreter::Dictionarytype> DictionaryDatum;
#endif
