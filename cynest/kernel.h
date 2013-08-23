#ifndef __PYX_HAVE__cynest__kernel
#define __PYX_HAVE__cynest__kernel

struct PyToken;

/* "cynest/kernel.pyx":16
 * cimport classes
 * 
 * cdef public class PyToken[object PyToken, type PyTokenType]:             # <<<<<<<<<<<<<<
 * #cdef class PyToken:
 *      """
 */
struct PyToken {
  PyObject_HEAD
  Token *thisptr;
};

#ifndef __PYX_HAVE_API__cynest__kernel

#ifndef __PYX_EXTERN_C
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

#endif /* !__PYX_HAVE_API__cynest__kernel */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initkernel(void);
#else
PyMODINIT_FUNC PyInit_kernel(void);
#endif

#endif /* !__PYX_HAVE__cynest__kernel */
