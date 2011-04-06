/*
 *  triedatum.cc
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

#include "triedatum.h"
#include "interpret.h"

sli::pool TrieDatum::memory(sizeof(TrieDatum),1024,1);

bool TrieDatum::equals(Datum const *dat) const
{
        // The following construct works around the problem, that
        // a direct dynamic_cast<const GenericDatum<D,slt> * > does not seem
        // to work.
    
    const TrieDatum
        *fd=dynamic_cast<TrieDatum * >(const_cast< Datum *>(dat));
    
    return  (fd == NULL)? false : (tree == fd->tree);
    
}
