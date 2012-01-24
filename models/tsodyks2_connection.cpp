/*
 *  tsodyks_connection.cpp
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

#include "tsodyks2_connection.h"
#include "network.h"
#include "connector_model.h"

namespace nest
{

  Tsodyks2Connection::Tsodyks2Connection() :
    ConnectionHetWD(),
    U_(0.5),
    u_(U_),
    x_(U_), // Since u_ is the maximal probability, x_ should be <= u_
    tau_rec_(800.0),
    tau_fac_(0.0)
  {
  }

  void Tsodyks2Connection::get_status(DictionaryDatum & d) const
  {
    ConnectionHetWD::get_status(d);

    def<double_t>(d, "U", U_);
    def<double_t>(d, "u", u_);
    def<double_t>(d, "tau_rec", tau_rec_);
    def<double_t>(d, "tau_fac", tau_fac_);
    def<double_t>(d, "x", x_);
    
  }
  
  void Tsodyks2Connection::set_status(const DictionaryDatum & d, ConnectorModel &cm)
  {
    ConnectionHetWD::set_status(d, cm);
    
    updateValue<double_t>(d, "U", U_);
    updateValue<double_t>(d, "u", u_);
    updateValue<double_t>(d, "tau_rec", tau_rec_);
    updateValue<double_t>(d, "tau_fac", tau_fac_);
    updateValue<double_t>(d, "x", x_);
  }

  /**
   * Set properties of this connection from position p in the properties
   * array given in dictionary.
   */  
  void Tsodyks2Connection::set_status(const DictionaryDatum & d, index p, ConnectorModel &cm)
  {
    ConnectionHetWD::set_status(d, p, cm);

    set_property<double_t>(d, "Us", p, U_);
    set_property<double_t>(d, "us", p, u_);
    set_property<double_t>(d, "xs", p, x_);
    set_property<double_t>(d, "tau_recs", p, tau_rec_);
  }

  void Tsodyks2Connection::initialize_property_arrays(DictionaryDatum & d) const
  {
    ConnectionHetWD::initialize_property_arrays(d);

    initialize_property_array(d, "Us"); 
    initialize_property_array(d, "us"); 
    initialize_property_array(d, "tau_recs");  
    initialize_property_array(d, "tau_facs");  
    initialize_property_array(d, "xs"); 
  }

  /**
   * Append properties of this connection to the given dictionary. If the
   * dictionary is empty, new arrays are created first.
   */
  void Tsodyks2Connection::append_properties(DictionaryDatum & d) const
  {
    ConnectionHetWD::append_properties(d);

    append_property<double_t>(d, "Us", U_); 
    append_property<double_t>(d, "us", u_); 
    append_property<double_t>(d, "tau_recs", tau_rec_);  
    append_property<double_t>(d, "tau_facs", tau_fac_);  
    append_property<double_t>(d, "xs", x_); 
  }

} // of namespace nest
