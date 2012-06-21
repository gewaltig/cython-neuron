/*
 *  selector.cpp
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

#include "selector.h"
#include "dictutils.h"
#include "network.h"
#include "topology_names.h"

namespace nest
{

  Selector::Selector(const DictionaryDatum &d) :
    model(-1), depth(-1)
  {
    if (updateValue<long_t>(d, names::lid, depth)) {

      if (depth==0)
        throw BadProperty("lid must be >0");

      depth -= 1; // lid starts at 1 for backwards compatibility
    }

    std::string modelname;
    if(updateValue<std::string>(d, names::model, modelname)) {

      const Token model_token =
        Node::network()->get_modeldict().lookup(modelname);

      if ( model_token.empty() )
        throw UnknownModelName(modelname);

      model = static_cast<long_t>(model_token);

    }
  }

} // namespace nest
