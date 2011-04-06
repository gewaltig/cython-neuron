
#ifndef NEST_STARTUP_H
#define NEST_STARTUP_H

namespace nest {
class Network;
}

class SLIInterpreter;


int pyneststartup(int argc, char**argv, SLIInterpreter &engine, nest::Network* &pNet);
int neststartup(int argc, char**argv, SLIInterpreter &engine, nest::Network* &pNet);

#endif
