"""
High-level API of PyNEST.

This file defines the user-level functions of NEST's Python interface
by mapping NEST/SLI commands to Python. Please try to follow these
rules:

1. SLI commands have the same name in Python. This means that most
   function names are written in camel case, although the Python
   guidelines suggest to use lower case for funtcion names. However,
   this way, it is easier for users to migrate from SLI to Python and.

2. Nodes are identified by their global IDs (GID) by default.

3. GIDs are always written as lists, e.g. [0], [1,2]

4. Commands that return a GID must return it as list of GID(s).

5. When possible, loops over nodes should be propagated down to the
   SLI level.  This minimizes the number of Python<->SLI conversions
   and increases performance.  Loops in SLI are also faster than in
   Python. 
        
6. If you have a *very* good reason, you may deviate from these guidelines.

Authors: Jochen Eppler, Marc-Oliver Gewaltig, Moritz Helias, Eilif Mueller
"""

import string
import types

# These variables MUST be set by __init__.py right after importing.
# There is no safety net, whatsoever.
nest = sps = spp = sr = None

class NESTError(Exception):
    def __init__(self, msg) :
        Exception.__init__(self, msg)


# -------------------- Helper functions

def is_sequencetype(seq) :
    """Return True if the given object is a sequence type, False else"""
    
    return type(seq) in (tuple, list)


def is_iterabletype(seq) :
    """Return True if the given object is iterable, False else"""

    try:
        i = iter(seq)
    except TypeError:
        return False

    return True


def is_sequence_of_nonneg_ints(seq):
    """Return True if the given object is a list or tuple of ints, False else"""
    return is_sequencetype(seq) and all([type(n) == type(0) and n >= 0 for n in seq])


def raise_if_not_list_of_gids(seq, argname):
    """
    Raise a NestError if seq is not a sequence of ints, otherwise, do nothing.
    The main purpose of this function is to perform a simple check that an
    argument is a potentially valid list of GIDs (ints >= 0).
    """
    if not is_sequence_of_nonneg_ints(seq):
        raise NESTError(argname + " must be a list or tuple of GIDs")
 

def broadcast(val, l, allowedtypes, name="val"):

    if type(val) in allowedtypes:
        return l*(val,)
    elif len(val)==1:
        return l*val
    elif len(val)!=l:
        raise NESTError("'%s' must be a single value, a list with one element or a list with %i elements." % (name, l))

    return val


def flatten(x):
    """flatten(sequence) -> list

    Returns a flat list with all elements from the sequence and all
    contained sub-sequences (iterables).

    Examples:
    >>> [1, 2, [3,4], (5,6)]
    [1, 2, [3, 4], (5, 6)]
    >>> flatten([[[1,2,3], (42,None)], [4,5], [6], 7, MyVector(8,9,10)])
    [1, 2, 3, 42, None, 4, 5, 6, 7, 8, 9, 10]"""

    result = []
    for el in x:
        if hasattr(el, "__iter__") and not isinstance(el, str) and not type(el) == dict:
            result.extend(flatten(el))
        else:
            result.append(el)

    return result


# -------------------- Functions to get information on NEST

def sysinfo():
    """Print information on the platform on which NEST was compiled."""

    sr("sysinfo")


def version():
    """Print the NEST version."""

    sr("statusdict [[ /kernelname /version ]] get")
    return string.join(spp())
    

def authors():
    """Show the authors of NEST."""

    sr("authors")


def helpdesk(browser="firefox"):
    """Open the NEST helpdesk in the given browser."""
    
    sr("/helpdesk << /command (%s) >> SetOptions" % browser)
    sr("helpdesk")


def help(obj=None, pager="less"):
    """Show the help page for the given object"""

    if obj:
        sr("/page << /command (%s) >> SetOptions" % pager)
        sr("/%s help" % obj)
    else:
        print ("Type 'nest.helpdesk()' to access the online documentation in a browser.")
        print ("Type 'nest.help(object)' to get help on a NEST object or command.")
        print ('')
        print ("Type 'nest.Models()' to see a list of available models in NEST.")
        print ('')
        print ("Type 'nest.authors()' for information about the makers of NEST.")
        print ("Type 'nest.sysinfo()' to see details on the system configuration.")
        print ("Type 'nest.version()' for information about the NEST version.")
        print ('')
        print ("For more information visit http://www.nest-initiative.org.")


def get_verbosity():
    """Return verbosity level of NEST's messages."""
    
    sr('verbosity')
    return spp()


def set_verbosity(level):
    """
    Change verbosity level for NEST's messages. level is a string and
    can be one of M_FATAL, M_ERROR, M_WARNING, or M_INFO
    """

    sr("%s setverbosity" % level)


def message(level,sender,text):
    """Print a message using NEST's message system."""

    sps(level)
    sps(sender)
    sps(text)
    sr('message')


# -------------------- Functions for simulation control

def Simulate(t):
    """Simulate the network for t milliseconds."""

    sps(float(t))
    sr('ms Simulate')


def ResumeSimulation():
    """Resume an interrupted simulation."""

    sr("ResumeSimulation")


def ResetKernel():
    """Reset the simulation kernel. This will destroy the network as
    well as all custom models created with CopyModel(). Calling this
    function is equivalent to restarting NEST."""

    sr('ResetKernel')


def ResetNetwork():
    """Reset all nodes and connections to their original state."""

    sr('ResetNetwork')


def SetKernelStatus(params):
    """Set parameters for the simulation kernel."""
    
    sps(0)
    sps(params)
    sr('SetStatus')


def GetKernelStatus(keys = None):
    """
    Obtain parameters of the simulation kernel.

    Returns:
    - Parameter dictionary if called without argument
    - Single parameter value if called with single parameter name
    - List of parameter values if called with list of parameter names
    """
    
    sr('0 GetStatus')
    rootstatus = spp()

    sr('/subnet GetDefaults')
    subnetdefaults = spp()

    subnetdefaults["frozen"] = None
    subnetdefaults["global_id"] = None
    subnetdefaults["local"] = None
    subnetdefaults["local_id"] = None
    subnetdefaults["parent"] = None
    subnetdefaults["state"] = None
    subnetdefaults["thread"] = None
    subnetdefaults["vp"] = None

    d = dict()

    for k in rootstatus :
        if k not in subnetdefaults :
            d[k] = rootstatus[k]

    if not keys:
        return d
    elif is_sequencetype(keys):
        return [d[k] for k in keys]
    else:
        return d[keys]

def Install(module_name):
    """
    Load a dynamically linked NEST module.

    Example:
    nest.Install("mymodule")

    Returns:
    NEST module identifier, required for unloading.

    Note:
    Dynamically linked modules are search in the LD_LIBRARY_PATH
    (DYLD_LIBRARY_PATH under OSX). 
    """

    return sr("(%s) Install" % module_name)


# -------------------- Functions for parallel computing

def Rank():
    """Return the MPI rank of the local process."""

    sr("Rank")
    return spp()

def NumProcesses():
    """Return the overall number of MPI processes."""

    sr("NumProcesses")
    return spp()

def SetAcceptableLatency(port, latency):
    """Set the acceptable latency (in ms) for a MUSIC port."""
    
    sps(latency)
    sr("/%s exch SetAcceptableLatency" % port)


# -------------------- Functions for model handling

def Models(mtype = "all", sel=None):
    """Return a list of all available models (neurons, devices and
    synapses). Use mtype='nodes' to only see neuron and device models,
    mtype='synapses' to only see synapse models. sel can be a string,
    used to filter the result list and only return models containing
    it."""

    if mtype not in ("all", "nodes", "synapses"):
        raise NESTError("type has to be one of 'all', 'nodes' or 'synapses'.")

    models = list()

    if mtype in ("all", "nodes"):
       sr("modeldict")
       models += list(spp().keys())

    if mtype in ("all", "synapses"):
       sr("synapsedict")
       models += list(spp().keys())
    
    if sel != None:
        models = [x for x in models if x.find(sel) >= 0]

    models.sort()
    return models


def SetDefaults(model, params) :
    """Set the default parameters of the given model to the values
    specified in the params dictionary."""

    sps(params)
    sr('/%s exch SetDefaults' % model)


def GetDefaults(model) :
    """Return a dictionary with the default parameters of the given
    model, specified by a string."""
    
    sr("/%s GetDefaults" % model)
    return spp()


def CopyModel(existing, new, params=None):
    """ Create a new model by copying an existing one. Default
    parameters can be given as params, or else are taken from
    existing."""
    
    if params:
        sps(params)
        sr("/%s /%s 3 2 roll CopyModel" % (existing, new))
    else:
        sr("/%s /%s CopyModel" % (existing, new))


# -------------------- Functions for node handling

def Create(model, n=1, params=None):
    """Create n instances of type model. Parameters for the new nodes
    can are given as params (a single dictionary or a list of
    dictionaries with size n). If omitted, the model's defaults are
    used."""

    broadcast_params = False

    sps(n)
    cmd = "/%s exch Create" % model

    if params:
        if type(params) == dict:
            sps(params)
            cmd = "/%s 3 1 roll Create" % model
        elif is_sequencetype(params) and (len(params) == 1 or len(params) == n):
            broadcast_params = True
        else:
            sr(";") # pop n from the stack
            raise NESTError("params has to be a single dictionary or a list of dictionaries with size n.")
        
    sr(cmd)

    lastgid = spp()
    ids = list(range(lastgid - n + 1, lastgid + 1))

    if broadcast_params:
        try:
            SetStatus(ids, broadcast(params, n, (dict,)))
        except:
            raise NESTError("SetStatus failed, but nodes already have been created. The ids of the new nodes are: %s" % ids)

    return ids

        
def SetStatus(nodes, params, val=None) :
    """ Set the parameters of nodes (identified by global ids)
    or connections (identified by handles as returned by
    FindConnections()) to params, which may be a single dictionary or
    a list of dictionaries. If val is given, params has to be the name
    of an attribute, which is set to val on the nodes/connections. val
    can be a single value or a list of the same size as nodes."""

    if not is_sequencetype(nodes):
        raise NESTError("nodes must be a list of nodes or synapses.")

    if len(nodes) == 0:
        return

    if type(params) == bytes :
        if is_iterabletype(val) and not type(val) in (bytes, dict):
            params = [{params : x} for x in val]
        else :
            params = {params : val}

    params = broadcast(params, len(nodes), (dict,), "params")
    if len(nodes) != len(params) :
        raise NESTError("Status dict must be a dict, or list of dicts of length 1 or len(nodes).")
        
    if type(nodes[0]) == dict:
        nest.engine.push_connections(nodes)
    else:
        sps(nodes)

    sps(params)
    sr('2 arraystore')
    sr('Transpose { arrayload ; SetStatus } forall')

def GetStatus(nodes, keys=None) :
    """Return the parameter dictionaries of the given list of nodes
    (identified by global ids) or connections (identified
    by handles as returned by FindConnections()). If keys is given, a
    list of values is returned instead. keys may also be a list, in
    which case the returned list contains lists of values."""

    if not is_sequencetype(nodes):
        raise NESTError("nodes must be a list of nodes or synapses.")

    if len(nodes) == 0:
        return nodes

    cmd='{ GetStatus } Map'

    if keys:
        if is_sequencetype(keys):
            keyss = string.join(["/%s" % x for x in keys])
            cmd='{ GetStatus } Map { [ [ %s ] ] get } Map' % keyss
        else:
            cmd='{ GetStatus /%s get} Map' % keys

    if type(nodes[0]) == dict:
        nest.engine.push_connections(nodes)
    else:
        sps(nodes)
   
    sr(cmd)

    return spp()


def GetLID(gid) :
    """
    Return the local id of a node with gid.
    GetLID(gid) -> lid
    """
    if len(gid) > 1:
        raise NESTError("Can only return the local ID of one node.")
    sps(gid[0])
    sr("GetLID")

    return spp()


# -------------------- Functions for connection handling

def FindConnections(source, target=None, synapse_type=None) :
    """Return an array of identifiers for connections that match the
    given parameters. Only source is mandatory and must be a list of
    one or more nodes. If target and/or synapse_type is/are given,
    they must be single values, lists of length one or the same length
    as source. Use GetStatus()/SetStatus() to inspect/modify the found
    connections."""

    if not target and not synapse_type:
        params = [{"source": s} for s in source]

    if not target and synapse_type:
        synapse_type = broadcast(synapse_type, len(source), (str,), "synapse_type")
        params = [{"source": s, "synapse_type": syn} for s, syn in zip(source, synapse_type)]

    if target and not synapse_type:
        target = broadcast(target, len(source), (int,), "target")
        params = [{"source": s, "target": t} for s, t in zip(source, target)]

    if target and synapse_type:
        target = broadcast(target, len(source), (int,), "target")
        synapse_type = broadcast(synapse_type, len(source), (str,), "synapse_type")
        params = [{"source": s, "target": t, "synapse_type": syn} for s, t, syn in zip(source, target, synapse_type)]

    sps(params)
    sr("{FindConnections} Map" % params)
    
    return flatten(spp())


def GetConnections(source=None, target=None, synapse_type=None) :
	"""
	Return an array of connection identifiers.
	
	Parameters:
        source - list of source GIDs
        target - list of target GIDs
        synapse_type - string with the synapse model type
        
        If GetConnections is called without parameters, all GIDs and all synapse types are iterated.

        If a synapse type is given, the return value is the list of connection ids for this type.
	If no synapse model is given, GetConnections will iterate all used synapse types and the return value will
        be a nested list, where each sublist contains the connection ids of the respective synapse model.
        
        
        Each connection id is a 5 tuple or, if available, a numpy array with 5 entries.
	
        Use GetSynapseStatus()/SetSynapseStatus() to inspect/modify the found
	connections.
	"""
	
	params={}
	if source:
		if not is_sequencetype(source):
			raise NESTError("source must be a list of gids.")
		params['source']=source
	if target:
		if not is_sequencetype(target):
			raise NESTError("target must be a list of gids.")
		params['target']=target

	if synapse_type:
		params['synapse_type']=synapse_type

	sps(params)
	sr("GetConnections_D")
    
	return spp()

def SetSynapseStatus(conn_ids, params, val=None):
    """
    Modify the connections given in conn_ids.
    conn_ids must be an array with connection ids where
    each connection id is a 5-tuple with the following anatomy
    (source_gid, target_gid, target_thread, synapse_id, port).
    params must be an array of dictionaries, one for each connection.
    connection ids are returned by GetConnections().
    
    This function is still experimental.
    Author: Marc-Oliver Gewaltig 
    """
    if not is_sequencetype(conn_ids):
        raise NESTError("conn_ids must be a list of connection ids.")

    if len(conn_ids) == 0:
        return

    if type(params) == types.StringType :
        if is_iterabletype(val) and not type(val) in (types.StringType, types.DictType):
            params = [{params : x} for x in val]
        else :
            params = {params : val}
            
        params = broadcast(params, len(conn_ids), (dict,), "params")
        if len(conn_ids) != len(params) :
            raise NESTError("Status dict must be a dict, or list of dicts of length 1 or len(conn_ids).")

        nest.engine.push_connections(conn_ids)
        sps(params)
        sr('SetStatus_aa')


def GetSynapseStatus(conn_ids, keys=None) :
    """
    Return the parameter dictionaries of the given list of connections
    (identified by handles as returned by GetConnections()). If keys is given, a
    list of values is returned instead. keys may also be a list, in
    which case the returned list contains lists of values.
    
    This function is still experimental.
    Author: Marc-Oliver Gewaltig 
    """

    if not is_sequencetype(conn_ids):
        raise NESTError("conn_ids must be a list of connections.")

    if len(conn_ids) == 0:
        return []

    cmd='GetStatus_a'

    if keys:
        if is_sequencetype(keys):
            keyss = string.join(["/%s" % x for x in keys])
            cmd='GetStatus_a  { [ [ %s ] ] get } Map' % keyss
        else:
            cmd='GetStatus_a { /%s get} Map' % keys

    nest.engine.push_connections(conn_ids)
    sr(cmd)
    return spp()

def Connect(pre, post, params=None, delay=None, model="static_synapse"):
    """Make one-to-one connections of type model between the nodes in
    pre and the nodes in post. pre and post have to be lists of the
    same length. If params is given (as dictionary or list of
    dictionaries), they are used as parameters for the connections. If
    params is given as a single float or as list of floats, it is used
    as weight(s), in which case delay also has to be given as float or
    as list of floats."""

    if len(pre) != len(post):
        raise NESTError("pre and post have to be the same length")

    # pre post Connect
    if params == None and delay == None:
        for s,d in zip(pre, post):
            sps(s)
            sps(d)
            sr('/%s Connect' % model)

    # pre post params Connect
    elif params != None and delay == None:
        params = broadcast(params, len(pre), (dict,), "params")
        if len(params) != len(pre):
            raise NESTError("params must be a dict, or list of dicts of length 1 or len(pre).")

        for s,d,p in zip(pre, post, params) :
            sps(s)
            sps(d)
            sps(p)
            sr('/%s Connect' % model)

    # pre post w d Connect
    elif params != None and delay != None:
        params = broadcast(params, len(pre), (float,), "params")
        if len(params) != len(pre):
            raise NESTError("params must be a float, or list of floats of length 1 or len(pre) and will be used as weight(s).")
        delay = broadcast(delay, len(pre), (float,), "delay")
        if len(delay) != len(pre):
            raise NESTError("delay must be a float, or list of floats of length 1 or len(pre).")

        for s,d,w,dl in zip(pre, post, params, delay) :
            sps(s)
            sps(d)
            sps(w)
            sps(dl)
            sr('/%s Connect' % model)

    else:
        raise NESTError("Both 'params' and 'delay' have to be given.")


def ConvergentConnect(pre, post, weight=None, delay=None, model="static_synapse"):
    """Connect all neurons in pre to each neuron in post. pre and post
    have to be lists. If weight is given (as a single float or as list
    of floats), delay also has to be given as float or as list of
    floats."""

    if weight == None and delay == None:
        for d in post :
            sps(pre)
            sps(d)
            sr('/%s ConvergentConnect' % model)

    elif weight != None and delay != None:
        weight = broadcast(weight, len(pre), (float,), "weight")
        if len(weight) != len(pre):
            raise NESTError("weight must be a float, or sequence of floats of length 1 or len(pre)")
        delay = broadcast(delay, len(pre), (float,), "delay")
        if len(delay) != len(pre):
            raise NESTError("delay must be a float, or sequence of floats of length 1 or len(pre)")
        
        for d in post:
            sps(pre)
            sps(d)
            sps(weight)
            sps(delay)
            sr('/%s ConvergentConnect' % model)

    else:
        raise NESTError("Both 'weight' and 'delay' have to be given.")


def RandomConvergentConnect(pre, post, n, weight=None, delay=None,
                            model="static_synapse", options=None):
    
    """Connect n randomly selected neurons from pre to each neuron in
    post. pre and post have to be lists. If weight is given (as a
    single float or as list of floats), delay also has to be given as
    float or as list of floats. options is a dictionary specifying
    options to the RandomConvergentConnect function: allow_autapses,
    allow_multapses.
    """

    # store current options, set desired options
    old_options = None
    error = False
    if options:
        old_options = sli_func('GetOptions', '/RandomConvergentConnect',
                               litconv=True)
        del old_options['DefaultOptions'] # in the way when restoring
        sli_func('SetOptions', '/RandomConvergentConnect', options,
                 litconv=True)

    if weight == None and delay == None:
        sli_func(
            '/m Set /n Set /pre Set { pre exch n m RandomConvergentConnect } forall',
            post, pre, n, '/'+model, litconv=True)
    
    elif weight != None and delay != None:
        weight = broadcast(weight, n, (float,), "weight")
        if len(weight) != n:
            raise NESTError("weight must be a float, or sequence of floats of length 1 or n")
        delay = broadcast(delay, n, (float,), "delay")
        if len(delay) != n:
            raise NESTError("delay must be a float, or sequence of floats of length 1 or n")

        sli_func(
            '/m Set /d Set /w Set /n Set /pre Set { pre exch n w d m RandomConvergentConnect } forall',
            post, pre, n, weight, delay, '/'+model, litconv=True)
    
    else:
        error = True

    # restore old options
    if old_options:
        sli_func('SetOptions', '/RandomConvergentConnect', old_options,
                 litconv=True)

    if error:
        raise NESTError("Both 'weight' and 'delay' have to be given.")


def DivergentConnect(pre, post, weight=None, delay=None, model="static_synapse"):
    """Connect each neuron in pre to all neurons in post. pre and post
    have to be lists. If weight is given (as a single float or as list
    of floats), delay also has to be given as float or as list of
    floats."""

    if weight == None and delay == None:
        for s in pre :
            sps(s)
            sps(post)
            sr('/%s DivergentConnect' % model)

    elif weight != None and delay != None:
        weight = broadcast(weight, len(post), (float,), "weight")
        if len(weight) != len(post):
            raise NESTError("weight must be a float, or sequence of floats of length 1 or len(post)")
        delay = broadcast(delay, len(post), (float,), "delay")
        if len(delay) != len(post):
            raise NESTError("delay must be a float, or sequence of floats of length 1 or len(post)")
        cmd='/%s DivergentConnect' % model
        for s in pre :
            sps(s)
            sps(post)
            sps(weight)
            sps(delay)
            sr(cmd)
    
    else:
        raise NESTError("Both 'weight' and 'delay' have to be given.")

def DataConnect(pre, params, model="static_synapse"):
    """
    Connect each neuron in pre according to the data in {params}.
    params is a list of dictionaries, each containing at least the keys:
    'target': [t1,...,tn]
    'weight': [w1,...,wn]
    'delay':[d1,...,dn]
    The parameter lists should be numpy float arrays.
    Otherwise, they will be converted, which takes time.
    """
    if not is_sequencetype(pre):
        raise NESTError("'pre' must be a list of nodes.")
    if not is_sequencetype(params):
        raise NESTError("'params' must be a list of dictionaries.")
    cmd='/%s DataConnect_' % model
    
    for s,p in zip(pre,params):
        sps(s)
        sps(p)
        sr(cmd)
    
def RandomDivergentConnect(pre, post, n, weight=None, delay=None,
                           model="static_synapse", options=None):
    """Connect each neuron in pre to n randomly selected neurons from
    post. pre and post have to be lists. If weight is given (as a
    single float or as list of floats), delay also has to be given as
    float or as list of floats. options is a dictionary specifying
    options to the RandomDivergentConnect function: allow_autapses,
    allow_multapses.
    """
    
    # store current options, set desired options
    old_options = None
    error = False
    if options:
        old_options = sli_func('GetOptions', '/RandomDivergentConnect',
                               litconv=True)
        del old_options['DefaultOptions'] # in the way when restoring
        sli_func('SetOptions', '/RandomDivergentConnect', options,
                 litconv=True)

    if weight == None and delay == None:
        sli_func(
            '/m Set /n Set /post Set { n post m RandomDivergentConnect } forall',
            pre, post, n, '/'+model, litconv=True)

    elif weight != None and delay != None:
        weight = broadcast(weight, n, (float,), "weight")
        if len(weight) != n:
            raise NESTError("weight must be a float, or sequence of floats of length 1 or n")
        delay = broadcast(delay, n, (float,), "delay")
        if len(delay) != n:
            raise NESTError("delay must be a float, or sequence of floats of length 1 or n")

        sli_func(
            '/m Set /d Set /w Set /n Set /post Set { n post w d m RandomDivergentConnect } forall',
            pre, post, n, weight, delay, '/'+model, litconv=True)

    else:
        error = True

    # restore old options
    if old_options:
        sli_func('SetOptions', '/RandomDivergentConnect', old_options,
                 litconv=True)

    if error:
        raise NESTError("Both 'weight' and 'delay' have to be given.")


# -------------------- Functions for hierarchical networks

def PrintNetwork(depth=1, subnet=None) :
    """Print the network tree up to depth, starting at subnet. if
    subnet is omitted, the current subnet is used instead."""
    
    if subnet == None:
        subnet = CurrentSubnet()

    sps(subnet[0])
    sr("%i PrintNetwork" % depth)


def CurrentSubnet() :
    """Returns the global id of the current subnet."""

    sr("CurrentSubnet")
    return [spp()]


def ChangeSubnet(subnet) :
    """Make subnet the current subnet."""

    sps(subnet[0])
    sr("ChangeSubnet")


def GetLeaves(subnets, properties=None, local_only=False) :
    """
    Return the global ids of the leaf nodes of the given subnets.
    
    Leaf nodes are all nodes that are not subnets.
    
    If properties is given, it must be a dictionary. Only global ids of nodes 
       matching the properties given in the dictionary exactly will be returned.
       Matching properties with float values (e.g. the membrane potential) may
       fail due to tiny numerical discrepancies and should be avoided.
       
    If local_only is True, only global ids of nodes simulated on the local MPI 
       process will be returned. By default, global ids of nodes in the entire
       simulation will be returned. This requires MPI communication and may
       slow down the script.
       
    See also: GetNodes, GetChildren
    """

    if properties is None:
        properties = {}
    func = 'GetLocalLeaves' if local_only else 'GetGlobalLeaves'
    return sli_func('/props Set { props %s } Map' % func, subnets, properties,
                    litconv=True)    


def GetNodes(subnets, properties=None, local_only=False):
    """
    Return the global ids of the all nodes of the given subnets.
    
    If properties is given, it must be a dictionary. Only global ids of nodes 
       matching the properties given in the dictionary exactly will be returned.
       Matching properties with float values (e.g. the membrane potential) may
       fail due to tiny numerical discrepancies and should be avoided.
       
    If local_only is True, only global ids of nodes simulated on the local MPI 
       process will be returned. By default, global ids of nodes in the entire
       simulation will be returned. This requires MPI communication and may
       slow down the script.
       
    See also: GetLeaves, GetChildren
    """

    if properties is None:
        properties = {}
    func = 'GetLocalNodes' if local_only else 'GetGlobalNodes'
    return sli_func('/props Set { props %s } Map' % func, subnets, properties,
                    litconv=True)    


def GetChildren(subnets, properties=None, local_only=False):
    """
    Return the global ids of the immediate children of the given subnets.
    
    If properties is given, it must be a dictionary. Only global ids of nodes 
       matching the properties given in the dictionary exactly will be returned.
       Matching properties with float values (e.g. the membrane potential) may
       fail due to tiny numerical discrepancies and should be avoided.
       
    If local_only is True, only global ids of nodes simulated on the local MPI 
       process will be returned. By default, global ids of nodes in the entire
       simulation will be returned. This requires MPI communication and may
       slow down the script.
       
    See also: GetNodes, GetLeaves
    """

    if properties is None:
        properties = {}
    func = 'GetLocalChildren' if local_only else 'GetGlobalChildren'
    return sli_func('/props Set { props %s } Map' % func, subnets, properties,
                    litconv=True)    

        
def GetNetwork(gid, depth):
    """Return a nested list with the children of subnet id at level
    depth. If depth==0, the immediate children of the subnet are
    returned. The returned list is depth+1 dimensional.
    """
    
    if len(gid)>1 :
        raise NESTError("GetNetwork() expects exactly one GID.")
    
    sps(gid[0])
    sps(depth)
    sr("GetNetwork")
    return spp()


def BeginSubnet(label=None, params=None):
    """Create a new subnet and change into it.
    A string argument can be used to name the new subnet
    A dictionary argument can be used to set the subnet's custom dict."""

    sn=Create("subnet")
    if label:
        SetStatus(sn, "label", label)
    if params:
        SetStatus(sn, "customdict", params)
    ChangeSubnet(sn)


def EndSubnet():
    """Change to the parent subnet and return the gid of the current."""

    csn=CurrentSubnet()
    parent=GetStatus(csn, "parent")

    if csn != parent:
        ChangeSubnet(parent)
        return csn
    else:
        raise NESTError("Unexpected EndSubnet(). Cannot go higher than the root node.")


def LayoutNetwork(model, dim, label=None, params=None) :
    """Create a subnetwork of dimension dim with nodes of type model
       and return a list of ids."""

    if type(model) == bytes:
        sps(dim)
        sr('/%s exch LayoutNetwork' % model)
        if label:
            sr("dup << /label (%s) >> SetStatus"%label)
        if params:
            sr("dup << /customdict")
            sps(params)
            sr(">> SetStatus")
        return [spp()]

    # If model is a function.
    elif type(model) == types.FunctionType:
        # The following code uses a model constructor function
        # model() instead of a model name string.
        BeginSubnet(label, params)

        if len(dim)==1:
            for i in range(dim[0]):
                model()
        else:
            for i in range(dim[0]):
                LayoutNetwork(model,dim[1:])

        gid = EndSubnet()
        return gid

    else:
        raise NESTError("model must be a string or a function.")