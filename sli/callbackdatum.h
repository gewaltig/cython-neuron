/*
 *  callbackdatum.h
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

#ifndef CALLBACKDATUM_H
#define CALLBACKDATUM_H
/* 
    Datum class, implementing SLI callbacks.
*/

#include "genericdatum.h"
#include "interpret.h"

typedef GenericDatum<Token,&SLIInterpreter::Callbacktype> CallbackDatum;

#endif
