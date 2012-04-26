/*
 *  topology3module.cpp
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

#include "config.h"
#include "arraydatum.h"
#include "integerdatum.h"
#include "network.h"
#include "model.h"
#include "genericmodel.h"
#include "communicator.h"
#include "communicator_impl.h"
#include "topology3module.h"
#include "layer.h"
#include "free_layer.h"
#include "mask.h"
#include "lockptrdatum_impl.h"
#include "dictdatum.h"
#include "booldatum.h"

namespace nest
{
  SLIType Topology3Module::MaskType;

  index Topology3Module::cached_layer_;
  AbstractNtree<index> * Topology3Module::cached_positions_;
  Network *Topology3Module::net_;

  Topology3Module::Topology3Module(Network& net)
  {
    net_ = &net;
    MaskType.settypename("masktype");
    MaskType.setdefaultaction(SLIInterpreter::datatypefunction);
  }

  Topology3Module::~Topology3Module()
  {
  }

  const std::string Topology3Module::name(void) const
  {
    return std::string("Topology3Module"); // Return name of the module
  }

  const std::string Topology3Module::commandstring(void) const
  {
    return
      std::string("/topology /C++ ($Revision: 7862 $) provide-component "
		  "/topology-interface /SLI (6203) require-component ");
  }

  GenericFactory<AbstractMask> &Topology3Module::mask_factory(void)
  {
    static GenericFactory<AbstractMask> factory;
    return factory;
  }

  static AbstractMask* create_doughnut(const DictionaryDatum& d) {
      Position<2> center(0,0);
      if (d->known(Name("anchor")))
        center = getValue<std::vector<double_t> >(d, Name("anchor"));

      BallMask<2> outer_circle(center,getValue<double_t>(d, "outer_radius"));
      BallMask<2> inner_circle(center,getValue<double_t>(d, "inner_radius"));

      return new MinusMask<2>(outer_circle, inner_circle);
  }

  void Topology3Module::init(SLIInterpreter *i)
  {
    // Register the topology functions as SLI commands.

    i->createcommand("CreateLayer_D",
		     &createlayer_Dfunction);

    i->createcommand("GetPosition_i",
		     &getposition_ifunction);

    i->createcommand("Displacement_a_i",
		     &displacement_a_ifunction);

    i->createcommand("Distance_a_i",
		     &distance_a_ifunction);

    i->createcommand("CreateMask_l_D",
		     &createmask_l_Dfunction);

    i->createcommand("Inside_M_a",
		     &inside_M_afunction);

    i->createcommand("and_M_M",
		     &and_M_Mfunction);

    i->createcommand("or_M_M",
		     &or_M_Mfunction);

    i->createcommand("sub_M_M",
		     &sub_M_Mfunction);

    i->createcommand("GetGlobalChildren_i_M_a",
		     &getglobalchildren_i_M_afunction);

    // Register layer types as models
    Network & net = get_network();

    register_model<FreeLayer<2> >(net, "topology_layer_free");
    register_model<FreeLayer<3> >(net, "topology_layer_3d");

    // Register mask types
    mask_factory().register_subtype<BallMask<2> >("circular");
    mask_factory().register_subtype<BallMask<3> >("spherical");
    mask_factory().register_subtype<BoxMask<2> >("rectangular");
    mask_factory().register_subtype<BoxMask<3> >("box");
    mask_factory().register_subtype<BoxMask<3> >("volume");  // For compatibility with topo 2.0
    mask_factory().register_subtype("doughnut",create_doughnut);

  }

  /*BeginDocumentation

    Name: CreateLayer - create a spatial layer of nodes

    Synopsis:
    dict CreateLayer -> layer

    Parameters:
    dict - dictionary with layer specification

    Description: The Topology module organizes neuronal networks in
    layers. A layer is a special type of subnet which contains information
    about the spatial position of its nodes. There are three classes of
    layers: grid-based layers, in which each element is placed at a
    location in a regular grid; free layers, in which elements can be
    placed arbitrarily in space; and random layers, where the elements are
    distributed randomly throughout a region in space.  Which kind of layer
    this command creates depends on the elements in the supplied
    specification dictionary.

    Author: Håkon Enger, Kittel Austvoll
  */
  void Topology3Module::CreateLayer_DFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    DictionaryDatum layer_dict =
      getValue<DictionaryDatum>(i->OStack.pick(0));

    index layernode =  AbstractLayer::create_layer(layer_dict);

    i->OStack.pop(1);
    i->OStack.push(layernode);
    i->EStack.pop();
  }

  /*
    BeginDocumentation

    Name: topology::GetPosition - retrieve position of input node

    Synopsis: node_gid GetPosition -> [array]

    Parameters:
    node_gid      - gid of layer node
    [array]       - spatial position of node [x y]

    Description: Retrieves spatial 2D position of layer node.

    Examples:

    topology using

    %%Create layer
    << /rows 5
       /columns 4
       /elements /iaf_neuron
    >> /dictionary Set

    dictionary CreateLayer /src Set

    4 GetPosition

    Author: Kittel Austvoll
  */

  void Topology3Module::GetPosition_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    Network & net = get_network();

    index node_gid = getValue<long_t>(i->OStack.pick(0));
    if ( not net.is_local_gid(node_gid) )
      throw KernelException("GetPosition is currently implemented for local nodes only.");

    Node const * const node = net.get_node(node_gid);

    AbstractLayer * const layer = dynamic_cast<AbstractLayer*>(node->get_parent());
    if ( !layer )
      throw LayerExpected();

    Token result = layer->get_position_vector(node->get_lid());

    i->OStack.pop(1);
    i->OStack.push(result);
    i->EStack.pop();

  }

  /*
    BeginDocumentation

    Name: topology::Displacement - compute displacement vector

    Synopsis: from_gid to_gid Displacement -> [double vector]
              from_pos to_gid Displacement -> [double vector]

    Parameters:
    from_gid    - int, gid of node in a topology layer
    from_pos    - double vector, position in layer
    to_gid      - int, gid of node in a topology layer

    Returns:
    [double vector] - vector pointing from position "from" to position "to"

    Description:
    This function returns a vector connecting the position of the "from_gid"
    node or the explicitly given "from_pos" position and the position of the
    "to_gid" node. Nodes must be parts of topology layers.

    The "from" position is projected into the layer of the "to_gid" node. If
    this layer has periodic boundary conditions (EdgeWrap is true), then the
    shortest displacement vector is returned, taking into account the
    periodicity. Fixed grid layers are in this case extended so that the
    nodes at the edges of the layer have a distance of one grid unit when
    wrapped.

    Example:

    topology using
    << /rows 5
       /columns 4
       /elements /iaf_neuron
    >> CreateLayer ;

    4 5         Displacement
    [0.2 0.3] 5 Displacement

    Author: Håkon Enger, Hans E Plesser, Kittel Austvoll

    See also: Distance, GetPosition
  */
  void Topology3Module::Displacement_a_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    Network & net = get_network();

    std::vector<double_t> point = getValue<std::vector<double_t> >(i->OStack.pick(1));

    index node_gid = getValue<long_t>(i->OStack.pick(0));
    if ( not net.is_local_gid(node_gid) )
      throw KernelException("Displacement is currently implemented for local nodes only.");

    Node const * const node = net.get_node(node_gid);

    AbstractLayer * const layer = dynamic_cast<AbstractLayer*>(node->get_parent());
    if ( !layer )
      throw LayerExpected();

    Token result = layer->compute_displacement(point, node->get_lid());

    i->OStack.pop(2);
    i->OStack.push(result);
    i->EStack.pop();

  }

  /*
    BeginDocumentation

    Name: topology::Distance - compute distance between nodes

    Synopsis: from_gid to_gid Distance -> double
              from_pos to_gid Distance -> double

    Parameters:
    from_gid    - int, gid of node in a topology layer
    from_pos    - double vector, position in layer
    to_gid      - int, gid of node in a topology layer

    Returns:
    double - distance between nodes or given position and node

    Description:
    This function returns the distance between the position of the "from_gid"
    node or the explicitly given "from_pos" position and the position of the
    "to_gid" node. Nodes must be parts of topology layers.

    The "from" position is projected into the layer of the "to_gid" node. If
    this layer has periodic boundary conditions (EdgeWrap is true), then the
    shortest distance is returned, taking into account the
    periodicity. Fixed grid layers are in this case extended so that the
    nodes at the edges of the layer have a distance of one grid unit when
    wrapped.

    Example:

    topology using
    << /rows 5
       /columns 4
       /elements /iaf_neuron
    >> CreateLayer ;

    4 5         Distance
    [0.2 0.3] 5 Distance

    Author: Hans E Plesser, Kittel Austvoll

    See also: Displacement, GetPosition
  */
  void Topology3Module::Distance_a_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    Network & net = get_network();

    std::vector<double_t> point = getValue<std::vector<double_t> >(i->OStack.pick(1));

    index node_gid = getValue<long_t>(i->OStack.pick(0));
    if ( not net.is_local_gid(node_gid) )
      throw KernelException("Displacement is currently implemented for local nodes only.");

    Node const * const node = net.get_node(node_gid);

    AbstractLayer * const layer = dynamic_cast<AbstractLayer*>(node->get_parent());
    if ( !layer )
      throw LayerExpected();

    Token result = layer->compute_distance(point, node->get_lid());

    i->OStack.pop(2);
    i->OStack.push(result);
    i->EStack.pop();

  }

  /*BeginDocumentation

    Name: CreateMask - create a spatial mask

    Synopsis:
    /type dict CreateMask -> mask

    Parameters:
    /type - mask type
    dict  - dictionary with mask specifications

    Description: Masks are used when creating connections in the Topology
    module. A mask describes which area of the pool layer shall be searched
    for nodes to connect for any given node in the driver layer. This
    command creates a mask object which may be combined with other mask
    objects using Boolean operators. The mask is specified in a dictionary.

    Author: Håkon Enger
  */
  void Topology3Module::CreateMask_l_DFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    const Name masktype = getValue<Name>(i->OStack.pick(1));    
    DictionaryDatum mask_dict =
      getValue<DictionaryDatum>(i->OStack.pick(0));

    
    MaskDatum datum( mask_factory().create(masktype,mask_dict) );

    i->OStack.pop(2);
    i->OStack.push(datum);
    i->EStack.pop();
  }

  /*BeginDocumentation

    Name: Inside - test if a point is inside a mask

    Synopsis:
    mask list Inside -> bool

    Parameters:
    mask - mask object
    list - point coordinates

    Returns:
    bool - true if the point is inside the mask
  */
  void Topology3Module::Inside_M_aFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    MaskDatum mask = getValue<MaskDatum>(i->OStack.pick(1));
    std::vector<double_t> point = getValue<std::vector<double_t> >(i->OStack.pick(0));

    bool ret = mask->inside(point);

    i->OStack.pop(2);
    i->OStack.push(Token(BoolDatum(ret)));
    i->EStack.pop();
  }

  void Topology3Module::And_M_MFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    MaskDatum mask1 = getValue<MaskDatum>(i->OStack.pick(1));
    MaskDatum mask2 = getValue<MaskDatum>(i->OStack.pick(0));

    MaskDatum newmask = mask1->intersect_mask(*mask2);

    i->OStack.pop(2);
    i->OStack.push(newmask);
    i->EStack.pop();
  }

  void Topology3Module::Or_M_MFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    MaskDatum mask1 = getValue<MaskDatum>(i->OStack.pick(1));
    MaskDatum mask2 = getValue<MaskDatum>(i->OStack.pick(0));

    MaskDatum newmask = mask1->union_mask(*mask2);

    i->OStack.pop(2);
    i->OStack.push(newmask);
    i->EStack.pop();
  }

  void Topology3Module::Sub_M_MFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    MaskDatum mask1 = getValue<MaskDatum>(i->OStack.pick(1));
    MaskDatum mask2 = getValue<MaskDatum>(i->OStack.pick(0));

    MaskDatum newmask = mask1->minus_mask(*mask2);

    i->OStack.pop(2);
    i->OStack.push(newmask);
    i->EStack.pop();
  }

  AbstractNtree<index> * Topology3Module::get_global_positions(AbstractLayer *layer)
  {
    if (cached_layer_ == layer->get_gid()) {
      return cached_positions_;
    }

    if (cached_positions_ != 0) {
      delete cached_positions_;
      cached_positions_ = 0;
      cached_layer_ = -1;
    }

    cached_positions_ = layer->get_global_positions();

    cached_layer_ = layer->get_gid();
    return cached_positions_;
  }


  void Topology3Module::GetGlobalChildren_i_M_aFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);

    index gid = getValue<long_t>(i->OStack.pick(2));
    MaskDatum maskd = getValue<MaskDatum>(i->OStack.pick(1));
    std::vector<double_t> anchor = getValue<std::vector<double_t> >(i->OStack.pick(0));

    AbstractMask &mask = *maskd;
    AbstractLayer *layer = dynamic_cast<AbstractLayer *>(get_network().get_node(gid));
    if (layer == NULL)
      throw LayerExpected();

    AbstractNtree<index> *tree = get_global_positions(layer);
    std::vector<index> gids = tree->get_nodes_only(mask,anchor);

    ArrayDatum result;
    result.reserve(gids.size());
    for(std::vector<index>::iterator it = gids.begin(); it != gids.end(); ++it)
      result.push_back(new IntegerDatum(*it));

    i->OStack.pop(3);
    i->OStack.push(result);
    i->EStack.pop();
  }

  std::string LayerExpected::message()
  {
    return std::string();
  }


} // namespace nest
