/*
 *  pydatum.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef PYTHONDATUM_H
#define PYTHONDATUM_H

#include <Python.h>

#include "token.h"
#include "datum.h"

/**
 * Python class for encapsulating generic Datums which can not be converted
 * to a native python type.
 */
struct PyDatum
{
    PyObject_HEAD
    Token token;
};

extern PyTypeObject PyDatumType;

/**
 * Create a new PyDatum object initialized with the given Datum.
 * @returns new reference.
 */
extern PyObject* PyDatum_FromDatum(Datum &d);

/**
 * Get the pointer to the Datum contained in this PyDatum.
 */
inline
Datum* PyDatum_GetDatum(PyDatum *pyd)
{
  return pyd->token.datum();
}

/**
 * Check if the object is a PyDatum.
 * @returns true if the object is a PyDatum.
 */
inline
bool PyDatum_Check(PyObject *pObj)
{
  return PyObject_IsInstance(pObj,reinterpret_cast<PyObject*>(&PyDatumType));
}

#endif
