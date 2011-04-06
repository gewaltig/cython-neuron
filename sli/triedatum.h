/*
 *  triedatum.h
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

#ifndef TYPEDFUNCTIONDATUM_H
#define TYPEDFUNCTIONDATUM_H
#include <typeinfo>
#include "slifunction.h"
#include "interpret.h"
#include "typechk.h"
#include "allocator.h"
#include "datum.h"

class TrieDatum: public TypedDatum<&SLIInterpreter::Trietype>
{
 protected:
  static sli::pool memory;
 private:
    Name     name;
    TypeTrie tree;
    
    Datum * clone(void) const
    {
        return new TrieDatum(*this);
    }


public:
    TrieDatum(TrieDatum const &fd)
            :TypedDatum<&SLIInterpreter::Trietype>(fd),
	     name(fd.name),tree(fd.tree)
    {}

    TrieDatum(Name const &n)
            :name(n), tree() {}
    
    TrieDatum(Name const &n, const TokenArray &ta)
            :name(n), tree(ta) {}
    

  void print(std::ostream& o) const
    {
        o << '+' << name << '+';
    }

    void pprint(std::ostream& o) const
    {
        print(o);
	o << "\nVariants are:" << std::endl;
	tree.info(o);
    }
    
    void info(std::ostream &out) const
    {
        out << "TrieDatum::info\n";
	pprint(out);
    }

    bool equals(Datum const *) const;
    
    const Name & getname(void) const
    {
        return name;
    }

    void insert(const TypeArray &a, const Token &t)
    {
        tree.insert(a,t);
    }
    
    void insert_move(const TypeArray &a, Token &t)
    {
        tree.insert_move(a,t);
    }
    
    Token  lookup(const TokenStack &s) const
    {
        return tree.lookup(s);
    }

    TypeTrie& get(void)
    {
      return tree;
    }

  static void * operator new(size_t size)
    {
      if(size != memory.size_of())
	return ::operator new(size);
      return memory.alloc();
    }

  static void operator delete(void *p, size_t size)
    {
      if(p == NULL)
	return;
      if(size != memory.size_of())
      {
	::operator delete(p);
	return;
      }
      memory.free(p);
    }
    
};


#endif
