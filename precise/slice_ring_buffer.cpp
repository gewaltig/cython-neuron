#include "slice_ring_buffer.h"

#include <limits>                
#include <cmath>

nest::SliceRingBuffer::SliceRingBuffer() :
  refract_(std::numeric_limits<long_t>::max(), 0, 0)
{
//  resize();  // sets up queue_
}

void nest::SliceRingBuffer::resize()
{
  long_t newsize = static_cast<long_t>(std::ceil(
     static_cast<double>(Scheduler::get_min_delay()+Scheduler::get_max_delay())
     / Scheduler::get_min_delay()));
  if (queue_.size() != static_cast<ulong_t>(newsize))
    {
      queue_.resize(newsize);
      clear();
    }

#ifndef HAVE_STL_VECTOR_CAPACITY_BASE_UNITY
  // create 1-element buffers
  for ( size_t j = 0 ; j < queue_.size() ; ++j )
    queue_[j].reserve(1);
#endif
}

void nest::SliceRingBuffer::clear()
{
  for ( size_t j = 0 ; j < queue_.size() ; ++j )
    queue_[j].clear();
}

void nest::SliceRingBuffer::prepare_delivery()
{
  // vector to deliver from in this slice
  deliver_ = &(queue_[Scheduler::get_slice_modulo(0)]);
  
  // sort events, first event last
  std::sort(deliver_->begin(), deliver_->end(), std::greater<SpikeInfo>());
}

void nest::SliceRingBuffer::discard_events()
{
  // vector to deliver from in this slice
  deliver_ = &(queue_[Scheduler::get_slice_modulo(0)]);

  deliver_->clear();
}


