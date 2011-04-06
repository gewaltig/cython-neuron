/*
 *  token.cc
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

#include <algorithm>
#include "token.h"
#include "datum.h"
#include "name.h"
#include "tokenarray.h"
#include "tokenutils.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "namedatum.h"
#include "booldatum.h"
#include "stringdatum.h"
#include "arraydatum.h"

/***********************************************************/
/* Definitions for Token                                       */
/***********************************************************/

// The copy-contructor must perform a kind of bootstrapping,
// since we cannot use the copy-contructor of the datum to
// create the new entry.
// Thus, this constructor must only be called by the
// (virtual) Datum members who create new Datums


Token::Token(int value)
{
  p = new IntegerDatum(value);
}

Token::Token(unsigned int value)
{
  p = new IntegerDatum(value);
}

Token::Token(long value)
{
  p = new IntegerDatum(value);
}

Token::Token(unsigned long value)
{
  p = new IntegerDatum(value);
}

Token::Token(double value)
{
  p = new DoubleDatum(value);
}

Token::Token(bool value)
{
  p = new BoolDatum(value);
}

Token::Token(const char* value)
{
  p= new StringDatum(value);
}

Token::Token(std::string value)
{
  p= new StringDatum(value);
}

Token::Token(const std::vector<long>& value)
{
  p= new ArrayDatum(value);
}

Token::Token(const std::vector<size_t>& value)
{
  p= new ArrayDatum(value);
}

Token::Token(const std::vector<double>& value)
{
  p= new ArrayDatum(value);
}

Token::Token(const std::valarray<double>& value)
{
  p= new ArrayDatum(value);
}

/*
Token::operator Datum* () const
{
  return p;
}
*/

Token::operator long () const
{
  return getValue<long>(*this);
}

Token::operator size_t () const
{
  return getValue<long>(*this);
}

Token::operator double () const
{
  return getValue<double>(*this);
}

Token::operator float () const
{
  return getValue<float>(*this);
}

Token::operator bool () const
{
  return getValue<bool>(*this);
}

Token::operator std::string () const
{
  return getValue<std::string>(*this);
}

void Token::info(std::ostream &out) const
{
    out << "Token::info\n";
    if(p)
    {
        p->Datum::info(out);
        
        out << "p    = " << p << std::endl;
        
        out << "Type = " << type().name() << std::endl;
        p->info(out);
    }
    else
        out << "<NULL token>\n";
}

void Token::pprint(std::ostream &out) const
{
  if (!p)
      out << "<Null token>";
  else
    p->pprint(out); 
}

std::ostream& operator<<(std::ostream& o, const Token& c)
{
  if (!c)
      o << "<Null token>";
  else
    c->print(o); 
  return o;
}


