/*
 *  sibling_container.cpp
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

#include "event.h"
#include "sibling_container.h"
#include "dictdatum.h"
#include "arraydatum.h"
#include "dictutils.h"
#include "network.h"
#include <string>

#ifdef N_DEBUG
#undef N_DEBUG
#endif

nest::SiblingContainer::SiblingContainer()
  :Node(),
   nodes_()
{
  set(frozen);  // freeze SiblingContainer by default
}

nest::SiblingContainer::SiblingContainer(const SiblingContainer &c)
  :Node(c),
   nodes_(c.nodes_)
{}

