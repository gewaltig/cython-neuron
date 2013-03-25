#ifndef __PYX_HAVE__cython_neuron
#define __PYX_HAVE__cython_neuron


#ifndef __PYX_HAVE_API__cython_neuron

#ifndef __PYX_EXTERN_C
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

__PYX_EXTERN_C DL_IMPORT(int) createNeuron(void);
__PYX_EXTERN_C DL_IMPORT(void) setNeuronParam(int, char *, double);
__PYX_EXTERN_C DL_IMPORT(double) getNeuronParam(int, char *);
__PYX_EXTERN_C DL_IMPORT(void) initIteratorNeuronParams(int);
__PYX_EXTERN_C DL_IMPORT(int) hasNeuronNextParam(int);
__PYX_EXTERN_C DL_IMPORT(void) update(int);
__PYX_EXTERN_C DL_IMPORT(void) calibrate(int);

#endif /* !__PYX_HAVE_API__cython_neuron */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initcython_neuron(void);
#else
PyMODINIT_FUNC PyInit_cython_neuron(void);
#endif

#endif /* !__PYX_HAVE__cython_neuron */
