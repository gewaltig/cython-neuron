/*
 *  multirange.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#ifndef MULTIRANGE_H
#define MULTIRANGE_H

#include <vector>
#include <utility>
#include "nest.h"

namespace nest {

/**
 * Class for sequences of ranges acting like a compressed vector.
 */
  class Multirange
  {
  public:
    typedef std::pair<index,index> Range;
    typedef std::vector<Range> RangeVector;

    class iterator {
    public:
      iterator(RangeVector::const_iterator iter, index n);
      index operator*() const;
      bool operator!=(const iterator& other) const;
      iterator& operator++();
      iterator operator++(int);

    private:
      RangeVector::const_iterator pair_iter_;
      index n_;
    };

    Multirange();
    void push_back(index x);
    void clear();
    index operator[](index n) const;
    index size() const;
    bool empty() const;
    iterator begin() const;
    iterator end() const;

  private:
    RangeVector ranges_;
    index size_;
  };

  inline
  Multirange::Multirange():
    ranges_(), size_(0)
  {
  }

  inline
  void Multirange::push_back(index x)
  {
    if ((not ranges_.empty()) && (ranges_.back().second+1 == x)) {
      ++ranges_.back().second;
    } else {
      ranges_.push_back(Range(x,x));
    }
    ++size_;
  }

  inline
  void Multirange::clear()
  {
    ranges_.clear();
    size_ = 0;
  }

  inline
  index Multirange::size() const
  {
    return size_;
  }

  inline
  bool Multirange::empty() const
  {
    return size_ == 0;
  }

  inline
  Multirange::iterator::iterator(RangeVector::const_iterator iter, index n):
    pair_iter_(iter), n_(n)
  {
  }

  inline
  bool Multirange::iterator::operator!=(const iterator& other) const
  {
    return (other.pair_iter_ != pair_iter_) || (other.n_ != n_);
  }

  inline
  index Multirange::iterator::operator*() const
  {
    return pair_iter_->first + n_;
  }

  inline
  Multirange::iterator& Multirange::iterator::operator++()
  {
    ++n_;
    if (n_ > pair_iter_->second-pair_iter_->first) {
      ++pair_iter_;
      n_ = 0;
    }
    return *this;
  }

  inline
  Multirange::iterator Multirange::iterator::operator++(int)
  {
    iterator tmp=*this;
    ++(*this);
    return tmp;
  }

  inline
  Multirange::iterator Multirange::begin() const
  {
    return Multirange::iterator(ranges_.begin(),0);
  }

  inline
  Multirange::iterator Multirange::end() const
  {
    return Multirange::iterator(ranges_.end(),0);
  }


} // namespace

#endif
