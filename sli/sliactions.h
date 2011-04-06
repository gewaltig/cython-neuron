/*
 *  sliactions.h
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

#ifndef  __DEFAULTACTIONS
#define  __DEFAULTACTIONS
/* 
    Actions associated with SLI types.
*/

#include "slifunction.h"

class DatatypeFunction : public SLIFunction
{
public:
  DatatypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class NametypeFunction : public SLIFunction
{
public:
NametypeFunction () {}
    void execute(SLIInterpreter *) const;
};

class ProceduretypeFunction : public SLIFunction
{
public:
ProceduretypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class LitproceduretypeFunction : public SLIFunction
{
public:
LitproceduretypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class FunctiontypeFunction : public SLIFunction
{
public:
FunctiontypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class CallbacktypeFunction : public SLIFunction
{
public:
CallbacktypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class XIstreamtypeFunction : public SLIFunction
{
public:
XIstreamtypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class XIfstreamtypeFunction : public SLIFunction
{
public:
XIfstreamtypeFunction() {}
    void execute(SLIInterpreter *) const;
};

class TrietypeFunction : public SLIFunction
{
public:
  TrietypeFunction() {}
    void execute(SLIInterpreter *) const;
};

#endif
