/*
 *  music_event_handler.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2009 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef MUSIC_EVENT_HANDLER
#define MUSIC_EVENT_HANDLER

#include "config.h"

#ifdef HAVE_MUSIC

#include <music.hh>
#include "nest.h"
#include "node.h"

#include <queue>

namespace nest {
  
  class Network;

/**
 * Event handler for all events of a MUSIC port received on this process.
 */
class MusicEventHandler : public MUSIC::EventHandlerGlobalIndex
{
public:
  MusicEventHandler();
  MusicEventHandler(std::string portname, double acceptable_latency, Network* net);

  virtual ~MusicEventHandler();

  /**
   * Register a new node to a specific channel on this port.
   */
  void register_channel(int channel, nest::Node* mp);

  /**
   * Publish the MUSIC port.
   * This method has to be called once before the first simulation to
   * tell MUSIC which channels lie on which processor.
   */
  void publish_port();
  
  /**
   * Called by MUSIC from within tick() to deliver events to
   * NEST. This function only queues the events. Delivery to the
   * targets takes place in update().
   */
  void operator () (double t, MUSIC::GlobalIndex channel);

  /**
   * This function is called by the scheduler and delivers the queued
   * events to the target music_in_proxies.
   */
  void update(Time const &, const long_t, const long_t);

private:
  MUSIC::EventInputPort *music_port_;
  MUSIC::PermutationIndex *music_perm_ind_;
  bool published_;
  std::string portname_;
  std::vector<nest::Node*> channelmap_;      //!< Maps channel number to music_event_in_proxy
  std::vector<MUSIC::GlobalIndex> indexmap_; //!< Maps local index to global MUSIC index (channel)
  double acceptable_latency_;                //!< The acceptable latency of the port in ms
  Network* net_;                             //!< Pointer to global network driver.

  /**
   * Buffers incoming spike events until they are due. The vector has
   * one entry per channel. The priority queues used within the vector
   * implement min-heaps stored in vectors.
   */
  std::vector< std::priority_queue< double, std::vector<double>, std::greater<double> > > eventqueue_;
};
  
} // namespace nest

#endif // HAVE_MUSIC

#endif // MUSIC_EVENT_HANDLER
