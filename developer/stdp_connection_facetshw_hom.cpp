/*
 *  stdp_connection_facetshw_hom.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "network.h"
#include "dictdatum.h"
#include "connector_model.h"
#include "common_synapse_properties.h"
#include "stdp_connection_facetshw_hom.h"
#include "event.h"

namespace nest
{
  //
  // Implementation of class STDPFACETSHWHomCommonProperties.
  //

  STDPFACETSHWHomCommonProperties::STDPFACETSHWHomCommonProperties() :
    CommonSynapseProperties(),

    tau_plus_(20.0),
    tau_minus_(20.0),

    no_synapses_(0),
    synapses_per_driver_(50),
    driver_readout_time_(36.0), //in ms
    hardware_stage_(3),

    Wmax_(100.0)
  {
    lookuptable_causal_.resize(16);
    lookuptable_acausal_.resize(16);

    //intermediate Guetig (mu=0.4)
    //with r=4 bits and n=36 SSPs
    lookuptable_causal_.at(0) = 2;
    lookuptable_causal_.at(1) = 3;
    lookuptable_causal_.at(2) = 4;
    lookuptable_causal_.at(3) = 4;
    lookuptable_causal_.at(4) = 5;
    lookuptable_causal_.at(5) = 6;
    lookuptable_causal_.at(6) = 7;
    lookuptable_causal_.at(7) = 8;
    lookuptable_causal_.at(8) = 9;
    lookuptable_causal_.at(9) = 10;
    lookuptable_causal_.at(10) = 11;
    lookuptable_causal_.at(11) = 12;
    lookuptable_causal_.at(12) = 13;
    lookuptable_causal_.at(13) = 14;
    lookuptable_causal_.at(14) = 14;
    lookuptable_causal_.at(15) = 15;

    lookuptable_acausal_.at(0) = 0;
    lookuptable_acausal_.at(1) = 0;
    lookuptable_acausal_.at(2) = 1;
    lookuptable_acausal_.at(3) = 2;
    lookuptable_acausal_.at(4) = 3;
    lookuptable_acausal_.at(5) = 4;
    lookuptable_acausal_.at(6) = 5;
    lookuptable_acausal_.at(7) = 6;
    lookuptable_acausal_.at(8) = 7;
    lookuptable_acausal_.at(9) = 8;
    lookuptable_acausal_.at(10) = 9;
    lookuptable_acausal_.at(11) = 10;
    lookuptable_acausal_.at(12) = 10;
    lookuptable_acausal_.at(13) = 11;
    lookuptable_acausal_.at(14) = 12;
    lookuptable_acausal_.at(15) = 13;

    weight_per_lut_entry_ = Wmax_ / (lookuptable_causal_.size() - 1);
    calc_readout_cycle_duration_();
  }

  void STDPFACETSHWHomCommonProperties::calc_readout_cycle_duration_()
  {
    readout_cycle_duration_ = int((no_synapses_ - 1.0) / synapses_per_driver_ + 1.0) * driver_readout_time_;
  }

  void STDPFACETSHWHomCommonProperties::get_status(DictionaryDatum & d) const
  {
    CommonSynapseProperties::get_status(d);

    def<double_t>(d, "tau_plus", tau_plus_);
    def<double_t>(d, "tau_minus_stdp", tau_minus_);
    def<double_t>(d, "Wmax", Wmax_);
    def<double_t>(d, "weight_per_lut_entry", weight_per_lut_entry_);

    def<long_t>(d, "no_synapses", no_synapses_);
    def<long_t>(d, "synapses_per_driver", synapses_per_driver_);
    def<double_t>(d, "driver_readout_time", driver_readout_time_);
    def<double_t>(d, "readout_cycle_duration", readout_cycle_duration_);
    def<long_t>(d, "hardware_stage", hardware_stage_);

    (*d)["lookuptable_causal"] = IntVectorDatum(new std::vector<long_t>(lookuptable_causal_));
    (*d)["lookuptable_acausal"] = IntVectorDatum(new std::vector<long_t>(lookuptable_acausal_));
  }
 
  void STDPFACETSHWHomCommonProperties::set_status(const DictionaryDatum & d, ConnectorModel &cm)
  {
    CommonSynapseProperties::set_status(d, cm);

    updateValue<double_t>(d, "tau_plus", tau_plus_);
    updateValue<double_t>(d, "tau_minus_stdp", tau_minus_);
    if(updateValue<double_t>(d, "Wmax", Wmax_)){
      weight_per_lut_entry_ = Wmax_ / (lookuptable_causal_.size() - 1);
    }

    //they should not be allowed to be changed!
    //updateValue<double_t>(d, "weight_per_lut_entry", weight_per_lut_entry_);
    //updateValue<double_t>(d, "readout_cycle_duration", readout_cycle_duration_);

    if(updateValue<long_t>(d, "no_synapses", no_synapses_)){
      calc_readout_cycle_duration_();
    }
    if(updateValue<long_t>(d, "synapses_per_driver", synapses_per_driver_)){
      calc_readout_cycle_duration_();
    }
    if(updateValue<double_t>(d, "driver_readout_time", driver_readout_time_)){
      calc_readout_cycle_duration_();
    }
    updateValue<long_t>(d, "hardware_stage", hardware_stage_);

    if(d->known("lookuptable_causal")){
      updateValue<std::vector<long_t> >(d, "lookuptable_causal", lookuptable_causal_);

      //right size?
      if(lookuptable_causal_.size() != lookuptable_acausal_.size()){
        throw BadProperty("Look-up table has not 2^4 entries!");
      }

      //are look-up table entries out of bounds?
      for(size_t i = 0; i < size_t(lookuptable_causal_.size()); i++){
        if((lookuptable_causal_[i] < 0) || (lookuptable_causal_[i] > 15)){
          throw BadProperty("Look-up table entries must be integers in [0,15]");
        }
      }
    }
    if(d->known("lookuptable_acausal")){
      updateValue<std::vector<long_t> >(d, "lookuptable_acausal", lookuptable_acausal_);

      //right size?
      if(lookuptable_acausal_.size() != lookuptable_causal_.size()){
        throw BadProperty("Look-up table has not 2^4 entries!");
      }

      //are look-up table entries out of bounds?
      for(size_t i = 0; i < size_t(lookuptable_acausal_.size()); i++){
        if((lookuptable_acausal_[i] < 0) || (lookuptable_acausal_[i] > 15)){
          throw BadProperty("Look-up table entries must be integers in [0,15]");
        }
      }
    }
  }


  //
  // Implementation of class STDPFACETSHWConnectionHom.
  //

  STDPFACETSHWConnectionHom::STDPFACETSHWConnectionHom() :
    a_causal_(0.0),
    a_acausal_(0.0),
    a_th_causal_(21.835), //exp(-10ms/20ms) * 36SSPs
    a_th_acausal_(21.835),

    init_flag_(false),
    synapse_id_(0),
    next_readout_time_(0.0),
    discrete_weight_(0)
  { }

  STDPFACETSHWConnectionHom::STDPFACETSHWConnectionHom(const STDPFACETSHWConnectionHom &rhs) :
    ConnectionHetWD(rhs)
  {
    a_causal_ = rhs.a_causal_;
    a_acausal_ = rhs.a_acausal_;
    a_th_causal_ = rhs.a_th_causal_;
    a_th_acausal_ = rhs.a_th_acausal_;

    init_flag_ = rhs.init_flag_;
    synapse_id_ = rhs.synapse_id_;
    next_readout_time_ = rhs.next_readout_time_;
    discrete_weight_ = rhs.discrete_weight_;
  }

  void STDPFACETSHWConnectionHom::get_status(DictionaryDatum & d) const
  {
    // base class properties, different for individual synapse
    ConnectionHetWD::get_status(d);

    // own properties, different for individual synapse
    def<double_t>(d, "a_causal", a_causal_);
    def<double_t>(d, "a_acausal", a_acausal_);
    def<double_t>(d, "a_th_causal", a_th_causal_);
    def<double_t>(d, "a_th_acausal", a_th_acausal_);

    def<bool>(d, "init_flag", init_flag_);
    def<long_t>(d, "synapse_id", synapse_id_);
    def<double_t>(d, "next_readout_time", next_readout_time_);
    //useful to get conversion before activity, but weight_per_lut_entry_ not known here
    //def<uint_t>(d, "discrete_weight", entry_to_weight_(weight_to_entry_(weight_, weight_per_lut_entry_), weight_per_lut_entry_));
  }

  void STDPFACETSHWConnectionHom::set_status(const DictionaryDatum & d, ConnectorModel &cm)
  {
    // base class properties
    ConnectionHetWD::set_status(d, cm);

    updateValue<double_t>(d, "a_causal", a_causal_);
    updateValue<double_t>(d, "a_acausal", a_acausal_);
    updateValue<double_t>(d, "a_th_causal", a_th_causal_);
    updateValue<double_t>(d, "a_th_acausal", a_th_acausal_);

    updateValue<long_t>(d, "synapse_id", synapse_id_);

    //they should not be allowed to be changed!
    //updateValue<bool>(d, "init_flag", init_flag_);
    //updateValue<double_t>(d, "next_readout_t", next_readout_time_);

    //setting discrete_weight_ does not make sense, is temporary variable
  }

   /**
   * Set properties of this connection from position p in the properties
   * array given in dictionary.
   */
  void STDPFACETSHWConnectionHom::set_status(const DictionaryDatum & d, index p, ConnectorModel &cm)
  {
    ConnectionHetWD::set_status(d, p, cm);

     if (d->known("tau_pluss") ||
         d->known("tau_minus_stdps") ||
         d->known("Wmaxs") ||
         d->known("weight_per_lut_entrys") ||
         d->known("no_synapsess") ||
         d->known("synapses_per_drivers") ||
         d->known("driver_readout_times") ||
         d->known("readout_cycle_durations") ||
         d->known("hardware_stages") ||
         d->known("lookuptable_causals") ||
         d->known("lookuptable_acausals") )
     {
       cm.network().message(SLIInterpreter::M_ERROR, "STDPFACETSHWConnectionHom::set_status()", "you are trying to set common properties via an individual synapse.");
     }
  }

  void STDPFACETSHWConnectionHom::initialize_property_arrays(DictionaryDatum & d) const
  {
    ConnectionHetWD::initialize_property_arrays(d);
  }

  /**
   * Append properties of this connection to the given dictionary. If the
   * dictionary is empty, new arrays are created first.
   */
  void STDPFACETSHWConnectionHom::append_properties(DictionaryDatum & d) const
  {
    ConnectionHetWD::append_properties(d);
  }

} // of namespace nest
