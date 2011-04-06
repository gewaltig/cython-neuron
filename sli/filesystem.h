/*
 *  filesystem.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
/**************************************
  Functions related to the filesystem.
  SLI's stream I/O functions are located in
  sli_io.h.
  *************************************/

#include "slimodule.h"
#include "slifunction.h"


class FilesystemModule: public SLIModule
{
  class FileNamesFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class SetDirectoryFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class DirectoryFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class MoveFileFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class CopyFileFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class DeleteFileFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class MakeDirectoryFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class RemoveDirectoryFunction: public SLIFunction
  {
    public:
    void execute(SLIInterpreter *) const;
  };

  class TmpNamFunction: public SLIFunction
  {
  public:
    TmpNamFunction() {}
    void execute(SLIInterpreter *) const;
  };

  class CompareFilesFunction: public SLIFunction
  {
  public:
    CompareFilesFunction() {}
    void execute(SLIInterpreter *) const;
  };
  
  public:

  FileNamesFunction filenamesfunction; 
  SetDirectoryFunction setdirectoryfunction;
  DirectoryFunction directoryfunction;
  MoveFileFunction movefilefunction;
  CopyFileFunction copyfilefunction;
  DeleteFileFunction deletefilefunction;
  MakeDirectoryFunction makedirectoryfunction; 
  RemoveDirectoryFunction removedirectoryfunction; 
  TmpNamFunction tmpnamfunction; 
  CompareFilesFunction comparefilesfunction;
  
  FilesystemModule(void) {};
  ~FilesystemModule() {};

  void init(SLIInterpreter *);
  const std::string name(void) const;
  const std::string commandstring(void) const;
};

#endif
























