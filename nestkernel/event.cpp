
/*
 *  event.cpp.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004-2008 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/** 
 *  @file event_impl.h
 *  Implementation of Event::operator() for all event types.
 *  @note Must be isolated here, since it requires full access to
 *  classes Node and Scheduler.
 *  @note Cannot be cpp-file because of templates for AnalogDataRequest.
 *  @note All functions in this file must be inline, to avoid duplicate
 *        objects.
 *  @note Presently included in network.h and connection.h
 */

#include "node.h"
#include "event.h"
#include "scheduler.h"

namespace nest 
{
    Event::Event()
    : sender_(NULL),
    receiver_(NULL),
    p_(-1),
    rp_(0),
    d_(1),
    stamp_(Time::step(0)),
    offset_(0.0),
    w_(0.0)
    {}


    delay Event::get_max_delay() const
    {
      // This is dead stupid, but I was not able to
      // formulate a forward declaration of the static
      // function Scheduler::get_max_delay() :-(
      // mog
      return Scheduler::get_max_delay();
    }


    void SpikeEvent::operator()()
    {
      receiver_->handle(*this);
    }


    void DSSpikeEvent::operator()()
    {
      sender_->event_hook(*this);
    }

  
    void RateEvent::operator()()
    {
      receiver_->handle(*this);
    }

  
    void CurrentEvent::operator()()
    {
      receiver_->handle(*this);
    }

  
    void DSCurrentEvent::operator()()
    {
      sender_->event_hook(*this);
    }

  
    void ConductanceEvent::operator()()
    {
      receiver_->handle(*this);
    }

  
    void DoubleDataEvent::operator()()
    {
      receiver_->handle(*this);
    }

  
    void DataLoggingRequest::operator()()
    {
      receiver_->handle(*this);
    }

  
    void DataLoggingReply::operator()()
    {
      receiver_->handle(*this);
    }

 }


