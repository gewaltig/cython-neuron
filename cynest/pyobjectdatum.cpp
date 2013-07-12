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

#include "pyobjectdatum.h"
//#include "dataconverter.h"
//#include "dictdatum.h"


  
//void PyObjectDatum::call_status_method(std::string cmd, void* status) {
	/*PyGILState_STATE s = PyGILState_Ensure();
	DictionaryDatum* status_ = (DictionaryDatum*)status;
	PyObject* dict = PyObject_GetAttrString(pyObj, "params");
	
	if(cmd.equals("setStatus")) {
		
		for(Dictionary::iterator it = (*status)->begin(); it != (*status)->end(); ++it) {
			if(!it->first.toString().equals("pyobject")) {
				//PyObject_SetItem(dict, PyString_FromString(it->first.toString().c_str()), dataConverter.datumToObject((**status)[it->first]));
			}
		}
	}
	else if(cmd.equals("getStatus")) {
		PyObject* subPyObj=0;
		PyObject *key=0;
		Py_ssize_t pos = 0;

		while (PyDict_Next(dict, &pos, &key, &subPyObj)) 
		{
			// (**status)[PyString_AsString(key)] = dataConverter.objectToDatum(subPyObj);
		}
	}
	PyGILState_Release(s);*/
//}


