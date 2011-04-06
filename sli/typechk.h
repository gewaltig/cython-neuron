/*
 *  typechk.h
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

#ifndef TYPECHECK_H
#define TYPECHECK_H
/* 
    classes for dynamic type checking in SLI
*/

/******************************************************************
Project:   SYNOD/SLI 2.0

Task:      With a TypeTrie it will be possible to perfrom a type 
           check of (SLI) function input parameters. A TypeNode 
           represents the position and the datatype of a single 
           input parameter. The leaves of the tree will contain the 
           interpreter function of correct input parameters.

           A simple add type tree:
           -----------------------

           root
            |
           long -----------------> double -|
            |                        |
           long -> double -|       long -> double -|
           (add)   (add)           (add)   (add)

Baseclass: None

Inherit :  

History:  This is a revised version of the type checking mechanism based on
          tries.
Author:    Marc-Oliver Gewaltig, Thomas Matyak

Date:      18.11.95

*******************************************************************/


#include <typeinfo>
#include <iostream>
#include <vector>
#include "slifunction.h"
#include "tokenarray.h"
#include "tokenstack.h"
#include "typearray.h"

class TypeTrie {
private:
  class TypeNode
  {
    private:
    unsigned int refs;         // number of references to this Node

    public:
        
    Name        type;          // expected type at this stack level
    Token       func;          // points to the operator or an error func.
    
    TypeNode    *alt;         // points to the next parameter alternative
    TypeNode    *next;        // points to the next stack level for this path
    
    
    void addreference(void)
      { ++refs; }
    
    void removereference(void)
      {
	if(--refs == 0)
	  delete this;
      }

    TypeNode(const Name &n)
      : refs(1), type(n),func(),alt(NULL),next(NULL) {}

    TypeNode(const Name &n, Token f)
      : refs(1), type(n),func(f),alt(NULL),next(NULL) {}

    ~TypeNode()
      {
	assert(refs == 0);
	if (next != NULL)
	  next->removereference();
	if (alt != NULL)
	  alt->removereference();
      }
    void toTokenArray(TokenArray &) const;
    void info(std::ostream &, std::vector<TypeNode const *> &) const;

  };

  TypeNode *root;
  Name any;
  Name object;

//    TypeTrie operator=(const TypeTrie &){}; // disable this operator
  TypeNode * getalternative(TypeNode *, const Name &);

  TypeNode* newnode(const TokenArray &) const;

public:

    TypeTrie()
      : root(new TypeNode(Name())),
	any("anytype"), // This could be defined somewhere else
	object("trie::object") // This could be defined somewhere else
    {}

  TypeTrie(const TokenArray &ta)
    : root(NULL),
      any("anytype"), // This could be defined somewhere else
      object("trie::object") // This could be defined somewhere else
    {
      root= newnode(ta);
    }
  
  TypeTrie(const TypeTrie &tt)
    :root(tt.root),
     any(tt.any), object(tt.object)
    {
      if(root !=NULL)
        root->addreference();
    }

    ~TypeTrie();
  
    void  insert_move(const TypeArray& , Token &);
    void  insert(const TypeArray&a , const Token &t)
    {
        Token tmp(t);
        insert_move(a,tmp);
    }
    
  Token lookup(const TokenStack &st) const;
  
  bool operator == (const TypeTrie &) const;
  
  inline bool equals(const Name &, const Name &) const;
  void toTokenArray(TokenArray &) const;
  void info(std::ostream &) const;
};


// Typename comparison including /anytype which compares
// positively for all other typenames

inline
bool TypeTrie::equals(const Name &t1, const Name &t2) const
{
    return(t1==t2 || t2==any || t1==any);
}
#endif


