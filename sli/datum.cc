/*
 *  datum.cc
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

#include "token.h"

#include "doubledatum.h"
#include "integerdatum.h"
#include "dictdatum.h"
#include "dictutils.h"
#include "arraydatum.h"
#include "booldatum.h"
#include "stringdatum.h"
#include "datumconverter.h"
#include "datum.h"

void Datum::setwriteable(bool f)
{
    wflag = f;
}


void Datum::info(std::ostream &out) const
{
    out << "wflag = " << wflag << std::endl;
}

void Datum::use_converter(DatumConverter &converter)
{
  converter.convert_me(*this);
}
