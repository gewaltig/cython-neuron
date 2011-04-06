#ifndef RANDOM_NUMBERS_H
#define RANDOM_NUMBERS_H

/*
    Copyright (C) 2002-2008 The NEST Initiative 
                
    This file is part of NEST.
*/

#include "slitype.h"
#include "slimodule.h" 
//#include "dictdatum.h"

#include <string>

#ifndef DICT_H
class Dictionary;
#endif

#ifndef DICTDATUM_H
class DictionaryDatum;
#endif

/**
 *  Provides random numbers and deviates to SLI.
 */    
class RandomNumbers: public SLIModule
{
  public:
  
  static SLIType RngType;         // actual RNG
  static SLIType RngFactoryType;  // random generator factory

  static SLIType RdvType;         // random deviate generator 
  static SLIType RdvFactoryType;  // random deviate generator factory 
  

  RandomNumbers(){};
  ~RandomNumbers();

  const std::string name(void) const
    {
      return "RandomNumbers";
    }

   const std::string commandstring(void) const
    {
      return std::string("/librandom /C++ ($Revision: $) provide-component "
                         "/librandom /SLI (6796) require-component");
    }

  /**
   * Initializes the random number module.  
   * The random number generator
   * and the random deviate generator dictionaries are set up.
   */
 void init(SLIInterpreter *);

  // RNG creation function
  class CreateRNGFunction: public SLIFunction
  {
    public: 
    void execute(SLIInterpreter *) const;
  };

  // RNG creation function
  class CreateRDVFunction: public SLIFunction
  {
    public: 
    void execute(SLIInterpreter *) const;
  };

  class SetStatus_vdFunction: public SLIFunction
  {
  public:
    void execute(SLIInterpreter *) const;
  };

  class GetStatus_vFunction: public SLIFunction
  {
  public:
    void execute(SLIInterpreter *) const;
  };

  // RNG access functions
  class IrandFunction: public SLIFunction  
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class DrandFunction: public SLIFunction  
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class SeedFunction: public SLIFunction  
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class RandomArrayFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class RandomFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };


  // create function
  CreateRNGFunction createrngfunction;
  CreateRDVFunction createrdvfunction;

  // set/get functions
  SetStatus_vdFunction setstatus_vdfunction;
  GetStatus_vFunction  getstatus_vfunction;

  // access functions
  SeedFunction seedfunction;
  IrandFunction irandfunction;
  DrandFunction drandfunction;

  RandomArrayFunction randomarrayfunction;
  RandomFunction randomfunction;

private:
  
  //! Utility function for registering number generators
  template <typename NumberGenerator>
  void register_rng_(const std::string& name, DictionaryDatum& dict);

  //! Utility function for registering deviate generators
  template <typename DeviateGenerator>
  void register_rdv_(const std::string& name, DictionaryDatum& dict);

};

#endif
