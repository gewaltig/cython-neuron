/*
 *  oosupport.cc
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

/* 
    SLI's data access functions
*/
#include "oosupport.h"
#include "namedatum.h"
#include "dictdatum.h"
#include "dictstack.h"

void OOSupportModule::init(SLIInterpreter *i)
{
  i->createcommand("call", &callmemberfunction);
}

const std::string  OOSupportModule::commandstring(void) const
{
  return std::string("/oosupport /C++ ($Revision$) provide-component  /oosupport /SLI (1.2) require-component");
}

const std::string OOSupportModule::name(void) const
{
  return std::string("OOSupport");
}

void OOSupportModule::CallMemberFunction::execute(SLIInterpreter *i) const
{
        //  call: dict key call -> unknown

    DictionaryDatum *dict =
        dynamic_cast<DictionaryDatum *>(i->OStack.pick(1).datum());
    assert(dict != NULL);
    LiteralDatum *key =
        dynamic_cast<LiteralDatum *>(i->OStack.pick(0).datum());
    assert(key != NULL);

    Token value=(*dict)->lookup(*key);

    if(value.datum() != NULL)
    {
      Token nt(new NameDatum(*key));
      i->DStack->push(*dict);
      i->EStack.pop(); // never forget me
      i->EStack.push(i->baselookup(i->end_name));
      i->EStack.push_move(nt);
      i->OStack.pop(2);
    }
    else
      i->raiseerror("UnknownMember");
}
