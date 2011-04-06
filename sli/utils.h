/*
 *  utils.h
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

#ifndef UTILS_H
#define UTILS_H
#include <vector>

#include "token.h"
#include "tokenarray.h"

bool array2vector(std::vector<long> &, const TokenArray &);

#endif
