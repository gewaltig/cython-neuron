/*
 *  connection_id.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef CONNECTION_ID_H
#define CONNECTION_ID_H

#include "dictutils.h"
#include "nest_names.h"

namespace nest
{
  
  class ConnectionID
  {
    public:
      ConnectionID() {}  
      ConnectionID(long source_gid, long target_thread, long synapse_typeid, long port);
      
      DictionaryDatum get_dict();
      bool operator==(const ConnectionID& c);
      std::ostream & print_me(std::ostream& out) const;
    
    private:
      long source_gid_;
      long target_thread_;
      long synapse_typeid_;
      long port_;
  };

  std::ostream & operator<<(std::ostream& , const ConnectionID&);
  
} // namespace

#endif /* #ifndef CONNECTION_ID_H */
