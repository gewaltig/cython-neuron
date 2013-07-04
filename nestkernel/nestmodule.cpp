/*
 *  nestmodule.cpp
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nestmodule.h"

#include <iostream>  
#include <sstream>
#include "nest.h"
#include "network.h"
#include "nodelist.h"
#include "interpret.h"
#include "node.h"
#include "subnet.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "booldatum.h"
#include "arraydatum.h"
#include "stringdatum.h"
#include "tokenutils.h"
#include "sliexceptions.h"
#include "random_datums.h"
#include "connectiondatum.h"
#include "communicator.h"
#include "communicator_impl.h"
#include "genericmodel.h"

// for RandomPopulationConnectD
#include "gsl_binomial_randomdev.h"
#include "gslrandomgen.h"
#include "normal_randomdev.h"
#include "fdstream.h"

#if defined IS_BLUEGENE_P || defined IS_BLUEGENE_Q
extern "C"
{
  // These functions are defined in the file "bg_get_mem.c". They need
  // to reside in a plain C file, because the #pragmas defined in the
  // BG header files interfere with C++, causing "undefined reference
  // to non-virtual thunk" MH 12-02-22, redid fix by JME 12-01-27.
  long bg_get_heap_mem();
  long bg_get_stack_mem();
}
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

extern int SLIsignalflag;

namespace nest
{
  SLIType NestModule::ConnectionType;

  Network* NestModule::net_ = 0;

  // At the time when NestModule is constructed, the SLI Interpreter
  // must already be initialized. NestModule relies on the presence of
  // the following SLI datastructures: Name, Dictionary

  NestModule::NestModule()
  {
    // The network pointer must be initalized using register_network()
    // before the NestModule instance is created.
    assert(net_ != 0);
  }

  NestModule::~NestModule()
  {
    // The network is deleted outside NestModule, since the
    // dynamicloadermodule also needs it

    ConnectionType.deletetypename();
  }

  void NestModule::register_network(Network& net)
  {
    assert(net_ == 0);   // register_network() must be called once only
    net_ = &net;
  }

  // The following concerns the new module:

  const std::string NestModule::name(void) const
  {
    return std::string("NEST Kernel 2"); // Return name of the module
  }

  const std::string NestModule::commandstring(void) const
  {
    return std::string("/nest-init /C++ ($Revision$) provide-component "
                       "/nest-init /SLI (1.21) require-component");
  }


  /* BeginDocumentation
     Name: ChangeSubnet - change the current working subnet.
     Synopsis:
     gid   ChangeSubnet -> -
     Parameters:
     gid - The GID of the new current subnet.
     Description:
     Change the current subnet to the one given as argument. Create
     will place newly created nodes in the current working subnet.
     ChangeSubnet is not allowed for layer subnets used in the
     topology module.
     
     This function can be used to change the working subnet to a new
     location, similar to the UNIX command cd.

     SeeAlso: CurrentSubnet
  */

  void NestModule::ChangeSubnet_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    index node_gid = getValue<long>(i->OStack.pick(0));

    if(get_network().get_node(node_gid)->allow_entry())
      get_network().go_to(node_gid);
    else
      throw SubnetExpected();
    
    i->OStack.pop();
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: CurrentSubnet - return the gid of the current network node.

     Synopsis: CurrentSubnet -> gid
     Description:
     CurrentSubnet returns the gid of the current working subnet in form
     of an integer number
     Availability: NEST
     SeeAlso: ChangeSubnet
     Author: Marc-Oliver Gewaltig
  */  
  void NestModule::CurrentSubnetFunction::execute(SLIInterpreter *i) const
  {
    assert(get_network().get_cwn() != 0);
    index current = get_network().get_cwn()->get_gid();

    i->OStack.push(current);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: SetStatus - sets the value of properties of a node, connection, random deviate generator or object

     Synopsis: 
     gid   dict SetStatus -> -
     conn  dict SetStatus -> -
     rdev  dict SetStatus -> -
     obj   dict SetStatus -> -

     Description:
     SetStatus changes properties of a node (specified by its gid), a connection 
     (specified by a connection object), a random deviate generator (see GetStatus_v 
     for more) or an object as used in object-oriented programming in SLI 
     (see cvo for more). Properties can be inspected with GetStatus. 

     Note that many properties are read-only and cannot be changed.

     Examples:
     /dc_generator Create /dc_gen Set  %Creates a dc_generator, which is a node
     dc_gen GetStatus info %view properties (amplitude is 0)
     dc_gen << /amplitude 1500. >> SetStatus
     dc_gen GetStatus info % amplitude is now 1500

     Author: docu by Sirko Straube

     SeeAlso: ShowStatus, GetStatus, info, modeldict, Set, SetStatus_v, SetStatus_dict
  */
  void NestModule::SetStatus_idFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    DictionaryDatum dict = getValue<DictionaryDatum>(i->OStack.top());
    index node_id = getValue<long>(i->OStack.pick(1));

    // Network::set_status() performs entry access checks for each
    // target and throws UnaccessedDictionaryEntry where necessary 
    get_network().set_status(node_id,dict);
		
    i->OStack.pop(2);
    i->EStack.pop();
  }

  void NestModule::SetStatus_CDFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    DictionaryDatum dict = getValue<DictionaryDatum>(i->OStack.top());
    ConnectionDatum conn = getValue<ConnectionDatum>(i->OStack.pick(1));
    DictionaryDatum conn_dict = conn.get_dict();

    long synapse_id = getValue<long>(conn_dict, nest::names::synapse_modelid);
    long port = getValue<long>(conn_dict, nest::names::port);
    long gid = getValue<long>(conn_dict, nest::names::source);
    thread tid = getValue<long>(conn_dict, nest::names::target_thread);
    get_network().get_node(gid); // Just to check if the node exists

    dict->clear_access_flags();

    get_network().set_synapse_status(gid, synapse_id, port, tid, dict);

    std::string missed;
    if ( !dict->all_accessed(missed) )
    {
      if ( get_network().dict_miss_is_error() )
        throw UnaccessedDictionaryEntry(missed);
      else
        get_network().message(SLIInterpreter::M_WARNING, "SetStatus", 
                              ("Unread dictionary entries: " + missed).c_str());
    }

    i->OStack.pop(2);
    i->EStack.pop();
  }

  void NestModule::Cva_CFunction::execute(SLIInterpreter *i) const
  {
    ConnectionDatum conn = getValue<ConnectionDatum>(i->OStack.top());
    ArrayDatum ad;
    ad.push_back(conn.get_source_gid());
    ad.push_back(conn.get_target_gid());
    ad.push_back(conn.get_target_thread());
    ad.push_back(conn.get_synapse_model_id());
    ad.push_back(conn.get_port());
    Token result(ad);
    i->OStack.top().swap(result);
    i->EStack.pop();
  }
 
  void NestModule::SetStatus_aaFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    ArrayDatum dict_a = getValue<ArrayDatum>(i->OStack.top());
    ArrayDatum conn_a = getValue<ArrayDatum>(i->OStack.pick(1));

    if((dict_a.size() != 1) and (dict_a.size() != conn_a.size()))
    { 
      throw RangeCheck();
    }
    if (dict_a.size() ==1) // Broadcast
    {	
      DictionaryDatum dict = getValue<DictionaryDatum>(dict_a[0]);
      const size_t n_conns=conn_a.size();
      for(size_t con=0;con < n_conns; ++con)
      {
        ConnectionDatum con_id = getValue<ConnectionDatum>(conn_a[con]);
	dict->clear_access_flags();
	get_network().set_synapse_status(con_id.get_source_gid(), // source_gid
                                         con_id.get_synapse_model_id(), // synapse_id
                                         con_id.get_port(), // port
                                         con_id.get_target_thread(), // target thread
                                         dict);
	std::string missed;
	if ( !dict->all_accessed(missed) )
	  {
	    if ( get_network().dict_miss_is_error() )
	      throw UnaccessedDictionaryEntry(missed);
	    else
	      get_network().message(SLIInterpreter::M_WARNING, "SetStatus", 
				    ("Unread dictionary entries: " + missed).c_str());
	  }
      }
    }
    else
    {
      const size_t n_conns=conn_a.size();
      for(size_t con=0;con < n_conns; ++con)
      {
        DictionaryDatum dict = getValue<DictionaryDatum>(dict_a[con]);
        ConnectionDatum con_id = getValue<ConnectionDatum>(conn_a[con]);
	dict->clear_access_flags();
        get_network().set_synapse_status(con_id.get_source_gid(), // source_gid
                                         con_id.get_synapse_model_id(), // synapse_id
                                         con_id.get_port(), // port
                                         con_id.get_target_thread(), // target thread
                                         dict);
	std::string missed;
	if ( !dict->all_accessed(missed) )
	  {
	    if ( get_network().dict_miss_is_error() )
	      throw UnaccessedDictionaryEntry(missed);
	    else
	      get_network().message(SLIInterpreter::M_WARNING, "SetStatus", 
				    ("Unread dictionary entries: " + missed).c_str());
	  }
      }
    }
    
    i->OStack.pop(2);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: GetStatus - return the property dictionary of a node, connection, random deviate generator or object
     Synopsis: 
     gid   GetStatus -> dict
     conn  GetStatus -> dict
     rdev  GetStatus -> dict
     obj   GetStatus -> dict

     Description:
     GetStatus returns a dictionary with the status information 
     for a node (specified by its gid), a connection (specified by a connection
     object), a random deviate generator (see GetStatus_v for more) or an
     object as used in object-oriented programming in SLI (see cvo for more).

     The interpreter exchanges data with the network element using
     its status dictionary. To abbreviate the access pattern
          gid GetStatus /lit get
     a variant of get implicitly calls GetStatus
          gid /lit get .
     In this way network elements and dictionaries can be accessed
     with the same syntax. Sometimes access to nested data structures in 
     the status dictionary is required. In this case the advanced addressing
     scheme of get is useful in which the second argument is an array of 
     literals. See the documentation of get for details.

     The information contained in the property dictionary depends on the
     concrete node model.
     
     Please refer to the model documentation for details.
  
     Standard entries for nodes:

     global_id   - local ID of the node
     status      - integer, representing the status flags of the node
     model       - literal, defining the current node
     thread      - the thread the node is allocated on
     vp          - the virtual process a node belongs to
  
     Note that the standard entries cannot be modified directly.

     Author: Marc-Oliver Gewaltig
     Availability: NEST
     SeeAlso: ShowStatus, info, SetStatus, get, GetStatus_v, GetStatus_dict
  */
  void NestModule::GetStatus_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);
     
    index node_id = getValue<long>(i->OStack.pick(0));  
    DictionaryDatum dict= get_network().get_status(node_id);

    i->OStack.pop();
    i->OStack.push(dict);
    i->EStack.pop();
  }

  void NestModule::GetStatus_CFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    ConnectionDatum conn = getValue<ConnectionDatum>(i->OStack.pick(0));

    long gid=conn.get_source_gid();
    get_network().get_node(gid); // Just to check if the node exists
     
    DictionaryDatum result_dict = get_network().get_synapse_status(
        gid,
        conn.get_synapse_model_id(), 
        conn.get_port(),
        conn.get_target_thread());

    i->OStack.pop();
    i->OStack.push(result_dict);
    i->EStack.pop();
  }

  // [intvector1,...,intvector_n]  -> [dict1,.../dict_n] 
  void NestModule::GetStatus_aFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);
    const ArrayDatum conns = getValue<ArrayDatum>(i->OStack.pick(0));
    size_t n_results=conns.size();
    ArrayDatum result;
    result.reserve(n_results);
    for(size_t nt=0; nt< n_results; ++nt)
    {
      ConnectionDatum con_id= getValue<ConnectionDatum>(conns.get(nt));
      DictionaryDatum result_dict = get_network().get_synapse_status(
          con_id.get_source_gid(),
          con_id.get_synapse_model_id(),
          con_id.get_port(),
          con_id.get_target_thread());
      result.push_back(result_dict);
    }

    i->OStack.pop();
    i->OStack.push(result);
    i->EStack.pop();
  }

  /*BeginDocumentation
    Name: SetDefaults - Set the default values for a node or synapse model.
    Synopsis: /modelname dict SetDefaults -> -
    SeeAlso: GetDefaults
    Author: Jochen Martin Eppler
    FirstVersion: September 2008
  */
  void NestModule::SetDefaults_l_DFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);

    const Name modelname = getValue<Name>(i->OStack.pick(1));
    DictionaryDatum dict = getValue<DictionaryDatum>(i->OStack.pick(0));

    const Token nodemodel = get_network().get_modeldict().lookup(modelname);
    const Token synmodel = get_network().get_synapsedict().lookup(modelname);

    dict->clear_access_flags(); // set properties with access control

    if (!nodemodel.empty())
    {
      const index model_id = static_cast<index>(nodemodel);
      get_network().get_model(model_id)->set_status(dict);
    }
    else if (!synmodel.empty())
    {
      const index synapse_id = static_cast<index>(synmodel);
      get_network().set_connector_defaults(synapse_id, dict);
    }
    else
      throw UnknownModelName(modelname.toString());

    std::string missed;
    if (!dict->all_accessed(missed))
    {
      if (get_network().dict_miss_is_error())
        throw UnaccessedDictionaryEntry(missed);
      else
        get_network().message(SLIInterpreter::M_WARNING, "SetDefaults", ("Unread dictionary entries: " + missed).c_str());
    }

    i->OStack.pop(2);
    i->EStack.pop();
  }

  /*BeginDocumentation
    Name: GetDefaults - Return the default values for a node or synapse model.
    Synopsis: /modelname GetDefaults -> dict
    SeeAlso: SetDefaults
    Author: Jochen Martin Eppler
    FirstVersion: September 2008
  */
  void NestModule::GetDefaults_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    const Name modelname = getValue<Name>(i->OStack.pick(0));

    const Token nodemodel = get_network().get_modeldict().lookup(modelname);
    const Token synmodel = get_network().get_synapsedict().lookup(modelname);

    DictionaryDatum dict;
 
    if (!nodemodel.empty())
    {
      const long model_id = static_cast<long>(nodemodel);
      Model* m= get_network().get_model(model_id);
      dict = m->get_status();
    }
    else if (!synmodel.empty())
    {
      const long synapse_id = static_cast<long>(synmodel);
      dict = get_network().get_connector_defaults(synapse_id);
    }
    else
      throw UnknownModelName(modelname.toString());

    i->OStack.pop();
    i->OStack.push(dict);
    i->EStack.pop();
  }

  // params: params

  // params: params
  void NestModule::GetConnections_DFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    DictionaryDatum dict = getValue<DictionaryDatum>(i->OStack.pick(0));

    dict->clear_access_flags();

    ArrayDatum array = get_network().get_connections(dict);
    std::string missed;
     
    i->OStack.pop();
    i->OStack.push(array);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: Simulate - simulate n milliseconds
  
     Synopsis:
     n(int) Simulate -> -
  
     Description: Simulate the network for n milliseconds. 
     Use resume to continue the simulation after an interrupt.

     SeeAlso: resume, ResumeSimulation, unit_conversion
  */
  void NestModule::SimulateFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    const double time= i->OStack.top();

    std::ostringstream os;
    os << "Simulating " << time << " ms.";
    i->message(SLIInterpreter::M_INFO, "Simulate", os.str().c_str());
    Time t = Time::ms(time);

    //experimental callback and signal safe, uncomment for testing, MD 090105
        //i->EStack.push(i->execbarrier_token);  // protect by barrier
    get_network().simulate(t);
        //i->EStack.pop();                       // pop the barrier


    // successful end of simulate
    i->OStack.pop();
    i->EStack.pop();
  }

  /*BeginDocumentation
    Name: ResumeSimulation - resume an interrupted simulation
    SeeAlso: Simulate
  */
  void NestModule::ResumeSimulationFunction::execute(SLIInterpreter *i) const
  {
    get_network().resume();
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: CopyModel - copy a model to a new name, set parameters for copy, if given
     Synopsis:
     /model /new_model param_dict -> -
     /model /new_model            -> -
     Parameters:
     /model      - literal naming an existing model
     /new_model  - literal giving the name of the copy to create, must not
                   exist in modeldict or synapsedict before
     /param_dict - parameters to set in the new_model
     Description: 
     A copy of model is created and registered in modeldict or synapsedict
     under the name new_model. If a parameter dictionary is given, the parameters
     are set in new_model.
     Warning: It is impossible to unload modules after use of CopyModel.
   */
  void NestModule::CopyModel_l_l_DFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);

    // fetch existing model name from stack
    const Name oldmodname = getValue<Name>(i->OStack.pick(2));
    const std::string newmodname = getValue<std::string>(i->OStack.pick(1));
    DictionaryDatum dict = getValue<DictionaryDatum>(i->OStack.pick(0));

    const Dictionary &modeldict = get_network().get_modeldict();
    const Dictionary &synapsedict = get_network().get_synapsedict();

    if (modeldict.known(newmodname) || synapsedict.known(newmodname))
      throw NewModelNameExists(newmodname);

    dict->clear_access_flags(); // set properties with access control
    const Token oldnodemodel = modeldict.lookup(oldmodname);
    const Token oldsynmodel = synapsedict.lookup(oldmodname);

    if (!oldnodemodel.empty())
    {
      const index old_id = static_cast<index>(oldnodemodel);
      const index new_id = get_network().copy_model(old_id, newmodname);
      get_network().get_model(new_id)->set_status(dict);
    }
    else if (!oldsynmodel.empty())
    {
      const index old_id = static_cast<index>(oldsynmodel);
      const index new_id = get_network().copy_synapse_prototype(old_id, newmodname);
      get_network().set_connector_defaults(new_id, dict);
    }
    else
      throw UnknownModelName(oldmodname);

    std::string missed;
    if ( !dict->all_accessed(missed) )
    {
      if ( get_network().dict_miss_is_error() )
        throw UnaccessedDictionaryEntry(missed);
      else
        get_network().message(SLIInterpreter::M_WARNING, "CopyModel", ("Unread dictionary entries: " + missed).c_str());
    }
    
    i->OStack.pop(3);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: Create - create a number of equal nodes in the current subnet
     Synopsis:
     /model          Create -> gid
     /model n        Create -> gid
     /model   params Create -> gid
     /model n params Create -> gid
     Parameters:
     /model - literal naming the modeltype (entry in modeldict)
     n      - the desired number of nodes
     params - parameters for the newly created node(s)
     gid    - gid of last created node
     Description:
     Create generates n new network objects of the supplied model
     type. If n is not given, a single node is created. The objects
     are added as children of the current working node. params is a
     dictsionary with parameters for the new nodes.
 
     SeeAlso: modeldict, ChangeSubnet
  */
  void NestModule::Create_l_iFunction::execute(SLIInterpreter *i) const
  {
    // check for stack load
    i->assert_stack_load(2);

    // extract arguments
    const long n_nodes = getValue<long>(i->OStack.pick(0));
    if ( n_nodes <= 0 )
      throw RangeCheck();

    const std::string modname = getValue<std::string>(i->OStack.pick(1));
    const Token model = get_network().get_modeldict().lookup(modname);
    if ( model.empty() )
      throw  UnknownModelName(modname);
       
    // create
    const index model_id = static_cast<index>(model);
    const long last_node_id = get_network().add_node(model_id, n_nodes);
    i->OStack.pop(2);
    i->OStack.push(last_node_id);
    i->EStack.pop();
  }

  void NestModule::RestoreNodes_aFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);
    ArrayDatum node_list=getValue<ArrayDatum>(i->OStack.top());
    get_network().restore_nodes(node_list);
    i->OStack.pop();
    i->EStack.pop();
  }

  void NestModule::GetNodes_i_D_b_bFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(4);

    const bool return_gids_only = getValue<bool>(i->OStack.pick(0));
    const bool  include_remote = not getValue<bool>(i->OStack.pick(1));
    const DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(2));
    const index node_id        = getValue<long>(i->OStack.pick(3));

    Subnet *subnet = dynamic_cast<Subnet *>(get_network().get_node(node_id));     
    if (subnet==NULL)
      throw SubnetExpected();

    LocalNodeList localnodes(*subnet);
    vector<Communicator::NodeAddressingData> globalnodes;
    if (params->empty())
      nest::Communicator::communicate(localnodes,globalnodes,include_remote);
    else
      nest::Communicator::communicate(localnodes, globalnodes, get_network(), params, include_remote);

    ArrayDatum result;
    result.reserve(globalnodes.size());
    for( vector<Communicator::NodeAddressingData>::iterator n = globalnodes.begin(); n != globalnodes.end(); ++n )
    {
      if ( return_gids_only )
        result.push_back(new IntegerDatum(n->get_gid()));
      else
      {
        DictionaryDatum* node_info = new DictionaryDatum(new Dictionary);
        (**node_info)[names::global_id] = n->get_gid();
        (**node_info)[names::vp] = n->get_vp();
        (**node_info)[names::parent] = n->get_parent_gid();
        result.push_back(node_info);
      }
    }

    i->OStack.pop(4);
    i->OStack.push(result);
    i->EStack.pop();
  }

  void NestModule::GetChildren_i_D_bFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);

    const bool  include_remote = not getValue<bool>(i->OStack.pick(0));
    const DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(1));
    const index node_id        = getValue<long>(i->OStack.pick(2));

    Subnet *subnet = dynamic_cast<Subnet *>(get_network().get_node(node_id));     
    if (subnet == NULL)
      throw SubnetExpected();
 
    LocalChildList localnodes(*subnet);
    ArrayDatum result;

    vector<Communicator::NodeAddressingData> globalnodes;
    if (params->empty())
       nest::Communicator::communicate(localnodes,globalnodes,include_remote);
    else
      nest::Communicator::communicate(localnodes, globalnodes, get_network(), params, include_remote);
    result.reserve(globalnodes.size());
    for(vector<Communicator::NodeAddressingData>::iterator n = globalnodes.begin(); n != globalnodes.end(); ++n)
      result.push_back(new IntegerDatum(n->get_gid()));
    
    i->OStack.pop(3);
    i->OStack.push(result);
    i->EStack.pop();
  }

  void NestModule::GetLeaves_i_D_bFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);

    const bool  include_remote = not getValue<bool>(i->OStack.pick(0));
    const DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(1));
    const index node_id        = getValue<long>(i->OStack.pick(2));

    Subnet *subnet = dynamic_cast<Subnet *>(get_network().get_node(node_id));     
    if (subnet == NULL)
      throw SubnetExpected();
 
    LocalLeafList localnodes(*subnet);
    ArrayDatum result;

    vector<Communicator::NodeAddressingData> globalnodes;
    if (params->empty())
      nest::Communicator::communicate(localnodes,globalnodes,include_remote);
    else
      nest::Communicator::communicate(localnodes, globalnodes, get_network(), params, include_remote);
    result.reserve(globalnodes.size());

    for(vector<Communicator::NodeAddressingData>::iterator n = globalnodes.begin(); n != globalnodes.end(); ++n)
      result.push_back(new IntegerDatum(n->get_gid()));

    i->OStack.pop(3);
    i->OStack.push(result);
    i->EStack.pop();
  }


  /* BeginDocumentation
     Name: ResetKernel - Put the simulation kernel back to its initial state.
     Description: 
     This function re-initializes the simulation kernel, returning it to the 
     same state as after NEST has started. 
     In particular,
     - all network nodes
     - all connections
     - all user-defined neuron and synapse models
     are deleted, and 
     - time 
     - random generators
     are reset. The only exception is that dynamically loaded modules are not 
     unloaded. This may change in a future version of NEST. The SLI interpreter
     is not affected by ResetKernel.
     Availability: NEST
     Author: Marc-oliver Gewaltig
     SeeAlso: ResetNetwork, reset, ResetOptions
  */
  void NestModule::ResetKernelFunction::execute(SLIInterpreter *i) const
  {
    get_network().reset_kernel();
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: ResetNetwork - Reset the dynamic state of the network.
     Synopsis: ResetNetwork -> -
     Description:
     ResetNetwork resets the dynamic state of the entire network to its state 
     at T=0. The dynamic state comprises typically the membrane potential, 
     synaptic currents, buffers holding input that has been delivered, but not
     yet become effective, and all events pending delivery. Technically, this
     is achieve by calling init_state() on all nodes and forcing a call to 
     init_buffers() upon the next call to Simulate. Node parameters, such as
     time constants and threshold potentials, are not affected.
     
     Note: 
     - Time and random number generators are NOT reset.
     - Files belonging to recording devices (spike detector, voltmeter, etc)
       are closed. You must change the file name before simulating again, otherwise
       the files will be overwritten og you will receive an error, depending on
       the value of /overwrite_files (in root node).
     - ResetNetwork will reset the nodes to the state values stored in the model
       prototypes. So if you have used SetDefaults to change a state value of a
       model since you called Simulate the first time, the network will NOT be reset
       to the status at T=0.
     - The dynamic state of synapses with internal dynamics (STDP, facilitation) is
       NOT reset at present. This will be implemented in a future version of NEST.

     SeeAlso: ResetKernel, reset
  */
  void NestModule::ResetNetworkFunction::execute(SLIInterpreter *i) const
  {
    get_network().reset_network();
    i->message(SLIInterpreter::M_INFO, "ResetNetworkFunction",
      "The network has been reset. Random generators and time have NOT been reset.");
      
    i->EStack.pop();
  }

  // Connect for gid gid
  // See lib/sli/nest-init.sli for details
  void NestModule::Connect_i_i_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);
    
    index source = getValue<long>(i->OStack.pick(2));
    index target = getValue<long>(i->OStack.pick(1));
    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));

    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().connect(source, target, synmodel_id);
    
    i->OStack.pop(3);
    i->EStack.pop();
  }

  void NestModule::Connect_i_i_iFunction::execute(SLIInterpreter *i) const
  {
      long &source = static_cast<IntegerDatum *>(i->OStack.pick(2).datum())->get();
      long &target = static_cast<IntegerDatum *>(i->OStack.pick(1).datum())->get();
      long &synmodel_id = static_cast<IntegerDatum *>(i->OStack.pick(0).datum())->get();

      get_network().connect(source, target, synmodel_id);
      
      i->OStack.pop(3);
      i->EStack.pop();
  }

  // Connect for gid gid weight delay
  // See lib/sli/nest-init.sli for details
  void NestModule::Connect_i_i_d_d_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(5);

    index source = getValue<long>(i->OStack.pick(4));
    index target = getValue<long>(i->OStack.pick(3));
    double_t weight = getValue<double_t>(i->OStack.pick(2));
    double_t delay  = getValue<double_t>(i->OStack.pick(1));
    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));

    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().connect(source, target, weight, delay, synmodel_id);
    
    i->OStack.pop(5);
    i->EStack.pop();
  }

 // Connect for gid gid weight delay syn_id
  // See lib/sli/nest-init.sli for details
  void NestModule::Connect_i_i_d_d_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(5);

    index source = getValue<long>(i->OStack.pick(4));
    index target = getValue<long>(i->OStack.pick(3));
    double_t weight = getValue<double_t>(i->OStack.pick(2));
    double_t delay  = getValue<double_t>(i->OStack.pick(1));
    index synmodel_id = getValue<long>(i->OStack.pick(0));

    get_network().connect(source, target, weight, delay, synmodel_id);
    
    i->OStack.pop(5);
    i->EStack.pop();
  }

  // Connect for gid dict syn_id
  // See lib/sli/nest-init.sli for details
  void NestModule::Connect_i_D_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);

    index source = getValue<long>(i->OStack.pick(2));
    DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(1));
    index synmodel_id = getValue<long>(i->OStack.pick(0));
 
    params->clear_access_flags();
    index target=getValue<long>(params->lookup(names::target));

    if ( get_network().connect(source, target, params, synmodel_id) )
    {
      // dict access control only if we actually made a connection
      std::string missed;
      if ( !params->all_accessed(missed) )
      {
	//if ( get_network().dict_miss_is_error() )
//	  throw UnaccessedDictionaryEntry(missed);
//	else
	  get_network().message(SLIInterpreter::M_WARNING, "Connect", 
				("Unread dictionary entries: " + missed).c_str());
      }
    }

    i->OStack.pop(3);
    i->EStack.pop();
  }


  // Connect for gid gid dict
  // See lib/sli/nest-init.sli for details
  void NestModule::Connect_i_i_D_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(4);

    index source = getValue<long>(i->OStack.pick(3));
    index target = getValue<long>(i->OStack.pick(2));
    DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(1));
    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    params->clear_access_flags();
 
    if ( get_network().connect(source, target, params, synmodel_id) )
    {
      // dict access control only if we actually made a connection
      std::string missed;
      if ( !params->all_accessed(missed) )
      {
	if ( get_network().dict_miss_is_error() )
	  throw UnaccessedDictionaryEntry(missed);
	else
	  get_network().message(SLIInterpreter::M_WARNING, "Connect", 
				("Unread dictionary entries: " + missed).c_str());
      }
    }

    i->OStack.pop(4);
    i->EStack.pop();
  }


   /* BeginDocumentation
     Name: DataConnect_i_dict_s - Connect many neurons from data.

     Synopsis: 
     gid dict model  DataConnect -> -

     gid    - GID of the source neuron
     dict   - dictionary with connection parameters
     model  - the synapse model as string or literal

     Description:
     Connects the source neuron to targets according to the data in dict, using the synapse 'model'.

     Dict is a parameter dictionary that must contain the connection parameters as DoubleVectors. 
     The parameter dictionary must contain at least the fields:
     /target <. gid_1 ... gid_n .>
     /weight <. w1_1 ... w_n .>
     /delay  <. d_1 ... d_n .>
     All of these must be DoubleVectors of the same length.

     Depending on the synapse model, the dictionary may contain other keys, again as
     DoubleVectors of the same length as /target. 

     DataConnect will iterate all vectors and create the connections according to the parameters given.
     SeeAlso: DataConnect_a, DataConnect
     Author: Marc-Oliver Gewaltig
   */
  void NestModule::DataConnect_i_dict_sFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(3);
     
    index source = getValue<long>(i->OStack.pick(2));
    DictionaryDatum params = getValue<DictionaryDatum>(i->OStack.pick(1));
    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().divergent_connect(source, params,synmodel_id);
      // dict access control only if we actually made a connection
    std::string missed;
    if ( !params->all_accessed(missed) )
      {
	if ( get_network().dict_miss_is_error() )
	  throw UnaccessedDictionaryEntry(missed);
	else
	  get_network().message(SLIInterpreter::M_WARNING, "Connect", 
				("The following synapse parameters are unused: " + missed).c_str());
      }

    i->OStack.pop(3);
    i->EStack.pop();
  }

 /* BeginDocumentation
     Name: DataConnect_a - Connect many neurons from a list of synapse status dictionaries.

     Synopsis: 
     [dict1, dict2, ..., dict_n ]  DataConnect_a -> -

     This variant of DataConnect can be used to re-instantiate a given connectivity matrix.
     The argument is a list of dictionaries, each containing at least the keys
     /source
     /target
     /weight
     /delay
     /model
     
     Example:
     
     % assume a connected network

     << >> GetConnections Flatten /conns Set % Get all connections
     conns { GetStatus } Map      /syns  Set % retrieve their synapse status

     ResetKernel                             % clear everything
     % rebuild neurons
     syns DataConnect                        % restore the connecions


     Author: Marc-Oliver Gewaltig
     FirstVersion: May 2012
     SeeAlso: DataConnect_i_dict_s, Connect
  */
  void NestModule::DataConnect_aFunction::execute(SLIInterpreter *i) const
  {
      i->assert_stack_load(1);
      ArrayDatum connectome = getValue<ArrayDatum>(i->OStack.top());

      get_network().connect(connectome);
      i->OStack.pop();
      i->EStack.pop();
  }

  /* BeginDocumentation
     Name: DivergentConnect - Connect node to a population of nodes.
     
     Synopsis:
     source [targets]                              DivergentConnect -> -
     source [targets]                    /synmodel DivergentConnect -> -
     source [targets] [weights] [delays]           DivergentConnect -> -
     source [targets] [weights] [delays] /synmodel DivergentConnect -> -

     Parameters: 
     source    - GID of source node 
     [targets] - array of (global IDs of) potential target nodes 
     [weights] - weights for the connections. List of the same size as targets or 1.
     [delays]  - delays for the connections. List of the same size as targets or 1.
     /synmodel - The synapse model for the connection (see Options below)

     Options:
     If not given, the synapse model is taken from the Options dictionary
     of the Connect command.

     Description:
     Connect a neuron with a set of target neurons.
     
     Author:
     Marc-Oliver Gewaltig
     modified Ruediger Kupper, 20.3.2003
     modified Jochen Eppler, 04.11.2005
     SeeAlso: RandomDivergentConnect, ConvergentConnect
  */
  
  void NestModule::DivergentConnect_i_ia_a_a_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(5);

    long source_adr = getValue<long>(i->OStack.pick(4));
    TokenArray target_adr = getValue<TokenArray>(i->OStack.pick(3));

    TokenArray weights = getValue<TokenArray>(i->OStack.pick(2));
    TokenArray delays = getValue<TokenArray>(i->OStack.pick(1));

    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().divergent_connect(source_adr, target_adr, weights, delays, synmodel_id);

    i->OStack.pop(5);
    i->EStack.pop();
  }

  // Documentation can be found in lib/sli/nest-init.sli near definition
  // of the trie for RandomConvergentConnect.
  void NestModule::RDivergentConnect_i_i_ia_da_da_b_b_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(8);
     
    long source_adr = getValue<long>(i->OStack.pick(7));
    size_t n = getValue<long>(i->OStack.pick(6));
    TokenArray target_adr = getValue<TokenArray>(i->OStack.pick(5));
    TokenArray weights = getValue<TokenArray>(i->OStack.pick(4));
    TokenArray delays = getValue<TokenArray>(i->OStack.pick(3));      
    bool allow_multapses = getValue<bool>(i->OStack.pick(2));
    bool allow_autapses = getValue<bool>(i->OStack.pick(1));

    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().random_divergent_connect(source_adr, target_adr, n, weights, delays, allow_multapses, allow_autapses, synmodel_id);

    i->OStack.pop(8);
    i->EStack.pop();     
  }


  /* BeginDocumentation
     Name: ConvergentConnect - Connect population of nodes to a single node.

     Synopsis: 
     [sources] target                    /synmodel ConvergentConnect -> -
     [sources] target                    /synmodel ConvergentConnect -> -
     [sources] target [weights] [delays] /synmodel ConvergentConnect -> -

     Parameters: 
     [source]  - GID of source nodes
     target    - array of (global IDs of) potential target nodes 
     [weights] - weights for the connections. List of the same size as sources or 1.
     [delays]  - delays for the connections. List of the same size as sources or 1.
     /synmodel - The synapse model for the connection (see Options below)

     Options:
     If not given, the synapse model is taken from the Options dictionary
     of the Connect command.

     Description:
     Connect a set of source neurons to a single neuron.

     Author:
     Ruediger Kupper, via copy-paste-and-modify from Oliver's DivergentConnect.
     Modified Ruediger Kupper, 20.03.2003
     Modified Jochen Martin Eppler, 03.03.2009
     SeeAlso: RandomConvergentConnect, DivergentConnect
  */

  void NestModule::ConvergentConnect_ia_i_a_a_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(5);

    TokenArray source_adr = getValue<TokenArray>(i->OStack.pick(4));
    long target_adr = getValue<long>(i->OStack.pick(3));

    TokenArray weights = getValue<TokenArray>(i->OStack.pick(2));
    TokenArray delays = getValue<TokenArray>(i->OStack.pick(1));
     
    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);


    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().convergent_connect(source_adr, target_adr, weights, delays, synmodel_id);

    i->OStack.pop(5);
    i->EStack.pop();
  }
    

     
  // Documentation can be found in lib/sli/nest-init.sli near definition
  // of the trie for RandomConvergentConnect.
  void NestModule::RConvergentConnect_ia_i_i_da_da_b_b_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(8);
     
    TokenArray source_adr = getValue<TokenArray>(i->OStack.pick(7));
    long target_adr = getValue<long>(i->OStack.pick(6));
    size_t n = getValue<long>(i->OStack.pick(5));
    TokenArray weights = getValue<TokenArray>(i->OStack.pick(4));
    TokenArray delays = getValue<TokenArray>(i->OStack.pick(3));      
    bool allow_multapses = getValue<bool>(i->OStack.pick(2));
    bool allow_autapses = getValue<bool>(i->OStack.pick(1));

    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().random_convergent_connect(source_adr, target_adr, n, weights, delays, allow_multapses, allow_autapses, synmodel_id);

    i->OStack.pop(8);
    i->EStack.pop();     
  }

  // Documentation can be found in lib/sli/nest-init.sli near definition
  // of the trie for RandomConvergentConnect.
  void NestModule::RConvergentConnect_ia_ia_ia_daa_daa_b_b_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(8);
     
    TokenArray source_adr = getValue<TokenArray>(i->OStack.pick(7));
    TokenArray target_adr = getValue<TokenArray>(i->OStack.pick(6));
    TokenArray n = getValue<TokenArray>(i->OStack.pick(5));
    TokenArray weights = getValue<TokenArray>(i->OStack.pick(4));
    TokenArray delays = getValue<TokenArray>(i->OStack.pick(3));      
    bool allow_multapses = getValue<bool>(i->OStack.pick(2));
    bool allow_autapses = getValue<bool>(i->OStack.pick(1));

    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    get_network().random_convergent_connect(source_adr, target_adr, n, weights, delays, allow_multapses, allow_autapses, synmodel_id);

    i->OStack.pop(8);
    i->EStack.pop();     
  }

  /* BeginDocumentation
     Name: MemoryInfo - Report current memory usage.
     Description:
     MemoryInfo reports the current utilization of the memory manager for all models,
     which are used at least once. The output is sorted ascending according according
     to the name of the model is written to stdout. The unit of the data is byte.
     Note that MemoryInfo only gives you information about the memory requirements of
     the static model data inside of NEST. It does not tell anything about the memory
     situation on your computer. 
     Synopsis:
     MemoryInfo -> -
     Availability: NEST
     Author: Jochen Martin Eppler
  */
  void NestModule::MemoryInfoFunction::execute(SLIInterpreter *i) const
  {
    get_network().memory_info();
    i->EStack.pop();
  }


#if defined IS_BLUEGENE_P || defined IS_BLUEGENE_Q
  /* BeginDocumentation
     Name: memory_thisjob_bg - Reports memory usage on Blue Gene/P/Q systems
     Description:
     BGMemInfo returns a dictionary with the heap and stack memory
     usage of a process in Bytes.
     Availability: NEST
     Author: Jochen Martin Eppler
  */
  void NestModule::MemoryThisjobBgFunction::execute(SLIInterpreter *i) const
  {
    DictionaryDatum dict(new Dictionary);

    unsigned long heap_memory = bg_get_heap_mem();
    (*dict)["heap"] = heap_memory;
    unsigned long stack_memory = bg_get_stack_mem();
    (*dict)["stack"] = stack_memory;
  
    i->OStack.push(dict);
    i->EStack.pop();
  }
#endif

  /* BeginDocumentation
     Name: PrintNetwork - Print network tree in readable form.
     Synopsis: 
     gid depth  PrintNetwork -> -
     Parameters: 
     gid        - Global ID of the subnet to start tree printout. 
     depth      - Integer, specifies down to which level the network is printed.
     Description:
     This function prints the network structure in a concise tree-like format 
     according to the following rules:
     - Each Node is shown on a separate line, showing its model name followed 
     by its in global id in brackets.
     
     +-[0] Subnet Dim=[1]
     |
     +- iaf_neuron [1]
     
     - Consecutive Nodes of the same model are summarised in a list. 
     The list shows the model name, the global id of the first node in the
     sequence, then the number of consecutive nodes, then the global id of 
     the last node in the sequence.
     
     +-[0] Subnet Dim=[1]
     |
     +- iaf_neuron [1]..(2)..[2]
     
     - If a node is a subnet, its global id is printed first, followed by the model
     name or its label (if it is defined). Next, the dimension is shown.
     If the current recursion level is less than the specified depth, the printout descends
     to the children of the subnet. 
     After the header, a new line is printed, followed by the list of children
     at the next indentation level.
     After the last child, a new line is printed and the printout of the parent subnet
     is continued.

     Example:
     SLI ] /iaf_neuron Create
     SLI [1] /iaf_cond_alpha 10 Create
     SLI [2] /dc_generator [2 5 6] LayoutNetwork
     SLI [3] [0] 1 PrintNetwork
     +-[0] Subnet Dim=[12]
        |
        +- iaf_neuron [1]
        +- lifb_cond_neuron [2]..(10)..[11]
        +-[12] Subnet Dim=[2 5 6]
     SLI [3] [0] 2 PrintNetwork
     +-[0] Subnet Dim=[12]
        |
        +- iaf_neuron [1]
        +- lifb_cond_neuron [2]..(10)..[11]
        +-[12] Subnet Dim=[2 5 6]
            |
            +-[13] Subnet Dim=[5 6]
            +-[49] Subnet Dim=[5 6]
     SLI [3] [0] 3 PrintNetwork
     +-[0] Subnet Dim=[12]
        |
        +- iaf_neuron [1]
        +- lifb_cond_neuron [2]..(10)..[11]
        +-[12] Subnet Dim=[2 5 6]
            |
            +-[13] Subnet Dim=[5 6]
            |   |
            |   +-[14] Subnet Dim=[6]
            |   +-[21] Subnet Dim=[6]
            |   +-[28] Subnet Dim=[6]
            |   +-[35] Subnet Dim=[6]
            |   +-[42] Subnet Dim=[6]
            +-[49] Subnet Dim=[5 6]
                |
                +-[50] Subnet Dim=[6]
                +-[57] Subnet Dim=[6]
                +-[64] Subnet Dim=[6]
                +-[71] Subnet Dim=[6]
                +-[78] Subnet Dim=[6]
     SLI [3] [0] 4 PrintNetwork
     +-[0] Subnet Dim=[12]
        |
        +- iaf_neuron [1]
        +- lifb_cond_neuron [2]..(10)..[11]
        +-[12] Subnet Dim=[2 5 6]
            |
            +-[13] Subnet Dim=[5 6]
            |   |
            |   +-[14] Subnet Dim=[6]
            |   |   |
            |   |   +- dc_generator [15]..(6)..[20]
            |   |
            |   +-[21] Subnet Dim=[6]
            |   |   |
            |   |   +- dc_generator [22]..(6)..[27]
            |   |
            |   +-[28] Subnet Dim=[6]
            |   |   |
            |   |   +- dc_generator [29]..(6)..[34]
            |   |
            |   +-[35] Subnet Dim=[6]
            |   |   |
            |   |   +- dc_generator [36]..(6)..[41]
            |   |
            |   +-[42] Subnet Dim=[6]
            |       |
            |       +- dc_generator [43]..(6)..[48]
            |
            +-[49] Subnet Dim=[5 6]
                |
                +-[50] Subnet Dim=[6]
                |   |
                |   +- dc_generator [51]..(6)..[56]
                |
                +-[57] Subnet Dim=[6]
                |   |
                |   +- dc_generator [58]..(6)..[63]
                |
                +-[64] Subnet Dim=[6]
                |   |
                |   +- dc_generator [65]..(6)..[70]
                |
                +-[71] Subnet Dim=[6]
                |   |
                |   +- dc_generator [72]..(6)..[77]
                |
                +-[78] Subnet Dim=[6]
                    |
                    +- dc_generator [79]..(6)..[84]


     Availability: NEST
     Author: Marc-Oliver Gewaltig, Jochen Martin Eppler
  */
  void NestModule::PrintNetworkFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);
    
    long gid = getValue<long>(i->OStack.pick(1));
    long depth = getValue<long>(i->OStack.pick(0));
    get_network().print(gid, depth - 1);

    i->OStack.pop(2);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: Rank - Return the MPI rank of the process.
     Synopsis: Rank -> int
     Description:
     Returns the rank of the MPI process (MPI_Comm_rank) executing the
     command. This function is mainly meant for logging and debugging
     purposes. It is highly discouraged to use this function to write
     rank-dependent code in a simulation script as this can break NEST
     in funny ways, of which dead-locks are the nicest.
     Availability: NEST 2.0
     Author: Jochen Martin Eppler
     FirstVersion: January 2006
     SeeAlso: NumProcesses, SyncProcesses, ProcessorName 
  */
  void NestModule::RankFunction::execute(SLIInterpreter *i) const
  {
    i->OStack.push(Communicator::get_rank());
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: NumProcesses - Return the number of MPI processes.
     Synopsis: NumProcesses -> int
     Description:
     Returns the number of MPI processes (MPI_Comm_size). This
     function is mainly meant for logging and debugging purposes.
     Availability: NEST 2.0
     Author: Jochen Martin Eppler
     FirstVersion: January 2006
     SeeAlso: Rank, SyncProcesses, ProcessorName
  */
  void NestModule::NumProcessesFunction::execute(SLIInterpreter *i) const
  {
    i->OStack.push(Communicator::get_num_processes());
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: SetFakeNumProcesses - Set a fake number of MPI processes.
     Synopsis: n_procs SetFakeNumProcesses -> -
     Description:
     Sets the number of MPI processes to n_procs. Used for benchmarking puposes only. 
     Availability: NEST 2.2
     Author: Susanne Kunkel
     FirstVersion: July 2011
     SeeAlso: NumProcesses
  */
  void NestModule::SetFakeNumProcessesFunction_i::execute(SLIInterpreter *i) const
  {
    long n_procs = getValue<long>(i->OStack.pick(0));
    Communicator::set_num_processes(n_procs);
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: SyncProcesses - Synchronize all MPI processes.
     Synopsis: SyncProcesses -> -
     Availability: NEST 2.0
     Author: Alexander Hanuschkin
     FirstVersion: April 2009
     Description:
     This function allows to synchronize all MPI processes at any
     point in a simulation script. Internally, the function uses
     MPI_Barrier(). Note that during simulation the processes are
     automatically synchronized without the need for user interaction.
     SeeAlso: Rank, NumProcesses, ProcessorName
  */
  void NestModule::SyncProcessesFunction::execute(SLIInterpreter *i) const
  {
    Communicator::synchronize();
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: TimeCommunication - returns average time taken for MPI_Allgather over n calls with m bytes
     Synopsis: 
     n m TimeCommunication -> time
     Availability: NEST 2.0
     Author: Abigail Morrison
     FirstVersion: August 2009
     Description:
     The function allows a user to test how much time a call the Allgather costs
  */
  void NestModule::TimeCommunication_i_i_bFunction::execute(SLIInterpreter *i) const 
  { 
    i->assert_stack_load(3); 
    long samples = getValue<long>(i->OStack.pick(2)); 
    long num_bytes = getValue<long>(i->OStack.pick(1)); 
    bool offgrid = getValue<bool>(i->OStack.pick(0));

    double_t time = 0.0;
    if ( offgrid )
      time = Communicator::time_communicate_offgrid(num_bytes,samples);
    else
      time = Communicator::time_communicate(num_bytes,samples);

    i->OStack.pop(3); 
    i->OStack.push(time);
    i->EStack.pop(); 
  } 

  /* BeginDocumentation
     Name: ProcessorName - Returns a unique specifier for the actual node.
     Synopsis: ProcessorName -> string
     Availability: NEST 2.0
     Author: Alexander Hanuschkin
     FirstVersion: April 2009
     Description:
     This function returns the name of the processor it was called
     on (MPI_Get_processor_name). See MPI documentation for more details. If NEST is not
     compiled with MPI support, this function returns the hostname of
     the machine as returned by the POSIX function gethostname().
     Examples:
     (I'm process ) =only Rank 1 add =only ( of ) =only NumProcesses =only ( on machine ) =only ProcessorName =
     SeeAlso: Rank, NumProcesses, SyncProcesses
  */
  void NestModule::ProcessorNameFunction::execute(SLIInterpreter *i) const
  {
    i->OStack.push(Communicator::get_processor_name());
    i->EStack.pop();
  }

#ifdef HAVE_MPI
  /* BeginDocumentation
     Name: abort - Abort all NEST processes gracefully.
     Paramteres:
     exitcode - The exitcode to quit with
     Description:
     This function can be run by the user to end all NEST processes as
     gracefully as possible. If NEST is compiled without MPI support,
     this will just call quit_i. If compiled with MPI support, it will
     call MPI_Abort, which will kill all processes of the application
     and thus prevents deadlocks. The exitcode is userabort in both
     cases (see statusdict/exitcodes).
     Availability: NEST 2.0
     Author: Jochen Martin Eppler
     FirstVersion: October 2012
     SeeAlso: quit, Rank, SyncProcesses, ProcessorName
  */
  void NestModule::MPIAbort_iFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1); 
    long exitcode = getValue<long>(i->OStack.pick(0));
    Communicator::mpi_abort(exitcode);
    i->EStack.pop();
  }
#endif

  /* BeginDocumentation
     Name: GetVpRNG - return random number generator associated to virtual process of node
     Synopsis:
     gid GetVpRNG -> rngtype
     Parameters: 
     gid  - global id of the node  
     Description: 
     This function is helpful in the implementation of parallelized wiring
     routines that create idential random structures independent of the
     number of machines and threads participating in the simulation. The
     function is used in SLI libraries, e.g. in the implementation of
     RandomConvergentConnect. There is probably no need to directly use
     GetVpRNG in scripts describing a particular simulation.

     In NEST each node (e.g. neuron) is assigned to a virtual process and
     each virtual process maintains its own random number generator. In a
     simulation run the virtual processes are equally distributed over the
     participating machines and threads as speciefied by the user. In NEST
     2.0 virtual processes are identified with threads.  Thus, with the
     option /total_num_virtual_procs of [0] set to n, there are in total
     always n threads (virtual processes) independent of the number of
     participating machines.  The concept of virtual processes is described
     in detail in [1].

     Identical results are achieved independent of the number of machines
     and threads participating in a simulation if all operations modifying
     a neuron and its incoming synapses use the random number generator of
     the virtual process the neuron is assigned to.

     An ArgumentTypeError is raised if GetVpRNG is called for a 
     non-local gid.

     Examples:
     In the implementation of RandomConvergentConnect the Connect
     operations are only carried out on the machine the target neuron lives
     on. Whether the neuron is located on a specific machine is tested
     using the /local property of the neuron.  The random selection of
     source neurons is made using the random number generator of the thread
     the target neuron is assigned to.

     References:
     [1] Morrison A, Mehring C, Geisel T, Aertsen A, and Diesmann M (2005)
         Advancing the boundaries of high connectivity network simulation 
         with distributed computing. Neural Computation 17(8):1776-1801
         The article is available at www.nest-initiative.org

     Author: Tobias Potjans, Moritz Helias, Diesmann     
     SeeAlso: GetGlobalRNG, RandomConvergentConnect
  */
  void NestModule::GetVpRngFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);
     
    index target = getValue<long>(i->OStack.pick(0));
    Node* target_node = get_network().get_node(target);

    if (!get_network().is_local_node(target_node))
      throw LocalNodeExpected(target);

    // Only nodes with proxies have a well-defined VP and thus thread.
    // Asking for the VP of, e.g., a subnet or spike_detector is meaningless.
    if ( !target_node->has_proxies() )
      throw NodeWithProxiesExpected(target);

    librandom::RngPtr rng = get_network().get_rng(target_node->get_thread());
  
    Token rt( new librandom::RngDatum(rng) );
    i->OStack.pop(1);
    i->OStack.push_move(rt);
    
    i->EStack.pop();
  }

  /* BeginDocumentation
     Name: GetGlobalRNG - return global random number generator
     Synopsis:
     GetGlobalRNG -> rngtype
     Description: 
     This function returns the global random number generator which
     can be used in situations where the same sequence of random
     numbers is needed in all MPI processes. The user must EXERT
     EXTREME CARE to ensure that all MPI processes use exactly
     the same random numbers while executing a script. NEST performs
     only a simple test upon each call to Simulate to check if the
     global RNGs on all MPI processes are still in sync.

     Examples:
     The RandomDivergentConnect function makes use of numbers
     from the global RNG.

     References:
     [1] Morrison A, Mehring C, Geisel T, Aertsen A, and Diesmann M (2005)
         Advancing the boundaries of high connectivity network simulation 
         with distributed computing. Neural Computation 17(8):1776-1801
         The article is available at www.nest-initiative.org

     Author: Tobias Potjans, Moritz Helias, Diesmann     
     SeeAlso: GetVpRNG, RandomDivergentConnect
  */
  void NestModule::GetGlobalRngFunction::execute(SLIInterpreter *i) const
  {
    librandom::RngPtr rng = get_network().get_grng();
  
    Token rt( new librandom::RngDatum(rng) );
    i->OStack.push_move(rt);
    
    i->EStack.pop();
  }

  void NestModule::Cvdict_CFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(1);

    ConnectionDatum conn = getValue<ConnectionDatum>(i->OStack.pick(0));
    DictionaryDatum dict = conn.get_dict();

    i->OStack.pop();
    i->OStack.push(dict);
    i->EStack.pop();
  }

#ifdef HAVE_MUSIC
  /* BeginDocumentation
     Name: SetAcceptableLatency - set the acceptable latency of a MUSIC input port

     Synopsis:
     (spikes_in) 0.5 SetAcceptableLatency -> -

     Parameters: 
     port_name - the name of the MUSIC input port
     latency   - the acceptable latency (ms) to set for the port

     Author: Jochen Martin Eppler
     FirstVersion: April 2009
     Availability: Only when compiled with MUSIC
     SeeAlso: music_event_in_proxy
  */  
  void NestModule::SetAcceptableLatencyFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(2);
     
    std::string port_name = getValue<std::string>(i->OStack.pick(1));
    double latency = getValue<double>(i->OStack.pick(0));

    get_network().set_music_in_port_acceptable_latency(port_name, latency);

    i->OStack.pop(2);
    i->EStack.pop();
  }
#endif

#ifdef HAVE_GSL
  void NestModule::RPopulationConnect_ia_ia_i_d_lFunction::execute(SLIInterpreter *i) const
  {
    i->assert_stack_load(5);

    TokenArray source_adr = getValue<TokenArray>(i->OStack.pick(4));
    TokenArray target_adr = getValue<TokenArray>(i->OStack.pick(3));
    ulong_t N = (ulong_t) getValue<long_t>(i->OStack.pick(2));
    DictionaryDatum param_dict = getValue<DictionaryDatum>(i->OStack.pick(1));

    const Name synmodel_name = getValue<std::string>(i->OStack.pick(0));
    const Token synmodel = get_network().get_synapsedict().lookup(synmodel_name);
    if ( synmodel.empty() )
      throw UnknownSynapseType(synmodel_name.toString());
    const index synmodel_id = static_cast<index>(synmodel);

    int_t M = Communicator::get_num_virtual_processes();
    int_t proc = Communicator::get_rank();

    long_t size_sources = source_adr.size();
    long_t size_targets = target_adr.size();
    long_t last_gid_sources = source_adr[size_sources-1];
    long_t last_gid_targets = target_adr[size_targets-1];
    long_t first_gid_sources = last_gid_sources - size_sources + 1;
    long_t first_gid_targets = last_gid_targets - size_targets + 1;

    uint_t offset_targets = first_gid_targets % M;

    // get switches
    bool static_type = param_dict->known("static_type") ? getValue<bool>(param_dict, "static_type") : false;
    bool tsodyks_type = param_dict->known("tsodyks_type") ? getValue<bool>(param_dict, "tsodyks_type") : false;
    bool distribute_weights = param_dict->known("distribute_weights") ? getValue<bool>(param_dict, "distribute_weights") : false;
    bool distribute_delays = param_dict->known("distribute_delays") ? getValue<bool>(param_dict, "distribute_delays") : false;
    bool distribute_stp_params = param_dict->known("distribute_stp_params") ? getValue<bool>(param_dict, "distribute_stp_params") : false;
    bool initialize_stp_synapse = param_dict->known("initialize_stp_synapse") ? getValue<bool>(param_dict, "initialize_stp_synapse") : false;
    bool distribute_stp_synapse_initialization = param_dict->known("distribute_stp_synapse_initialization") ? getValue<bool>(param_dict, "distribute_stp_synapse_initialization") : false;
    //bool num_targets_per_source = param_dict->known("num_targets_per_source") ? getValue<bool>(param_dict, "num_targets_per_source") : false;
    bool num_sources_per_target = param_dict->known("num_sources_per_target") ? getValue<bool>(param_dict, "num_sources_per_target") : false;
    bool num_syn_per_conn = param_dict->known("num_syn_per_conn") ? getValue<bool>(param_dict, "num_syn_per_conn") : false;
    bool exact_multinomial = param_dict->known("exact_multinomial") ? getValue<bool>(param_dict, "exact_multinomial") : true;

    if ((!static_type) && (!tsodyks_type))
    {
      i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "static_type or tsodyks_type have to be set to true");
      throw BadProperty();
    }

    double_t res = Time::get_resolution().get_ms();

    // extraction of parameters from param_dict
    // 1. check for setting of all required parameters
    // 2. declaration and default initialization
    // 3. extraction

    // 1. check for required dictionary elements
    try
    {
      getValue<double_t>(param_dict, "weight_m");
      if (getValue<double_t>(param_dict, "delay_m") < res)
      {
        i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "delay_m specified to be smaller than resolution.");
        return;
      }
      if (distribute_weights)
        getValue<double_t>(param_dict, "weight_s");
      if (distribute_delays)
        getValue<double_t>(param_dict, "delay_s");
    }
    catch (DictError &e)
    {
      i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "One or more necessary parameters are not specified in param_dict. See documentation for list of necessary parameters.");
      i->raiseerror(e.what());
      return;
    }

    // 2. declarations and default value initialization
    double_t weight;
    double_t weight_m;
    double_t weight_s;
    bool insert_weights;
    double_t delay;
    double_t delay_m;
    double_t delay_s;
    bool insert_delays;
    double_t U;
    double_t tau_rec;
    double_t tau_fac;
    double_t tau_psc;
    double_t U_m;
    double_t tau_rec_m;
    double_t tau_fac_m;
    double_t U_s;
    double_t tau_rec_s;
    double_t tau_fac_s;
    bool insert_U;
    bool insert_tau_rec;
    bool insert_tau_fac;
    bool insert_tau_psc;
    double_t u;
    double_t x;
    double_t y;
    double_t u_m;
    double_t x_m;
    double_t y_m;
    double_t u_s;
    double_t x_s;
    double_t y_s;
    bool insert_u;
    bool insert_xy;
    //     ostreamPtr os_ntps;
    //ostreamPtr os_nspt;
    //ostreamPtr os_nspc;
    //std::ofstream os_ntps;
    std::ofstream os_nspt;
    std::ofstream os_nspc;
    std::ostringstream filename;
    std::ostringstream filename_mul;
    std::vector<ulong_t> nspt;
    std::vector<ulong_t> nspc;
    ulong_t nspc_samples;

    weight_m = 0.0;
    weight_s = 0.0;
    delay_m = res;
    delay_s = 0.0;
    U_m = 0.0;
    U_s = 0.0;
    tau_rec_m = 0.0;
    tau_rec_s = 0.0;
    tau_fac_m = 0.0;
    tau_fac_s = 0.0;
    tau_psc = 0.0;
    u_m = 0.0;
    u_s = 0.0;
    x_m = 0.0;
    x_s = 0.0;
    y_m = 0.0;
    y_s = 0.0;
    insert_U = false;
    insert_tau_rec = false;
    insert_tau_fac = false;
    insert_tau_psc = false;
    insert_u = false;
    insert_xy = false;

    // 3. extraction of parameters from dictionary param_dict
    if ( param_dict->known("weight_m") )
    {
      insert_weights = true;
      if ( distribute_weights )
      {
        weight_m = getValue<double_t>(param_dict, "weight_m");
        weight_s = param_dict->known("weight_s") ? getValue<double_t>(param_dict, "weight_s") : 0.0;
      }
      else
        weight = getValue<double_t>(param_dict, "weight_m");
    }
    else
      insert_weights = false;

    if ( param_dict->known("delay_m") )
    {
      insert_delays = true;
      if ( distribute_delays )
      {
        delay_m = getValue<double_t>(param_dict, "delay_m");
        delay_s = param_dict->known("delay_s") ? getValue<double_t>(param_dict, "delay_s") : 0.0;
      }
      else
        delay = getValue<double_t>(param_dict, "delay_m");
    }
    else
      insert_delays = false;

    if (tsodyks_type)
    {
      insert_U = param_dict->known("U_m");
      insert_tau_rec = param_dict->known("tau_rec_m");
      insert_tau_fac = param_dict->known("tau_fac_m");
      insert_tau_psc = param_dict->known("tau_psc");
      if ( insert_U )
      {
        U_m = getValue<double_t>(param_dict, "U_m");
        U_s = param_dict->known("U_s") ? getValue<double_t>(param_dict, "U_s") : 0.0;
      }
      if ( insert_tau_rec )
      {
        tau_rec_m = getValue<double_t>(param_dict, "tau_rec_m");
        tau_rec_s = param_dict->known("tau_rec_s") ? getValue<double_t>(param_dict, "tau_rec_s") : 0.0;
      }
      if ( insert_tau_fac )
      {
        tau_fac_m = getValue<double_t>(param_dict, "tau_fac_m");
        tau_fac_s = param_dict->known("tau_fac_s") ? getValue<double_t>(param_dict, "tau_fac_s") : 0.0;
      }
      if ( insert_tau_psc )
        tau_psc = getValue<double_t>(param_dict, "tau_psc");

      if (initialize_stp_synapse)
      {
        insert_u = param_dict->known("u_m");
        insert_xy = (param_dict->known("x_m") && param_dict->known("y_m"));

        if ( insert_u )
        {
          u_m = getValue<double_t>(param_dict, "u_m");
          u_s = param_dict->known("u_s") ? getValue<double_t>(param_dict, "u_s") : 0.0; // 1.0?
        }
        if ( insert_xy )
        {
          x_m = getValue<double_t>(param_dict, "x_m");
          x_s = param_dict->known("x_s") ? getValue<double_t>(param_dict, "x_s") : 0.0; // 1.0?
          y_m = getValue<double_t>(param_dict, "y_m");
          y_s = param_dict->known("y_s") ? getValue<double_t>(param_dict, "y_s") : 0.0;
        }
      }
    }

    // drawing connection ids

    // unnormalized distributions of nodes on processes will be used
    // as probability distributions for drawing partitions of
    // connections for virtual processes from multinomial
    // distribution
    std::vector<ulong_t> target_distribution(size_targets, size_targets/M);
    // a generalization to create target_distribution would be a
    // function called locate_neurons that might be expanded to usage
    // of arrays as populations for now, we create the
    // target_distribution here
    int_t k;
    // correction for nodes not uniformly distributed
    // due to size_population%num_processes!=0
    for( k = offset_targets; k < (int_t) ( size_targets % M ) + (int_t) offset_targets; ++k )
    {
      target_distribution[k]++;
    }

    // first_gid_targets_distribution contains at position vp the
    // first (target) gid of all (target) gids belonging to that
    // virtual process. This will be used to map later random numbers
    // to local target gids.
    std::vector<ulong_t> first_gid_targets_distribution(M, 0);
    for( k = 0; k < M; ++k )
    {
      // fill with correct values
      first_gid_targets_distribution[ ( first_gid_targets + k ) % M ] = first_gid_targets + k;
    }

    // We use the multinomial distribution to determine the number of
    // connections that will be made on one virtual process, i.e. we
    // partition the set of edges into n_vps subsets. The number of
    // edges on one virtual process is binomially distributed with
    // the boundary condition that the sum of all edges over virtual
    // processes is the total number of processes.
    // To obtain the target_partitioning we adapt the gsl
    // implementation of the multinomial distribution.

    // K from gsl is equivalent to M = n_vps
    // N is already taken from stack
    // p[] is target_distribution
    std::vector<ulong_t> target_partitioning; // corresponds to n[]

    // calculate exact multinomial distribution
    if (exact_multinomial)
    {
      // get global rng that is tested for synchronization for all threads
      librandom::RngPtr grng = get_network().get_grng();

      // begin code adapted from gsl 1.8 //
      double_t sum_dist = 0.0; // corresponds to sum_p
      //norm is equivalent to size_targets
      uint_t sum_partitions = 0; // corresponds to sum_n

      for ( k = 0; k < M; k++ )
      {
        if (target_distribution[k] > 0)
        {
          // substituting gsl_ran call
          librandom::GSL_BinomialRandomDev bino(grng, ((double_t) target_distribution[k] ) / (size_targets - sum_dist), N - sum_partitions);
          target_partitioning.push_back(bino.uldev());
        }
        else
        {
          target_partitioning.push_back(0);
        }

        sum_dist += (double_t) target_distribution[k];
        sum_partitions += (uint_t) target_partitioning[k];
      }
      // end code adapted from gsl 1.8
    }
    else
    {
      for ( k = 0; k < M; k++ )
      {
        target_partitioning.push_back(0);
      }
    }
    // rng for local vp random numbers
    librandom::RngPtr rng;

    // loop over local threads on one machine
    int_t p;
    for (p = 0; p < M / Communicator::get_num_processes(); p++)
    {
      // current vp corresponding to proc
      int_t j;

      j = p * Communicator::get_num_processes() + proc;

      if(get_network().is_local_vp(j))
      {
        rng = get_network().get_rng(get_network().vp_to_thread(j));
        // normal deviate for parameter grabbing
        librandom::NormalRandomDev norm(rng);

        // if not exact multinomial mode, then calculate here number of neuron
        // on this process
        if (!exact_multinomial)
        {
          if (target_distribution[j] > 0)
          {
            librandom::GSL_BinomialRandomDev bino(rng, ((double_t) target_distribution[j] ) / size_targets, N);
            target_partitioning[j] = bino.uldev();
          }
        }

        // prepare filenames and initialize vectors for graph property readouts
        if (num_sources_per_target)
        {
          filename << getValue<std::string>(param_dict, "num_sources_per_target_fstem") << "-" << Communicator::get_rank() << "-" << p << ".dat";
          //os_nspt = ostreamPtr(new ofdstream(filename.str().c_str()));
          os_nspt.open(filename.str().c_str());
          if ( !os_nspt.good() )
          {
            Node::network()->message(SLIInterpreter::M_ERROR,
       			    "RandomPopulationConnect",
       			    "I/O error while opening file " + filename.str());
            if ( os_nspt.is_open() )
              os_nspt.close();
            filename.str().clear();
            throw IOError();
          }

          nspt = vector<ulong_t>(target_distribution[j]);
        }
        if (num_syn_per_conn)
        {
          filename_mul << getValue<std::string>(param_dict, "num_syn_per_conn_fstem") << "-" << Communicator::get_rank() << "-" << p << ".dat";
          nspc_samples = param_dict->known("nspc_samples") ? getValue<long_t>(param_dict, "nspc_samples") : 1;
          //os_nspc = ostreamPtr(new ofdstream(filename_mul.str().c_str()));
          os_nspc.open(filename.str().c_str());
          if ( !os_nspc.good() )
          {
            Node::network()->message(SLIInterpreter::M_ERROR,
       			      "RandomPopulationConnect",
       			      "I/O error while opening file " + filename.str());
            if ( os_nspc.is_open() )
              os_nspc.close();
            filename.str().clear();
            throw IOError();
          }
          nspc = vector<ulong_t>(nspc_samples * target_distribution[j]);
        }
        else
        {
          nspc_samples = 0;
        }



        while( target_partitioning[j] > 0 )
        {
          // draw random numbers for source node from all source neurons
          long_t s_index  = rng->ulrand(size_sources);
          // draw random numbers for target node from
          // target_distribution on this virtual process
          long_t t_index  = rng->ulrand(target_distribution[j]);

          // use s_index and t_index to fill graph readout variables
          if (num_sources_per_target)
          {
            nspt[t_index]++;
          }
          if (num_syn_per_conn)
          {
            for ( size_t nspc_i = 0; nspc_i < nspc_samples; ++nspc_i )
            {
              if (t_index == s_index + (long_t) nspc_i)
              {
       	 nspc[t_index + nspc_i * target_distribution[j]]++;
              }
            }
          }
          // map random number of source node to gid corresponding to
          // first_gid_sources
          long_t source_gid = first_gid_sources + s_index;
          // map random number of target node to gid using the
          // first_gid_targets_distribution corresponding to the
          // modulo distribution of the nodes on virtual processes
          long_t target_gid = first_gid_targets_distribution[j] + t_index * (long_t) M;

          // local part of connection settings:
          // - draw random number for every non-constant parameter
          // - make all parameters available for connect via substituting
          //   SetSynapseDefaults call

          // define parameters specified in param_dict for this connection
          // define connection parameter dict cpdict
          DictionaryDatum cpdd(new Dictionary());
          if (insert_weights)
          {
            if (distribute_weights)
            {
              do {
       	 weight = norm()*weight_s + weight_m;
              } while ((( weight_m >= 0.0 ) && ( weight < 0.0 )) || (( weight_m < 0.0 ) && ( weight > 0.0 )));
              // weight_m == 0.0 is interpreted as exc. if weights are distributed
            }
            def<double_t>(cpdd, "weight", weight);
          }
          if (insert_delays)
          {
            if (distribute_delays)
            {
              do {
       	 delay = distribute_delays ? round((norm()*delay_s + delay_m)/res)*res : round(delay_m/res)*res;
              } while (delay < 0.0);
              delay = (delay == 0.0) ? res : delay;
            }
            def<double_t>(cpdd, "delay", delay);
          }
          if (tsodyks_type)
          {
            if (distribute_stp_params)
            {
              if (insert_U)
              {
       	 do {
       	   U = U_s > 0.0 ? norm()*U_s + U_m : U_m;
       	 } while (U < 0.0);
       	 def<double_t>(cpdd, "U", U);
              }
              if (insert_tau_rec)
              {
       	 do {
       	   tau_rec = tau_rec_s > 0.0 ? norm()*tau_rec_s + tau_rec_m : tau_rec_m;
       	 } while (tau_rec < 0.0);
       	 def<double_t>(cpdd, "tau_rec", tau_rec);
              }
              if (insert_tau_fac)
              {
       	 do {
       	   tau_fac = tau_fac_s > 0.0 ? norm()*tau_fac_s + tau_fac_m : tau_fac_m;
       	 } while (tau_fac < 0.0);
       	 def<double_t>(cpdd, "tau_fac", tau_fac);
              }
            }
            else // don't distribute_stp_params
            {
              if (insert_U)
              {
       	 U = U_m;
       	 def<double_t>(cpdd, "U", U);
              }
              if (insert_tau_rec)
              {
       	 tau_rec = tau_rec_m;
       	 def<double_t>(cpdd, "tau_rec", tau_rec);
              }
              if (insert_tau_fac)
              {
       	 tau_fac = tau_fac_m;
       	 def<double_t>(cpdd, "tau_fac", tau_fac);
              }
            }
            if (insert_tau_psc) //not distributed for consistency
                                //with neuron model, see warning above
            {
              def<double_t>(cpdd, "tau_psc", tau_psc);
            }
            if (initialize_stp_synapse)
            {
              if (distribute_stp_synapse_initialization)
              {
       	 if (insert_u)
       	 {
       	   u = u_s > 0.0 ? norm()*u_s + u_m : u_m;
       	   do
       	   {
       	     u = norm()*u_s + u_m;
       	   } while ( u < 0.0 || u > 1.0 );
       	   def<double_t>(cpdd, "u", u);
       	 }
       	 if (insert_xy)
       	 {
       	   do
       	   {
       	     do {
       	       x = x_s > 0.0 ? norm()*x_s + x_m : x_m;
       	     } while (x < 0.0);
       	     do {
       	       y = y_s > 0.0 ? norm()*y_s + y_m : y_m;
       	     } while (y < 0.0);
       	   } while ( x + y > 1.0 );
       	   def<double_t>(cpdd, "x", x);
       	   def<double_t>(cpdd, "y", y);
       	 }
              }
              else
              {
       	 if (insert_u)
       	 {
       	   u = u_m;
       	   def<double_t>(cpdd, "u", u);
       	 }
       	 if (insert_xy)
       	 {
       	   x = x_m;
       	   def<double_t>(cpdd, "x", x);
       	   y = y_m;
       	   def<double_t>(cpdd, "y", y);
       	 }
              }
            }
          }

          // connect source and target with dictionary
          try
          {
            get_network().connect(source_gid, target_gid, cpdd, synmodel_id);
          }
          catch (UnknownSynapseType &e)
          {
            i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "Specified synapse type does not exist.");
            i->raiseerror(e.what());
            return;
          }
          catch(BadProperty &e)
          {
            i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "One of the properties does not exist or has wrong type.");
            i->raiseerror(e.what());
            return;
          }
          catch(BadDelay &e)
          {
            i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "The delay has to be greater than or equal to the resolution.");
            i->raiseerror(e.what());
            return;
          }
          catch(DimensionMismatch &e)
          {
            i->message(SLIInterpreter::M_ERROR, "RandomPopulationConnectD", "One of the properties was given with wrong dimensions.");
            i->raiseerror(e.what());
            return;
          }

          target_partitioning[j]--;
        }
        // write graph readout to file
        if (num_sources_per_target)
        {
          for ( size_t nspt_i = 0; nspt_i < nspt.size(); ++nspt_i )
          {
            os_nspt << nspt[nspt_i] << "\n";
          }
        }
        if (num_syn_per_conn)
        {
          std::vector<ulong_t> nspc_hist = std::vector<ulong_t>(0);
          for ( size_t nspc_i = 0; nspc_i < nspc.size(); ++nspc_i )
          {
            while (nspc[nspc_i] > nspc_hist.size())
            {
              nspc_hist.push_back(0);
            }
            nspc_hist[nspc[nspc_i]]++;
          }
          for ( size_t nspc_hist_i = 0; nspc_hist_i < nspc_hist.size(); ++nspc_hist_i )
          {
            os_nspc << nspc_hist_i << "\t" << nspc_hist[nspc_hist_i] << "\n";
            //std::cout << nspc_hist_i << "\t" << nspc_hist[nspc_hist_i] << "\n";
          }
        }
      }
    }

    i->OStack.pop(5);
    i->EStack.pop();
  }
#endif

  void NestModule::init(SLIInterpreter *i)
  {
    ConnectionType.settypename("connectiontype");
    ConnectionType.setdefaultaction(SLIInterpreter::datatypefunction);

    // ensure we have a network: it is created outside and registered via register_network()
    assert(net_ != 0);
     
    // set resolution, ensure clock is calibrated to new resolution
    Time::reset_resolution();
    net_->calibrate_clock();
      
    // register interface functions with interpreter
    i->createcommand("ChangeSubnet",    &changesubnet_ifunction);
    i->createcommand("CurrentSubnet",   &currentsubnetfunction);
    i->createcommand("GetNodes_i_D_b_b",    &getnodes_i_D_b_bfunction);
    i->createcommand("GetLeaves_i_D_b",   &getleaves_i_D_bfunction);
    i->createcommand("GetChildren_i_D_b", &getchildren_i_D_bfunction);

    i->createcommand("RestoreNodes_a", &restorenodes_afunction);

    i->createcommand("SetStatus_id", &setstatus_idfunction);
    i->createcommand("SetStatus_CD", &setstatus_CDfunction);
    i->createcommand("SetStatus_aa", &setstatus_aafunction);

    i->createcommand("GetStatus_i",  &getstatus_ifunction);
    i->createcommand("GetStatus_C",  &getstatus_Cfunction);
    i->createcommand("GetStatus_a",  &getstatus_afunction);

    i->createcommand("GetConnections_D", &getconnections_Dfunction);
    i->createcommand("cva_C", &cva_cfunction);

    i->createcommand("Simulate_d",   &simulatefunction);

    i->createcommand("CopyModel_l_l_D", &copymodel_l_l_Dfunction);
    i->createcommand("SetDefaults_l_D", &setdefaults_l_Dfunction);
    i->createcommand("GetDefaults_l",   &getdefaults_lfunction);
   
    i->createcommand("ResumeSimulation", &resumesimulationfunction);
    i->createcommand("Create_l_i", &create_l_ifunction);
   
    i->createcommand("Connect_i_i_l", &connect_i_i_lfunction);
    i->createcommand("Connect_i_i_i", &connect_i_i_ifunction);
    i->createcommand("Connect_i_i_d_d_l", &connect_i_i_d_d_lfunction);
    i->createcommand("Connect_i_i_d_d_i", &connect_i_i_d_d_ifunction);
    i->createcommand("Connect_i_i_D_l", &connect_i_i_D_lfunction);
    i->createcommand("Connect_i_D_i", &connect_i_D_ifunction);
    i->createcommand("DataConnect_i_dict_s", &dataconnect_i_dict_sfunction);
    i->createcommand("DataConnect_a", &dataconnect_afunction);

    i->createcommand("DivergentConnect_i_ia_a_a_l", &divergentconnect_i_ia_a_a_lfunction);
    i->createcommand("RandomDivergentConnect_i_i_ia_da_da_b_b_l", &rdivergentconnect_i_i_ia_da_da_b_b_lfunction);
    
    i->createcommand("ConvergentConnect_ia_i_a_a_l", &convergentconnect_ia_i_a_a_lfunction);
    i->createcommand("RandomConvergentConnect_ia_i_i_da_da_b_b_l", &rconvergentconnect_ia_i_i_da_da_b_b_lfunction);
    i->createcommand("RandomConvergentConnect_ia_ia_ia_daa_daa_b_b_l", &rconvergentconnect_ia_ia_ia_daa_daa_b_b_lfunction);

#ifdef HAVE_GSL
    i->createcommand("RandomPopulationConnect_ia_ia_i_d_l", &rpopulationconnect_ia_ia_i_d_lfunction);
#endif
   
    i->createcommand("ResetNetwork",&resetnetworkfunction);
    i->createcommand("ResetKernel",&resetkernelfunction);

    i->createcommand("MemoryInfo", &memoryinfofunction);

#if defined IS_BLUEGENE_P || defined IS_BLUEGENE_Q
    i->createcommand("memory_thisjob_bg", &memorythisjobbgfunction);
#endif
   
    i->createcommand("PrintNetwork", &printnetworkfunction);
    
    i->createcommand("Rank", &rankfunction);
    i->createcommand("NumProcesses", &numprocessesfunction);
    i->createcommand("SetFakeNumProcesses", &setfakenumprocesses_ifunction);
    i->createcommand("SyncProcesses", &syncprocessesfunction);
    i->createcommand("TimeCommunication_i_i_b", &timecommunication_i_i_bfunction); 
    i->createcommand("ProcessorName", &processornamefunction);
#ifdef HAVE_MPI
    i->createcommand("MPI_Abort", &mpiabort_ifunction);
#endif
   
    i->createcommand("GetVpRNG", &getvprngfunction);
    i->createcommand("GetGlobalRNG", &getglobalrngfunction);

    i->createcommand("cvdict_C", &cvdict_Cfunction);

#ifdef HAVE_MUSIC     
    i->createcommand("SetAcceptableLatency", &setacceptablelatency_l_dfunction);
#endif
   
    Token statusd = i->baselookup(Name("statusdict"));
    DictionaryDatum dd=getValue<DictionaryDatum>(statusd);
    dd->insert(Name("kernelname"), new StringDatum("NEST"));
    dd->insert(Name("kernelrevision"), new StringDatum("$Revision$"));
    dd->insert(Name("is_mpi"), new BoolDatum(Communicator::get_initialized()));
  }

} // namespace nest
