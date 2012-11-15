/*
 *  connection_id.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONNECTION_ID_H
#define CONNECTION_ID_H

#include "arraydatum.h"
#include "dictutils.h"

namespace nest
{
  
  class ConnectionID
  {
    public:
      ConnectionID() {}  
      ConnectionID(long source_gid, long target_gid, long target_thread, long synapse_typeid, long port);
      ConnectionID(long source_gid, long target_thread, long synapse_typeid, long port);
      ConnectionID(const ConnectionID &);
      
      DictionaryDatum get_dict() const;
      ArrayDatum to_ArrayDatum() const;
      bool operator==(const ConnectionID& c) const;
      std::ostream & print_me(std::ostream& out) const;      
      long get_source_gid() const;
      long get_target_gid() const;
      long get_target_thread() const;
      long get_synapse_type_id() const;
      long get_port() const;
  protected:
      long source_gid_;
      long target_gid_;
      long target_thread_;
      long synapse_typeid_;
      long port_;
  };

  inline
    ConnectionID::ConnectionID(const ConnectionID& cid)
      : source_gid_(cid.source_gid_),
	target_gid_(cid.target_gid_),
	target_thread_(cid.target_thread_),
	synapse_typeid_(cid.synapse_typeid_),
	port_(cid.port_)
    {}
    
  inline
    long ConnectionID::get_source_gid() const
    {
      return source_gid_;
    }

  inline
    long ConnectionID::get_target_gid() const
    {
      return target_gid_;
    }

  inline
    long ConnectionID::get_target_thread() const
    {
      return target_thread_;
    }

  inline
    long ConnectionID::get_synapse_type_id() const
    {
      return synapse_typeid_;
    }

  inline
    long ConnectionID::get_port() const
    {
      return port_;
    }

  std::ostream & operator<<(std::ostream& , const ConnectionID&);
  
} // namespace

#endif /* #ifndef CONNECTION_ID_H */
