#ifndef TOPOLOGY3MODULE_H
#define TOPOLOGY3MODULE_H

/*
 *  topology3module.h
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

#include "slimodule.h"
#include "position.h"
#include "ntree.h"
#include "exceptions.h"
#include "generic_factory.h"

namespace nest
{
  class Parameter;
  class AbstractMask;
  class AbstractLayer;

  template<int D>
  class Layer;

  class Topology3Module: public SLIModule
  {
  public:

    Topology3Module(Network&);
    ~Topology3Module();

    /**
     * Initialize module by registering models with the network.
     * @param SLIInterpreter* SLI interpreter, must know modeldict
     */
    void init(SLIInterpreter*);

    const std::string name(void) const;
    const std::string commandstring(void) const;

    static SLIType MaskType;         ///< SLI type for masks
    static SLIType ParameterType;    ///< SLI type for parameters

    /*
     * SLI functions: See source file for documentation
     */

    class CreateLayer_DFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } createlayer_Dfunction;

    class GetPosition_iFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } getposition_ifunction;

    class Displacement_a_iFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } displacement_a_ifunction;

    class Distance_a_iFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } distance_a_ifunction;

    class GetGlobalChildren_i_M_aFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } getglobalchildren_i_M_afunction;

    class ConnectLayers_i_i_DFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } connectlayers_i_i_Dfunction;

    class CreateMask_l_DFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } createmask_l_Dfunction;

    class Inside_a_MFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } inside_a_Mfunction;

    class And_M_MFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } and_M_Mfunction;

    class Or_M_MFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } or_M_Mfunction;

    class Sub_M_MFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } sub_M_Mfunction;

    class Mul_P_PFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } mul_P_Pfunction;

    class Div_P_PFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } div_P_Pfunction;

    class Add_P_PFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } add_P_Pfunction;

    class Sub_P_PFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } sub_P_Pfunction;

    class CreateParameter_l_DFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } createparameter_l_Dfunction;

    class GetValue_a_PFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } getvalue_a_Pfunction;

    class DumpLayerNodes_os_iFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } dumplayernodes_os_ifunction;

    class DumpLayerConnections_os_i_lFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } dumplayerconnections_os_i_lfunction;

    class GetElement_i_iaFunction: public SLIFunction
    {
    public:
      void execute(SLIInterpreter *) const;
    } getelement_i_iafunction;

    /**
     * Return a reference to the network managed by the topology module.
     */
    static Network &get_network();

    typedef GenericFactory<AbstractMask> MaskFactory;
    typedef GenericFactory<AbstractMask>::CreatorFunction MaskCreatorFunction;

    template<class T>
    static bool register_mask(const Name & name);
    static bool register_mask(const Name& name, MaskCreatorFunction creator);

    static lockPTRDatum<AbstractMask, &Topology3Module::MaskType> /*MaskDatum*/ create_mask(const Token &);
    static AbstractMask *create_mask(const Name& name, const DictionaryDatum &d);

    typedef GenericFactory<Parameter> ParameterFactory;
    typedef GenericFactory<Parameter>::CreatorFunction ParameterCreatorFunction;

    template<class T>
    static bool register_parameter(const Name & name);
    static bool register_parameter(const Name& name, ParameterCreatorFunction creator);

    static lockPTRDatum<Parameter, &Topology3Module::ParameterType> /*ParameterDatum*/ create_parameter(const Token &);
    static Parameter *create_parameter(const Name& name, const DictionaryDatum &d);

  private:


    /**
     * Return a reference to the mask factory class.
     */
    static MaskFactory &mask_factory_();

    /**
     * Return a reference to the parameter factory class.
     */
    static ParameterFactory &parameter_factory_();

    /**
     * - @c net must be static, so that the execute() members of the
     *   SliFunction classes in the module can access the network.
     */
    static Network* net_;
  };

  /**
   * Exception to be thrown if the wrong argument type
   * is given to a function
   * @ingroup KernelExceptions
   */
  class LayerExpected: public KernelException
  {
  public:
  LayerExpected()
    : KernelException("LayerExpected") {}
    ~LayerExpected() throw () {}

    std::string message();
  };

  inline
  Network &Topology3Module::get_network()
  {
    assert(net_ != 0);
    return *net_;
  }

  template<class T>
  inline
  bool Topology3Module::register_mask(const Name& name)
  {
    return mask_factory_().register_subtype<T>(name);
  }

  inline
  bool Topology3Module::register_mask(const Name& name, MaskCreatorFunction creator)
  {
    return mask_factory_().register_subtype(name, creator);
  }

  inline
  AbstractMask *Topology3Module::create_mask(const Name& name, const DictionaryDatum &d)
  {
    return mask_factory_().create(name,d);
  }

  template<class T>
  inline
  bool Topology3Module::register_parameter(const Name& name)
  {
    return parameter_factory_().register_subtype<T>(name);
  }

  inline
  bool Topology3Module::register_parameter(const Name& name, ParameterCreatorFunction creator)
  {
    return parameter_factory_().register_subtype(name, creator);
  }


} // namespace nest

#endif
