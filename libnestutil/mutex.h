/*
 *  mutex.h
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

#ifndef MUTEX_H
#define MUTEX_H

#include "config.h"

#ifdef HAVE_PTHREADS
#ifdef HAVE_PTHREAD_IGNORED
#undef __PURE_CNAME
#include <pthread.h>
#define __PURE_CNAME
#else
#include <pthread.h>
#endif
#endif

namespace nest {

  class Mutex
  {
    Mutex(Mutex&);
  public:
    Mutex();
    ~Mutex();
    
    void lock();
    void unlock();
#ifdef HAVE_PTHREADS
    pthread_mutex_t* operator&();
  private:
    pthread_mutex_t mtx;
#endif
  };

  inline
  Mutex::Mutex()
  { 
#ifdef HAVE_PTHREADS
    pthread_mutex_init(&mtx,NULL);
#endif
  }

  inline
  Mutex::~Mutex()
  { 
#ifdef HAVE_PTHREADS
    pthread_mutex_destroy(&mtx);
#endif
  }
  
  inline
  void Mutex::lock()
  { 
#ifdef HAVE_PTHREADS
    pthread_mutex_lock(&mtx);
#endif
  }

  inline
  void Mutex::unlock()
  { 
#ifdef HAVE_PTHREADS
    pthread_mutex_unlock(&mtx);
#endif
  }

#ifdef HAVE_PTHREADS
  inline
  pthread_mutex_t* Mutex::operator& ()
  {
    return &mtx;
  }
#endif


}
#endif
