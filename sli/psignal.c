/*
 *  psignal.c
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

/*******************************/
/* signal handling code  */
#include "psignal.h"
/* Do Not switch the order of the #include and #define statements!!! */
#ifndef _POSIX_SOURCE
#define _SYNOD__SET_POSIX_SOURCE
#define _POSIX_SOURCE
#endif


int SLIsignalflag =0;

/* 
   The following is a POSIX.1 conforming implementation of
   the ISO C signal() funtion. 
   Since Solaris 7 still sticks to the unreliable Signal mechanism
   of Unix SVR4, we decide to implement a new version, using 
   the POSIX.1 sigaction function.
   The implementation is taken from 
   Stevens, Richard W. (1993) "Advanced Programming in the UNIX Environment",
       Addison Wesley Longman, Reading, MA
*/ 


Sigfunc*
posix_signal(int signo, Sigfunc *func)
{
  struct sigaction act, oact;

/* the following comment is from Alpha signal.h: */
/*
 * POSIX.1 specifies no argument for this function pointer, although
 * the intention is clearly that it be (*sa_handler)(int).
 */
  
  act.sa_handler = (void(*)())func; /* Thus we cast the supplied poiner! */
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  if(signo == SIGALRM)
  {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; /* SunOS */
#endif
  }
  else
  {
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;  /* SVR4, 4.3+BSD */
#endif
  }
  if(sigaction(signo, &act, &oact) < 0)
    return(SIG_ERR);
  return(oact.sa_handler);
}

void SLISignalHandler(int s)
{
/*
   We explicitly assume signal to be POSIX.1 conforming.
   Store the numeric value of the signal in a global variable.
   its value is later evaluated in the interpreter cycle.
*/
  if(SLIsignalflag == 0) /* Ignore second signal, if the */
    {
      SLIsignalflag = s;   /* first has not been processed.*/
    }
 
  return;
}
#ifdef _SYNOD__SET_POSIX_SOURCE
#undef _SYNOD__SET_POSIX_SOURCE
#undef _POSIX_SOURCE
#endif
