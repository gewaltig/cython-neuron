#ifndef PYNESTKERNEL2
#define PYNESTKERNEL2
#include <Python.h>

/**
 * C++ API to NEST. 
 * The NEST interpreter and the network are encapsulated in a class which we expose to Cynest.
 *
 */

class SLIInterpreter;
class Datum;
namespace nest
{
  class Network;
}

class NESTEngine
{
 public:
  NESTEngine();
  ~NESTEngine();
  

  bool init(std::vector<std::string> argv,  std::string modulepath);  //!< Initialize NEST. This is part of the Python API
  bool push(PyObject *args);
  bool push_connections(PyObject *args);
  bool run( std::string cmd);

  PyObject *pop();

  Datum* get_Datum(PyObject *pObj);

  bool check_engine();

 private:

  /**
   * Helper function to initialize numpy.
   * This is a wrapper around a numpy macro which apparently contains a return statement.
   */
  void init_numpy();

  bool initialized_;
  PyObject *NESTError_;
  SLIInterpreter *pEngine_ ;
  nest::Network *pNet_;
};

#endif
