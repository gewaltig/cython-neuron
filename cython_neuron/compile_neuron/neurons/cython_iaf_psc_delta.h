#ifndef __PYX_HAVE__cython_iaf_psc_delta
#define __PYX_HAVE__cython_iaf_psc_delta


#ifndef __PYX_HAVE_API__cython_iaf_psc_delta

#ifndef __PYX_EXTERN_C
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

__PYX_EXTERN_C DL_IMPORT(void) putSpecialFunctions(double (*)(int, long, double), long (*)(int, int, long, double), unsigned int (*)(int, unsigned int));
__PYX_EXTERN_C DL_IMPORT(int) createNeuron(void);
__PYX_EXTERN_C DL_IMPORT(void) setNeuronParams(int, PyObject *);
__PYX_EXTERN_C DL_IMPORT(PyObject) *getNeuronParams(int);
__PYX_EXTERN_C DL_IMPORT(void) setStdVars(int, long, double, double, double, long);
__PYX_EXTERN_C DL_IMPORT(void) getStdVars(int, long *, double *, double *, double *, long *);
__PYX_EXTERN_C DL_IMPORT(void) update(int);
__PYX_EXTERN_C DL_IMPORT(void) calibrate(int);
__PYX_EXTERN_C DL_IMPORT(void) setStatus(int);
__PYX_EXTERN_C DL_IMPORT(void) getStatus(int);

#endif /* !__PYX_HAVE_API__cython_iaf_psc_delta */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initcython_iaf_psc_delta(void);
#else
PyMODINIT_FUNC PyInit_cython_iaf_psc_delta(void);
#endif

#endif /* !__PYX_HAVE__cython_iaf_psc_delta */
