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

#ifndef PYOBJECTDATUM_H
#define PYOBJECTDATUM_H

#include <Python.h>
#include "datum.h"

#include <string>
#include "dataconverter.h"


// prefixed all references to members of GenericDatum with this->,
// since HP's aCC otherwise complains about them not being declared
// according to ISO Standard Sec. 14.6.2(3) [temp.dep]
// HEP, 2001-08-08



class PyObjectDatum: public Datum
{

private:
  PyObject* pyObj;
  
  double* currents;
  double* in_spikes;
  double* ex_spikes;
  long* t_lag;
  long* spike;
  
  int forbiddenParamsLength;
  std::string forbiddenParams[16];

  PyObjectDatum *clone(void) const
  {
        return new PyObjectDatum(*this);
  }

  PyObjectDatum * get_ptr()
  {
    return clone();
  }
public:
  PyObjectDatum(PyObject *py) {
	  pyObj = py;
	  this->currents = NULL;
	  this->in_spikes = NULL;
	  this->ex_spikes = NULL;
	  this->t_lag = NULL;
	  this->spike = NULL;
	  Py_XINCREF(pyObj);
	  
	  forbiddenParamsLength = 16;
	  forbiddenParams[0] = "archiver_length";
	  forbiddenParams[1] = "frozen";
	  forbiddenParams[2] = "global_id";
	  forbiddenParams[3] = "local";
	  forbiddenParams[4] = "local_id";
	  forbiddenParams[5] = "model";
	  forbiddenParams[6] = "node_type";
	  forbiddenParams[7] = "parent";
	  forbiddenParams[8] = "pyobject";
	  forbiddenParams[9] = "recordables";
	  forbiddenParams[10] = "state";
	  forbiddenParams[11] = "t_spike";
	  forbiddenParams[12] = "tau_minus";
	  forbiddenParams[13] = "tau_minus_triplet";
	  forbiddenParams[14] = "thread";
	  forbiddenParams[15] = "vp";
  }

  PyObjectDatum(PyObjectDatum* py) {
	  this->pyObj = py->pyObj;
	  Py_XINCREF(pyObj);
	  // todo
  }
  ~PyObjectDatum() {}




void putStdParams(double* curr, double* is, double* es, long* tl, long* sp) {
	this->currents = curr;
	this->in_spikes = is;
	this->ex_spikes = es;
	this->t_lag = tl;
	this->spike = sp;
}

void call_method(std::string cmd) {
	PyGILState_STATE s = PyGILState_Ensure();

	PyObject_CallMethod(this->pyObj, cmd.c_str(), NULL);

    PyGILState_Release(s);
}

void call_update() {
	PyGILState_STATE s = PyGILState_Ensure();

	PyObject_CallMethod(this->pyObj, "setStdParams", "dddl", *currents, *in_spikes, *ex_spikes, *t_lag);
	PyObject_CallMethod(this->pyObj, "update", NULL);
	*spike = PyInt_AsLong(PyObject_CallMethod(this->pyObj, "getSpike", NULL));

    PyGILState_Release(s);
}



bool isOK(std::string method)
{	
	for(int i = 0; i < forbiddenParamsLength; i++) {
		if(method.compare(forbiddenParams[i]) == 0) {
			return false;
		}
	}
	
	return true;
}

void call_status_method(std::string cmd, void* status_) {
	PyGILState_STATE s = PyGILState_Ensure();
	DictionaryDatum* status = static_cast<DictionaryDatum*>(status_);
	
	if(cmd.compare("setStatus") == 0) {
		PyObject* dict = PyDict_New();

		for(Dictionary::iterator it = (*status)->begin(); it != (*status)->end(); ++it) {
			if(isOK(it->first.toString()) == true) {
				PyObject_SetItem(dict, PyString_FromString(it->first.toString().c_str()), dataConverter.datumToObject( (**status)[it->first.toString()].datum()));
			}
		}
		
		PyObject_CallMethodObjArgs(this->pyObj, PyString_FromString("setStatus"), dict, NULL);
	}
	else if(cmd.compare("getStatus") == 0) {
		PyObject* dict = PyObject_CallMethod(this->pyObj, "getStatus", NULL);
		PyObject* subPyObj=0;
		PyObject *key=0;
		Py_ssize_t pos = 0;

		while (PyDict_Next(dict, &pos, &key, &subPyObj)) 
		{
			(**status)[PyString_AsString(key)] = dataConverter.objectToDatum(subPyObj);
		}
		
		if(currents != NULL && in_spikes != NULL && ex_spikes != NULL && t_lag != NULL && spike != NULL) {
			(**status)["currents"] = *currents;
			(**status)["in_spikes"] = *in_spikes;
			(**status)["ex_spikes"] = *ex_spikes;
			(**status)["t_lag"] = *t_lag;
			(**status)["spike"] = *spike;
		}
	}
	PyGILState_Release(s);
}

  /**
   * Accept a DatumVisitor as a visitor to the datum (visitor pattern).
   * This member has to be overridden in the derived classes
   * to call visit and passing themselves as an argument.
   */
  void use_converter(DatumConverter &v) { }
  
  void  print(std::ostream & ) const {printf("This is a PyObjectDatum");}
  void  pprint(std::ostream &) const {printf("This is a PyObjectDatum");}

  void  list(std::ostream &o, std::string prefix, int l) const
  {
    if(l==0)
      prefix="-->"+prefix;
    else
      prefix="   "+prefix;
    o << prefix;
    print(o);
  }

  void  input_form(std::ostream &o) const
  {
    pprint(o);
  }
  
  bool  equals(const Datum *d) const
  {
    return this == d;
  }
  
  void  info(std::ostream &) const{printf("PyObjectDatum");}
};



#endif
