/*
 *  genericdatum.h
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

#ifndef GENERICDATUM_H
#define GENERICDATUM_H
/* 
    Datum template for generic C/C++ data types
*/
#include "datum.h"

/***********************************************************/
/* Concrete  Generic Data Objects                          */
/***********************************************************/

template<class D, SLIType *slt>
class GenericDatum: public TypedDatum<slt>
{
    
    virtual Datum * clone(void) const
    {
        return new GenericDatum<D,slt>(*this);
    }

protected:
    D d;
public:
    GenericDatum() {}
    virtual ~GenericDatum() {}
    
    GenericDatum(const D& d_s): d(d_s) {}
    GenericDatum(const GenericDatum<D,slt>& gd): TypedDatum<slt>(gd), d(gd.d) {}
    
    const D& operator=(const D &d_s)
    {
        d = d_s;
        return d;
    }

    D& get(void)
    {
        return d;
    }

    void print(std::ostream& o) const
    {
        o << d;
    }

    void pprint(std::ostream& o) const
    {
        o << d;
    }

    void info(std::ostream &out) const
    {
        out << "GenericDatum<D,slt>::info\n";
        out << "d = " << d << std::endl;
    }

    bool equals(const Datum *dat) const
    {
        // The following construct works around the problem, that
        // a direct dynamic_cast<const GenericDatum<D,slt> * > does not seem
        // to work.
    
        const GenericDatum<D,slt>
            *ddc=dynamic_cast<GenericDatum<D,slt> * >(const_cast< Datum *>(dat));

//    std::cerr << "d = " << d << " ddc = " << ddc << " dat = " << dat << std::endl;
    
        if(ddc == NULL)
            return false;
    
        return d == ddc->d;
    }

};


/******************************************/

#endif




