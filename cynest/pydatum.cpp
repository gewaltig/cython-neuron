/*
 *  pydatum.cpp
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

#include "pydatum.h"
#include "py3k_compatibility.h"
#include <sstream>


PyObject* PyDatum_FromDatum(Datum &d)
{
  PyDatum *py_datum = PyObject_New(PyDatum, &PyDatumType);
  new(&py_datum->token) Token(d);
  return reinterpret_cast<PyObject*>(py_datum);
}

static void PyDatum_dealloc(PyDatum* self)
{
  self->token.~Token();
 #ifdef IS_PY3K  
  self->ob_base.ob_type->tp_free((PyObject*)self);
#else
  self->ob_type->tp_free((PyObject*)self);
#endif
}

static PyObject* PyDatum_gettype(PyDatum *self, void *)
{
  return PyString_FromString(self->token->gettypename().toString().c_str());
}

static PyObject* PyDatum_str(PyDatum *self)
{
  std::stringstream s;
  self->token->print(s);
  return PyString_FromString(s.str().c_str());
}

static PyObject* PyDatum_repr(PyDatum *self)
{
  std::stringstream s;
  self->token->pprint(s);
  return PyString_FromString(s.str().c_str());
}

static PyGetSetDef PyDatum_getseters[] = {
  {"type", (getter)PyDatum_gettype, NULL, "type", NULL},
  {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};

PyTypeObject PyDatumType = {
    PyVarObject_HEAD_INIT(NULL,0)
  "Datum",                         /* tp_name */
  sizeof(PyDatum),                      /* tp_basicsize */
  0,                                    /* tp_itemsize */
  (destructor)PyDatum_dealloc,          /* tp_dealloc */
  0,                                    /* tp_print */
  0,                                    /* tp_getattr */
  0,                                    /* tp_setattr */
  0,                                    /* tp_compare */
  (reprfunc)PyDatum_repr,               /* tp_repr */
  0,                                    /* tp_as_number */
  0,                                    /* tp_as_sequence */
  0,                                    /* tp_as_mapping */
  0,                                    /* tp_hash  */
  0,                                    /* tp_call */
  (reprfunc)PyDatum_str,                /* tp_str */
  0,                                    /* tp_getattro */
  0,                                    /* tp_setattro */
  0,                                    /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,                   /* tp_flags */
  "Python encapsulation of SLI Datums", /* tp_doc */
  0,                                    /* tp_traverse */
  0,                                    /* tp_clear */
  0,                                    /* tp_richcompare */
  0,                                    /* tp_weaklistoffset */
  0,                                    /* tp_iter */
  0,                                    /* tp_iternext */
  0,                                    /* tp_methods */
  0,                                    /* tp_members */
  PyDatum_getseters,                    /* tp_getset */
  0,                                    /* tp_base */
  0,                                    /* tp_dict */
  0,                                    /* tp_descr_get */
  0,                                    /* tp_descr_set */
  0,                                    /* tp_dictoffset */
  0,                                    /* tp_init */
  0,                                    /* tp_alloc */
  0,                                    /* tp_new */
  0,                                    /* tp_free */
  0,                                    /* tp_is_gc */
  0,                                    /* tp_bases */
  0,                                    /* tp_mro */
  0,                                    /* tp_cache */
  0,                                    /* tp_subclasses */
  0,                                    /* tp_weaklist */
  0,                                    /* tp_del */
#ifdef IS_PY3K
    0,                                  /* tp_version_tag */
#endif
};

