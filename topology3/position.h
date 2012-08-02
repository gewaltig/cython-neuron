#ifndef POSITION_H
#define POSITION_H

/*
 *  position.h
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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include "nest.h"
#include "token.h"

namespace nest
{

  // It is necessary to declare the template for operator<< first in order
  // to get the friend declaration to work
  template <int D, class T>
  class Position;

  template <int D, class T>
  std::ostream & operator<<(std::ostream & os, const Position<D,T> & pos);

  template <int D, class T=double_t>
  class Position
  {
  public:
    template<int OD, class OT>
    friend class Position;

    /**
     * Default constructor, initializing all coordinates to zero.
     */
    Position();

    /**
     * 2D Constructor.
     */
    Position(const T&, const T&);

    /**
     * 3D Constructor.
     */
    Position(const T&, const T&, const T&);

    /**
     * Constructor initializing a Position from an array.
     */
    Position(const T * const y);

    /**
     * Constructor initializing a Position from a std::vector.
     */
    Position(const std::vector<T> &y);

    /**
     * Copy constructor.
     */
    Position(const Position & other);

    /**
     * @returns the Position as a std::vector
     */
    operator std::vector<T>() const;

    /**
     * @returns an element (coordinate) of the Position
     */
    T &operator[](int i);

    /**
     * @returns an element (coordinate) of the Position
     */
    const T &operator[](int i) const;

    /**
     * Moves Position variables into an array.
     * @returns array of positions stored as a token object.
     */
    Token getToken() const;

    /**
     * Elementwise addition.
     * @returns elementwise sum of coordinates.
     */
    template <class OT>
    Position operator+(const Position<D,OT> &other) const;

    /**
     * Elementwise subtraction.
     * @returns elementwise difference of coordinates.
     */
    template <class OT>
    Position operator-(const Position<D,OT> &other) const;

    /**
     * Unary minus.
     * @returns opposite vector.
     */
    Position operator-() const;

    /**
     * Elementwise multiplication.
     * @returns elementwise product of coordinates.
     */
    template <class OT>
    Position operator*(const Position<D,OT> &other) const;

    /**
     * Elementwise division.
     * @returns elementwise quotient of coordinates.
     */
    template <class OT>
    Position operator/(const Position<D,OT> &other) const;

    /**
     * Elementwise addition with scalar
     * @returns position vector with scalar added to all coordinates
     */
    Position operator+(const T &) const;

    /**
     * Elementwise subtraction with scalar
     * @returns position vector with scalar subtracted from all coordinates
     */
    Position operator-(const T &) const;

    /**
     * Multiplication with scalar
     * @returns position vector multiplied with the scalar.
     */
    Position operator*(const T &) const;

    /**
     * Division with scalar
     * @returns position vector divided by the scalar.
     */
    Position operator/(const T &) const;

    /**
     * In-place elementwise addition.
     * @returns the Position itself after adding the other Position
     * elementwise.
     */
    template <class OT>
    Position &operator+=(const Position<D,OT> &);

    /**
     * In-place elementwise subtraction.
     * @returns the Position itself after subtracting the other Position
     * elementwise.
     */
    template <class OT>
    Position &operator-=(const Position<D,OT> &);

    /**
     * In-place elementwise multiplication.
     * @returns the Position itself after multiplying with the other
     * Position elementwise.
     */
    template <class OT>
    Position &operator*=(const Position<D,OT> &);

    /**
     * In-place elementwise division.
     * @returns the Position itself after dividing by the other Position
     * elementwise.
     */
    template <class OT>
    Position &operator/=(const Position<D,OT> &);

    /**
     * In-place elementwise addition with scalar.
     * @returns the Position itself after adding the scalar to all coordinates.
     */
    Position &operator+=(const T &);

    /**
     * In-place elementwise subtraction with scalar.
     * @returns the Position itself after subtracting the scalar from all coordinates.
     */
    Position &operator-=(const T &);

    /**
     * In-place multiplication by scalar.
     * @returns the Position itself after multiplying with the scalar.
     */
    Position &operator*=(const T &);

    /**
     * In-place elementwise division.
     * @returns the Position itself after dividing by the scalar.
     */
    Position &operator/=(const T &);

    /**
     * @returns true if all coordinates are equal
     */
    bool operator==(const Position &y) const;

    /**
     * @returns true if all coordinates are less
     */
    bool operator<(const Position &y) const;

    /**
     * @returns true if all coordinates are greater
     */
    bool operator>(const Position &y) const;

    /**
     * @returns true if all coordinates are less or equal
     */
    bool operator<=(const Position &y) const;

    /**
     * @returns true if all coordinates are greater or equal
     */
    bool operator>=(const Position &y) const;

    /**
     * Length of Position vector.
     * @returns Euclidian norm of the vector.
     */
    T length() const;

    /**
     * @returns string representation of Position
     */
    operator std::string() const;

    /**
     * Print position to output stream.
     *
     * Format: Only as many coordinates as dimensions,
     *         separated by spaces [default], no trailing space.
     *
     * @param out output stream
     * @param sep separator character
     */
    void print(std::ostream& out, char sep = ' ') const;

    /**
     * Output the Position to an ostream.
     */
    friend std::ostream & operator<< <>(std::ostream & os, const Position<D,T> & pos);

  private:
    T x_[D];
  };

  /**
   * A box is defined by the lower left corner (minimum coordinates) and
   * the upper right corner (maximum coordinates).
   */
  template<int D>
  struct Box
  {
    Box(const Position<D> &ll, const Position<D> &ur) :
      lower_left(ll), upper_right(ur)
      {}

    Position<D> lower_left;
    Position<D> upper_right;
  };

  template <int D, class T>
  Position<D,T>::Position()
  {
    for(int i=0;i<D;++i)
      x_[i] = 0;
  }

  template <int D, class T>
  Position<D,T>::Position(const T& x, const T& y)
  {
    assert(D==2);
    x_[0] = x;
    x_[1] = y;
  }

  template <int D, class T>
  Position<D,T>::Position(const T& x, const T& y, const T& z)
  {
    assert(D==3);
    x_[0] = x;
    x_[1] = y;
    x_[2] = z;
  }

  template <int D, class T>
  Position<D,T>::Position(const T * const y)
  {
    for(int i=0;i<D;++i)
      x_[i] = y[i];
  }

  template <int D, class T>
  Position<D,T>::Position(const std::vector<T> &y)
  {
    assert(y.size() == D); // FIXME: should fail more gracefully
    std::copy(y.begin(), y.end(), x_);
  }

  template <int D, class T>
  Position<D,T>::Position(const Position<D,T> & other)
  {
    for(int i=0;i<D;++i)
      x_[i] = other.x_[i];
  }

  template <int D, class T>
  Position<D,T>::operator std::vector<T>() const
  {
    std::vector<double_t> result;

    for(int i=0;i<D;++i)
      result.push_back(x_[i]);

    return result;
  }

  template <int D, class T>
  T & Position<D,T>::operator[](int i)
  {
    return x_[i];
  }

  template <int D, class T>
  const T & Position<D,T>::operator[](int i) const
  {
    return x_[i];
  }

  template <int D, class T>
  Token Position<D,T>::getToken() const
  {
    std::vector<T> result = std::vector<T>(*this);

    return Token(result);
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> Position<D,T>::operator+(const Position<D,OT> &other) const
  {
    Position p = *this;
    p += other;
    return p;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> Position<D,T>::operator-(const Position<D,OT> &other) const
  {
    Position p = *this;
    p -= other;
    return p;
  }

  template <int D, class T>
  Position<D,T> Position<D,T>::operator-() const
  {
    Position p;
    p -= *this;
    return p;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> Position<D,T>::operator*(const Position<D,OT> &other) const
  {
    Position p = *this;
    p *= other;
    return p;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> Position<D,T>::operator/(const Position<D,OT> &other) const
  {
    Position p = *this;
    p /= other;
    return p;
  }

  template <int D, class T>
  Position<D,T> Position<D,T>::operator+(const T &a) const
  {
    Position p = *this;
    p += a;
    return p;
  }

  template <int D, class T>
  Position<D,T> Position<D,T>::operator-(const T &a) const
  {
    Position p = *this;
    p -= a;
    return p;
  }

  template <int D, class T>
  Position<D,T> Position<D,T>::operator*(const T &a) const
  {
    Position p = *this;
    p *= a;
    return p;
  }

  template <int D, class T>
  Position<D,T> Position<D,T>::operator/(const T &a) const
  {
    Position p = *this;
    p /= a;
    return p;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> &Position<D,T>::operator+=(const Position<D,OT> &other)
  {
    for(int i=0;i<D;++i)
      x_[i] += other.x_[i];
    return *this;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> &Position<D,T>::operator-=(const Position<D,OT> &other)
  {
    for(int i=0;i<D;++i)
      x_[i] -= other.x_[i];
    return *this;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> &Position<D,T>::operator*=(const Position<D,OT> &other)
  {
    for(int i=0;i<D;++i)
      x_[i] *= other.x_[i];
    return *this;
  }

  template <int D, class T>
  template <class OT>
  Position<D,T> &Position<D,T>::operator/=(const Position<D,OT> &other)
  {
    for(int i=0;i<D;++i)
      x_[i] /= other.x_[i];
    return *this;
  }

  template <int D, class T>
  Position<D,T> &Position<D,T>::operator+=(const T &a)
  {
    for(int i=0;i<D;++i)
      x_[i] += a;
    return *this;
  }

  template <int D, class T>
  Position<D,T> &Position<D,T>::operator-=(const T &a)
  {
    for(int i=0;i<D;++i)
      x_[i] -= a;
    return *this;
  }

  template <int D, class T>
  Position<D,T> &Position<D,T>::operator*=(const T &a)
  {
    for(int i=0;i<D;++i)
      x_[i] *= a;
    return *this;
  }

  template <int D, class T>
  Position<D,T> &Position<D,T>::operator/=(const T &a)
  {
    for(int i=0;i<D;++i)
      x_[i] /= a;
    return *this;
  }

  template <int D, class T>
  bool Position<D,T>::operator==(const Position<D,T> &y) const
  {
    for(int i=0;i<D;++i) {
      if (x_[i] != y.x_[i]) return false;
    }
    return true;
  }

  template <int D, class T>
  bool Position<D,T>::operator<(const Position<D,T> &y) const
  {
    for(int i=0;i<D;++i) {
      if (x_[i] >= y.x_[i]) return false;
    }
    return true;
  }

  template <int D, class T>
  bool Position<D,T>::operator>(const Position<D,T> &y) const
  {
    for(int i=0;i<D;++i) {
      if (x_[i] <= y.x_[i]) return false;
    }
    return true;
  }

  template <int D, class T>
  bool Position<D,T>::operator<=(const Position<D,T> &y) const
  {
    for(int i=0;i<D;++i) {
      if (x_[i] > y.x_[i]) return false;
    }
    return true;
  }

  template <int D, class T>
  bool Position<D,T>::operator>=(const Position<D,T> &y) const
  {
    for(int i=0;i<D;++i) {
      if (x_[i] < y.x_[i]) return false;
    }
    return true;
  }

  template <int D, class T>
  T Position<D,T>::length() const
  {
    T lensq = 0;
    for(int i=0;i<D;++i)
      lensq += x_[i]*x_[i];
    return std::sqrt(lensq);
  }

  template <int D, class T>
  Position<D,T>::operator std::string() const
  {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  template <int D, class T>
  void Position<D,T>::print(std::ostream& out, char sep) const
  {
    out << x_[0];
    for(int i=1;i<D;++i)
      out << sep << x_[i];
  }

  template <int D, class T>
  std::ostream & operator<<(std::ostream & os, const Position<D,T> & pos)
  {
    os << "(";
    if (D>0)
      os << pos.x_[0];
    for(int i=1;i<D;++i)
      os << ", " << pos.x_[i];
    os << ")";
    return os;
  }

} // namespace nest

#endif
