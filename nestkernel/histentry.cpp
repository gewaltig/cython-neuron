/*
 *  histentry.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2007 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/**
 * \file histentry.cpp
 * Implementation of archiving_node to record and manage spike history
 * \author Moritz Helias, Abigail Morrison
 * \date april 2006
 * \note moved to separate file to avoid circular inclusion in node.h
 */

#include "histentry.h"

  // member functions of histentry

  nest::histentry::histentry(double_t t, double_t Kminus, double_t triplet_Kminus, size_t access_counter) :
    t_(t), Kminus_(Kminus), triplet_Kminus_(triplet_Kminus), access_counter_(access_counter)
  { } 

