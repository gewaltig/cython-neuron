/*
 *  functional.h
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

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

// template<class T>
// T delete_ptr(T p)
// {
//   delete p;
//   return 0;
// }

template<class T>
class delete_ptr
{
  public:
  T* operator()(T *p)
    {
      delete p;
      return 0;
    }
};
#endif
