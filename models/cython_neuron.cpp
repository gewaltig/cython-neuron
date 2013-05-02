/*
 *  cython_neuron.cpp
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

#include "exceptions.h"
#include "cython_neuron.h"
#include "network.h"
#include "dict.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "dictutils.h"
#include "numerics.h"
#include "universal_data_logger_impl.h"
#include "dictstack.h"
#include "buffer.h"

#include <string>
#include <limits>

#include <stdio.h>
/* ----------------------------------------------------------------
 * Recordables map
 * ---------------------------------------------------------------- */

void* CythonEntry::cInit = NULL;
void* CythonEntry::cCalibrate = NULL;
void* CythonEntry::cUpdate = NULL;
void* CythonEntry::cGetStatus = NULL;
void* CythonEntry::cSetStatus = NULL;
void* CythonEntry::cStdVars = NULL;

nest::RecordablesMap<nest::cython_neuron> nest::cython_neuron::recordablesMap_;

namespace nest
{
  // Override the create() method with one call to RecordablesMap::insert_()
  // for each quantity to be recorded.
  template <>
  void RecordablesMap<cython_neuron>::create()
  {
    // use standard names whereever you can for consistency!
    insert_(names::V_m, &cython_neuron::get_V_m_);
  }
}

nest::cython_neuron::Buffers_::Buffers_(cython_neuron &n)
  : logger_(n)
{}

nest::cython_neuron::Buffers_::Buffers_(const Buffers_ &, cython_neuron &n)
  : logger_(n)
{}

/* ----------------------------------------------------------------
 * Default and copy constructor for node
 * ---------------------------------------------------------------- */
nest::cython_neuron::cython_neuron()
  : Archiving_Node(),
    state_(new Dictionary()),
    B_(*this)
{
  // We add empty defaults for /calibrate and /update, so that the uninitialised node runs without errors.
  state_->insert(names::calibrate,new ProcedureDatum());
  state_->insert(names::update,new ProcedureDatum());
  recordablesMap_.create();
  initCython();
}

nest::cython_neuron::cython_neuron(const cython_neuron& n)
  : Archiving_Node(n),
    state_(new Dictionary(*n.state_)),
    B_(n.B_, *this)
{
  init_state_(n);
  initCython();
}

void nest::cython_neuron::initCython()
{
  cythonInit = NULL;
  cythonCalibrate = NULL;
  cythonUpdate = NULL;
  cythonSetStatus = NULL;
  cythonGetStatus = NULL;
  cythonStdVars = NULL;
}

/* ----------------------------------------------------------------
 * Node initialization functions
 * ---------------------------------------------------------------- */

void nest::cython_neuron::init_state_(const Node& proto)
{
  const cython_neuron& pr = downcast<cython_neuron>(proto);
  state_= DictionaryDatum(new Dictionary(*pr.state_));
}

void nest::cython_neuron::init_buffers_()
{
  B_.ex_spikes_.clear();       // includes resize
  B_.in_spikes_.clear();       // includes resize
  B_.currents_.clear();        // includes resize
  B_.logger_.reset(); 	       // includes resize
  Archiving_Node::clear_history();
}



void nest::cython_neuron::calibrate()
{
  B_.logger_.init();

  if(cythonCalibrate == NULL) {
    initSharedObject();
  }


  bool terminate=false;

  if(!state_->known(names::calibrate))
    {
      std::string msg=String::compose("Node %1 has no /calibrate function in its status dictionary.",get_gid());
      net_->message(SLIInterpreter::M_ERROR,"cython_neuron::calibrate",msg.c_str());
      terminate=true;
    }

  if(! state_->known(names::update))
    {
      std::string msg=String::compose("Node %1 has no /update function in its status dictionary. Terminating.",get_gid());
      net_->message(SLIInterpreter::M_ERROR,"cython_neuron::calibrate",msg.c_str());
      terminate=true;
    }

  if(terminate)
    {
      net_->terminate();
      net_->message(SLIInterpreter::M_ERROR,"cython_neuron::calibrate","Terminating.");
      return;
    }

    if(cythonCalibrate != NULL) {
    	cythonCalibrate(get_name(), neuronID,  &state_);   // call shared object
    }
}

/* ----------------------------------------------------------------
 * Update and spike handling functions
 */

void nest::cython_neuron::update(Time const & origin, const long_t from, const long_t to)
{
  assert(to >= 0 && (delay) from < Scheduler::get_min_delay());
  assert(from < to);
  (*state_)[names::t_origin]=origin.get_steps();

  if(cythonUpdate == NULL) {
    initSharedObject();
  }

  std::string name = get_name();

  for ( long_t lag = from ; lag < to ; ++lag )
  {
    (*state_)[names::in_spikes]=B_.in_spikes_.get_value(lag); // in spikes arriving at right border
    (*state_)[names::ex_spikes]=B_.ex_spikes_.get_value(lag); // ex spikes arriving at right border
    (*state_)[names::currents]=B_.currents_.get_value(lag);
    (*state_)[names::t_lag]=lag;


    if(cythonUpdate != NULL) {
    	cythonUpdate(name, neuronID);   // call shared object
    }

    long spike_emission = 0;

    // surely exists
    spike_emission = (*state_)[names::spike];

    // threshold crossing
    if (spike_emission)
    {
      set_spiketime(Time::step(origin.get_steps()+lag+1));
      SpikeEvent se;
      network()->send(*this, se, lag);
    }

    B_.logger_.record_data(origin.get_steps()+lag);
  }
}


void nest::cython_neuron::handle(SpikeEvent & e)
{
  assert(e.get_delay() > 0);

  if(e.get_weight() > 0.0)
    B_.ex_spikes_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()),
                            e.get_weight() * e.get_multiplicity() );
  else
    B_.in_spikes_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()),
                            e.get_weight() * e.get_multiplicity() );
}

void nest::cython_neuron::handle(CurrentEvent& e)
{
  assert(e.get_delay() > 0);

  const double_t I = e.get_current();
  const double_t w = e.get_weight();

  // add weighted current; HEP 2002-10-04
  B_.currents_.add_value(e.get_rel_delivery_steps(network()->get_slice_origin()),
		                     w * I);
}

void nest::cython_neuron::handle(DataLoggingRequest& e)
{
  B_.logger_.handle(e);
}

void nest::cython_neuron::setStatusCython()
{
    if(cythonSetStatus != NULL) {
    	cythonSetStatus(get_name(), neuronID, &state_);   // call shared object
    }
}

void nest::cython_neuron::getStatusCython() const
{
    if(cythonGetStatus != NULL) {
    	cythonGetStatus(get_name(), neuronID, &state_);   // call shared object
    }
}

// This method retrieves the pointer to the cython entry point and calls the special initialization method
void nest::cython_neuron::initSharedObject()
{
    CythonEntry cEntry;
    void* resultInit = cEntry.getInit();
    void* resultCalibrate = cEntry.getCalibrate();
    void* resultUpdate = cEntry.getUpdate();
    void* resultSetStatus = cEntry.getSetStatus();
    void* resultGetStatus = cEntry.getGetStatus();
    void* resultStdVars = cEntry.getStdVars();
    
    if(resultInit != NULL && resultCalibrate != NULL && resultUpdate != NULL && resultSetStatus != NULL && resultGetStatus != NULL) {
	cythonInit = (int (*)(std::string, int, Datum*))resultInit;
	cythonCalibrate = (void (*)(std::string, int, Datum*))resultCalibrate;
	cythonUpdate = (void (*)(std::string, int))resultUpdate;
	cythonSetStatus = (void (*)(std::string, int, Datum*))resultSetStatus;
	cythonGetStatus = (void (*)(std::string, int, Datum*))resultGetStatus;
	cythonStdVars = (void (*)(std::string, int, long*, double*, double*, double*, long*))resultStdVars;

	neuronID = cythonInit(get_name(), -1, &state_);

        IntegerDatum* sI = (IntegerDatum*)(*state_)[names::spike].datum();
        DoubleDatum* isD = (DoubleDatum*)(*state_)[names::in_spikes].datum();
	DoubleDatum* esD = (DoubleDatum*)(*state_)[names::ex_spikes].datum();
	DoubleDatum* cD = (DoubleDatum*)(*state_)[names::currents].datum();
	IntegerDatum* lI = (IntegerDatum*)(*state_)[names::t_lag].datum();

	cythonStdVars(get_name(), neuronID, sI->get_p_val(), isD->get_p_val(), esD->get_p_val(), cD->get_p_val(), lI->get_p_val());

	if(neuronID == -1) {
		printf("Error initializating %s\n", get_name().c_str() );
		initCython();
	}
    }
}

