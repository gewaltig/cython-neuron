#ifndef MATHINTERFACE_H
#define MATHINTERFACE_H
#include "slitype.h"
#include "slimodule.h" 
#include <string>

class MLInterface: public SLIModule
{
  public:
  
  MLInterface(){}

  const std::string name(void) const
      {
	  return "MathLink";
      }
  
  void init(SLIInterpreter *);
    
  class MathLinkOpenFunction: public SLIFunction  
      {
      public:
	  MathLinkOpenFunction() {}
	  void execute(SLIInterpreter *) const;
      };
  
  class MathLinkCloseFunction: public SLIFunction
      {
      public:
	  MathLinkCloseFunction() {}
	  void execute(SLIInterpreter *) const;
      };

  class MathLinkFlushFunction: public SLIFunction
      {
      public:
	  MathLinkFlushFunction() {}
	  void execute(SLIInterpreter *) const;
      };
  
  class MathLinkGetStringFunction: public SLIFunction  
      {
      public:
	  MathLinkGetStringFunction() {}
	  void execute(SLIInterpreter *) const;
      };
  
  class MathLinkPutStringFunction: public SLIFunction
      {
      public:
	  MathLinkPutStringFunction() {}
	  void execute(SLIInterpreter *) const;
      };

  const MathLinkOpenFunction       mathlinkopenfunction;
  const MathLinkCloseFunction      mathlinkclosefunction;
  const MathLinkFlushFunction      mathlinkflushfunction;
  const MathLinkPutStringFunction  mathlinkputstringfunction;
  const MathLinkGetStringFunction  mathlinkgetstringfunction;
  
};
#endif







