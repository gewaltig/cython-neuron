
/*
 *  stdp_connection.cpp
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
#include "stdp_connection.h"
#include "event.h"


namespace nest
{

  STDPConnection::STDPConnection() :
    ConnectionHetWD(),
    tau_plus_(20.0),
    lambda_(0.01),
    alpha_(1.0),
    mu_plus_(1.0),
    mu_minus_(1.0),
    Wmax_(100.0),
    Kplus_(0.0)
  { }


  STDPConnection::STDPConnection(const STDPConnection &rhs) :
    ConnectionHetWD(rhs)
  {
    tau_plus_ = rhs.tau_plus_;
    lambda_ = rhs.lambda_;
    alpha_ = rhs.alpha_;
    mu_plus_ = rhs.mu_plus_;
    mu_minus_ = rhs.mu_minus_;
    Wmax_ = rhs.Wmax_;
    Kplus_ = rhs.Kplus_;
  }

  void STDPConnection::get_status(DictionaryDatum & d) const
  {
    ConnectionHetWD::get_status(d);
    def<double_t>(d, "tau_plus", tau_plus_);
    def<double_t>(d, "lambda", lambda_);
    def<double_t>(d, "alpha", alpha_);
    def<double_t>(d, "mu_plus", mu_plus_);
    def<double_t>(d, "mu_minus", mu_minus_);
    def<double_t>(d, "Wmax", Wmax_);
  }

  void STDPConnection::set_status(const DictionaryDatum & d, ConnectorModel &cm)
  {
    ConnectionHetWD::set_status(d, cm);
    updateValue<double_t>(d, "tau_plus", tau_plus_);
    updateValue<double_t>(d, "lambda", lambda_);
    updateValue<double_t>(d, "alpha", alpha_);
    updateValue<double_t>(d, "mu_plus", mu_plus_);
    updateValue<double_t>(d, "mu_minus", mu_minus_);
    updateValue<double_t>(d, "Wmax", Wmax_);
  }

   /**
   * Set properties of this connection from position p in the properties
   * array given in dictionary.
   */
  void STDPConnection::set_status(const DictionaryDatum & d, index p, ConnectorModel &cm)
  {
    ConnectionHetWD::set_status(d, p, cm);

    set_property<double_t>(d, "tau_pluss", p, tau_plus_);
    set_property<double_t>(d, "lambdas", p, lambda_);
    set_property<double_t>(d, "alphas", p, alpha_);
    set_property<double_t>(d, "mu_pluss", p, mu_plus_);
    set_property<double_t>(d, "mu_minuss", p, mu_minus_);
    set_property<double_t>(d, "Wmaxs", p, Wmax_);
  }

  void STDPConnection::initialize_property_arrays(DictionaryDatum & d) const
  {
    ConnectionHetWD::initialize_property_arrays(d);

    initialize_property_array(d, "tau_pluss"); 
    initialize_property_array(d, "lambdas"); 
    initialize_property_array(d, "alphas"); 
    initialize_property_array(d, "mu_pluss"); 
    initialize_property_array(d, "mu_minuss");
    initialize_property_array(d, "Wmaxs");
  }

  /**
   * Append properties of this connection to the given dictionary. If the
   * dictionary is empty, new arrays are created first.
   */
  void STDPConnection::append_properties(DictionaryDatum & d) const
  {
    ConnectionHetWD::append_properties(d);

    append_property<double_t>(d, "tau_pluss", tau_plus_); 
    append_property<double_t>(d, "lambdas", lambda_); 
    append_property<double_t>(d, "alphas", alpha_); 
    append_property<double_t>(d, "mu_pluss", mu_plus_); 
    append_property<double_t>(d, "mu_minuss", mu_minus_);
    append_property<double_t>(d, "Wmaxs", Wmax_);
  }

} // of namespace nest
