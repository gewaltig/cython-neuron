/*
 *  gnureadline.cc
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

#include "gnureadline.h"
#include <cstdio>
#include "stringdatum.h"
#include <algorithm>

#ifdef HAVE_READLINE

extern "C" char* readline(const char *);
extern "C" void  add_history(const char *);
extern "C" int   read_history(const char *);
extern "C" int   write_history(const char *);
extern "C" void  using_history();
extern "C" void  clear_history();

/*BeginDocumentation
Name: GNUreadline - Read and edit a line from standard input
Synopsis: (prompt) GNUreadline -> (string) true
                               -> false
Description: GNUreadline offers an interface to the GNU readline library.
It offers - line editing
          - history browsing
          - filename completion (with TAB)
Remarks: If GNUreadline is executed with a number, the Interpreter is exited
(in contrast to readline).
SeeAlso: readline, GNUaddhistory
*/
void GNUReadline::GNUReadlineFunction::execute(SLIInterpreter *i) const
{
  //call: promptstring GNUreadline -> string true
  //                                         false

  assert(i->OStack.load()>0);
  i->EStack.pop();

  StringDatum *sd = dynamic_cast<StringDatum *>(i->OStack.top().datum());
  assert(sd != NULL);
  char *line_read = readline(sd->c_str());
  if(line_read == NULL)
  {
    // We have received EOF (Ctrl-D), so we quit.
    std::cout << std::endl; 
    Token t("quit");
    i->OStack.top().swap(t);
    i->OStack.push(i->baselookup(i->true_name));
  }
  else
  {
    StringDatum *sr = new StringDatum(line_read);  
    std::free(line_read);
    if(sr->empty())
    {
      i->OStack.pop();
      i->OStack.push(i->baselookup(i->false_name));
    }
    else
    {
      Token t(sr);
      i->OStack.top().swap(t);
      i->OStack.push(i->baselookup(i->true_name));
    }
  }
}

/*BeginDocumentation
Name: GNUaddhistory - Add a string to the readline-history
Synopsis: (string) GRNUaddhistory -> -
Description: Adds a string to the readline history. 
SeeAlso: GNUreadline
*/
void GNUReadline::GNUAddhistoryFunction::execute(SLIInterpreter *i) const
{
  //call: string GNUaddhistory

  assert(i->OStack.load()>0);
  i->EStack.pop();
  StringDatum *sd = dynamic_cast<StringDatum *>(i->OStack.top().datum());
  assert(sd != NULL);
  add_history(sd->c_str());
  char *home = std::getenv("HOME");
  std::string hist_file = std::string(home) + std::string("/.nest_history");
  
  // We write the history after _each_ command. This comes in handy if you
  // experience a crash during a long interactive session.
  write_history(hist_file.c_str());
  i->OStack.pop();
}


GNUReadline::~GNUReadline()
{
  clear_history();
}

void GNUReadline::init(SLIInterpreter *i)
{
  i->createcommand("GNUreadline", &gnureadlinefunction);
  i->createcommand("GNUaddhistory",&gnuaddhistoryfunction);
  using_history();
  char *home = std::getenv("HOME");
  std::string hist_file = std::string(home) + std::string("/.nest_history");
  read_history(hist_file.c_str());
}

#endif //HAVE_READLINE
