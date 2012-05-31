#ifndef MASK_H
#define MASK_H

/*
 *  mask.h
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

#include "nest.h"
#include "topology_names.h"
#include "position.h"
#include "dictdatum.h"
#include "dictutils.h"
#include "topology3module.h"

namespace nest
{
  class Topology3Module;

  /**
   * Abstract base class for masks with unspecified dimension.
   */
  class AbstractMask
  {
  public:
    /**
     * Virtual destructor
     */
    virtual ~AbstractMask()
      {}

    /**
     * @returns true if point is inside mask.
     */
    virtual bool inside(const std::vector<double_t> &) const = 0;

    /**
     * Create the intersection of this mask with another. Masks must have
     * the same dimension
     * @returns a new dynamically allocated mask.
     */
    virtual AbstractMask * intersect_mask(const AbstractMask & other) const = 0;

    /**
     * Create the union of this mask with another. Masks must have the same
     * dimension.
     * @returns a new dynamically allocated mask.
     */
    virtual AbstractMask * union_mask(const AbstractMask & other) const = 0;

    /**
     * Create the difference of this mask and another. Masks must have the
     * same dimension.
     * @returns a new dynamically allocated mask.
     */
    virtual AbstractMask * minus_mask(const AbstractMask & other) const = 0;

  };

  typedef lockPTRDatum<AbstractMask, &Topology3Module::MaskType> MaskDatum;

  /**
   * Abstract base class for masks with given dimension.
   */
  template<int D>
  class Mask : public AbstractMask
  {
  public:
    ~Mask()
      {}

    /**
     * @returns true if point is inside mask.
     */
    virtual bool inside(const Position<D> &) const = 0;

    /**
     * @returns true if point is inside mask.
     */
    bool inside(const std::vector<double_t> &pt) const
      { return inside(Position<D>(pt)); }

    /**
     * @returns true if the whole box is inside the mask.
     * @note a return value of false is not a guarantee that the whole box
     * is not inside the mask.
     */
    virtual bool inside(const Position<D> &,const Position<D> &) const = 0;

    /**
     * @returns true if the whole box is outside the mask.
     * @note a return value of false is not a guarantee that the whole box
     * is not outside the mask.
     */
    virtual bool outside(const Position<D> &,const Position<D> &) const = 0;

    /**
     * Clone method.
     * @returns dynamically allocated copy of mask object
     */
    virtual Mask * clone() const = 0;

    AbstractMask * intersect_mask(const AbstractMask & other) const;
    AbstractMask * union_mask(const AbstractMask & other) const;
    AbstractMask * minus_mask(const AbstractMask & other) const;
  };

  /**
   * Mask which covers all of space
   */
  template<int D>
  class AllMask : public Mask<D>
  {
  public:

    ~AllMask()
      {}

    using Mask<D>::inside;

    /**
     * @returns true always for this mask.
     */
    bool inside(const Position<D> &) const
      { return true; }

    /**
     * @returns true always for this mask
     */
    bool inside(const Position<D> &,const Position<D> &) const
      { return true; }

    /**
     * @returns false always for this mask
     */
    bool outside(const Position<D> &,const Position<D> &) const
      { return false; }

    Mask<D> * clone() const
      { return new AllMask(); }
  };

  /**
   * Mask defining a box region.
   */
  template<int D>
  class BoxMask : public Mask<D>
  {
  public:

    /**
     * Parameters that should be in the dictionary:
     * lower_left  - Position of lower left corner (array of doubles)
     * upper_right - Position of upper right corner (array of doubles)
     */
    BoxMask(const DictionaryDatum&);

    ~BoxMask()
      {}

    using Mask<D>::inside;

    /**
     * @returns true if point is inside the box
     */
    bool inside(const Position<D> &p) const
      { return (p>=lower_left_) && (p<=upper_right_); }

    /**
     * @returns true if the whole given box is inside this box
     */
    bool inside(const Position<D> &ll,const Position<D> &ur) const
      { return (ll>=lower_left_) && (ur<=upper_right_); }

    /**
     * @returns true if the whole given box is outside this box
     */
    bool outside(const Position<D> &ll,const Position<D> &ur) const
      {
          for(int i=0;i<D;++i) {
              if ((ur[i]<lower_left_[i]) || (ll[i]>upper_right_[i]))
                  return true;
          }
          return false;
      }

    Mask<D> * clone() const
      { return new BoxMask(*this); }

  protected:
    Position<D> lower_left_;
    Position<D> upper_right_;
  };

  /**
   * Mask defining a circular or spherical region.
   */
  template<int D>
  class BallMask : public Mask<D>
  {
  public:
    /**
     * @param center Center of sphere
     * @param radius Radius of sphere
     */
    BallMask(Position<D> center, double_t radius) :
      center_(center), radius_(radius)
      {}

    /**
     * Creates a BallMask from a Dictionary which should contain the key
     * "radius" with a double value and optionally the key "anchor" (the
     * center position) with an array of doubles.
     */
    BallMask(const DictionaryDatum&);

    ~BallMask()
      {}

    using Mask<D>::inside;

    /**
     * @returns true if point is inside the circle
     */
    bool inside(const Position<D> &p) const
      { return (p-center_).length() <= radius_; }

    /**
     * @returns true if the whole box is inside the circle
     */
    bool inside(const Position<D> &ll, const Position<D> &ur) const;

    /**
     * @returns true if the whole box is outside the circle
     */
    bool outside(const Position<D> &ll, const Position<D> &ur) const
      {
        // Currently only checks if the box is outside the bounding box of
        // the ball. This could be made more refined.
        for (int i=0; i<D; ++i) {
          if ((ur[i] < center_[i]-radius_) || (ll[i] > center_[i]+radius_))
            return true;
        }
        return false;
      }

    Mask<D> * clone() const
      { return new BallMask(*this); }

  protected:
    Position<D> center_;
    double_t radius_;
  };

  /**
   * Mask combining two masks with a Boolean AND, the intersection.
   */
  template<int D>
  class IntersectionMask : public Mask<D> {
  public:

    /**
     * Construct the intersection of the two given masks. Copies are made
     * of the supplied Mask objects.
     */
    IntersectionMask(const Mask<D> &m1, const Mask<D> &m2):
      mask1_(m1.clone()), mask2_(m2.clone())
      {}

    /**
     * Copy constructor
     */
    IntersectionMask(const IntersectionMask &m):
      mask1_(m.mask1_->clone()), mask2_(m.mask2_->clone())
      {}

    ~IntersectionMask()
      { delete mask1_; delete mask2_; }

    bool inside(const Position<D> &p) const
      { return mask1_->inside(p) && mask2_->inside(p); }

    bool inside(const Position<D> &ll, const Position<D> &ur) const
      { return mask1_->inside(ll,ur) && mask2_->inside(ll,ur); }

    bool outside(const Position<D> &ll, const Position<D> &ur) const
      { return mask1_->outside(ll,ur) || mask2_->outside(ll,ur); }

    Mask<D> * clone() const
      { return new IntersectionMask(*this); }

  protected:
    Mask<D> *mask1_, *mask2_;
  };

  /**
   * Mask combining two masks with a Boolean OR, the sum.
   */
  template<int D>
  class UnionMask : public Mask<D> {
  public:

    /**
     * Construct the union of the two given masks. Copies are made
     * of the supplied Mask objects.
     */
    UnionMask(const Mask<D> &m1, const Mask<D> &m2):
      mask1_(m1.clone()), mask2_(m2.clone())
      {}

    /**
     * Copy constructor
     */
    UnionMask(const UnionMask &m):
      mask1_(m.mask1_->clone()), mask2_(m.mask2_->clone())
      {}

    ~UnionMask()
      { delete mask1_; delete mask2_; }

    bool inside(const Position<D> &p) const
      { return mask1_->inside(p) || mask2_->inside(p); }

    bool inside(const Position<D> &ll, const Position<D> &ur) const
      { return mask1_->inside(ll,ur) || mask2_->inside(ll,ur); }

    bool outside(const Position<D> &ll, const Position<D> &ur) const
    { return mask1_->outside(ll,ur) && mask2_->outside(ll,ur); }

    Mask<D> * clone() const
      { return new UnionMask(*this); }

  protected:
    Mask<D> *mask1_, *mask2_;
  };

  /**
   * Mask combining two masks with a minus operation, the difference.
   */
  template<int D>
  class DifferenceMask : public Mask<D> {
  public:

    /**
     * Construct the difference of the two given masks. Copies are made
     * of the supplied Mask objects.
     */
    DifferenceMask(const Mask<D> &m1, const Mask<D> &m2):
      mask1_(m1.clone()), mask2_(m2.clone())
      {}

    /**
     * Copy constructor
     */
    DifferenceMask(const DifferenceMask &m):
      mask1_(m.mask1_->clone()), mask2_(m.mask2_->clone())
      {}

    ~DifferenceMask()
      { delete mask1_; delete mask2_; }

    bool inside(const Position<D> &p) const
      { return mask1_->inside(p) && !mask2_->inside(p); }

    bool inside(const Position<D> &ll, const Position<D> &ur) const
      { return mask1_->inside(ll,ur) && mask2_->outside(ll,ur); }

    bool outside(const Position<D> &ll, const Position<D> &ur) const
      { return mask1_->outside(ll,ur) || mask2_->inside(ll,ur); }

    Mask<D> * clone() const
      { return new DifferenceMask(*this); }

  protected:
    Mask<D> *mask1_, *mask2_;
  };


  /**
   * Mask oriented in the opposite direction.
   */
  template<int D>
  class ConverseMask : public Mask<D> {
  public:
    /**
     * Construct the converse of the two given mask. A copy is made of the
     * supplied Mask object.
     */
    ConverseMask(const Mask<D> &m): m_(m.clone()) {}

    /**
     * Copy constructor
     */
    ConverseMask(const ConverseMask &m): m_(m.m_->clone()) {}

    ~ConverseMask()
      { delete m_; }

    bool inside(const Position<D> &p) const
      { return m_->inside(-p); }

    bool inside(const Position<D> &ll, const Position<D> &ur) const
      { return m_->inside(-ur,-ll); }

    bool outside(const Position<D> &ll, const Position<D> &ur) const
      { return m_->outside(-ur,-ll); }

    Mask<D> * clone() const
      { return new ConverseMask(*this); }

  protected:
    Mask<D> *m_;
  };


  template<int D>
  AbstractMask * Mask<D>::intersect_mask(const AbstractMask & other) const
  {
    const Mask * other_d = dynamic_cast<const Mask*>(&other);
    assert(other_d); // FIXME: Fail gracefully
    return new IntersectionMask<D>(*this,*other_d);
  }

  template<int D>
  AbstractMask * Mask<D>::union_mask(const AbstractMask & other) const
  {
    const Mask * other_d = dynamic_cast<const Mask*>(&other);
    assert(other_d); // FIXME: Fail gracefully
    return new UnionMask<D>(*this,*other_d);
  }

  template<int D>
  AbstractMask * Mask<D>::minus_mask(const AbstractMask & other) const
  {
    const Mask * other_d = dynamic_cast<const Mask*>(&other);
    assert(other_d); // FIXME: Fail gracefully
    return new DifferenceMask<D>(*this,*other_d);
  }

  template<int D>
  BoxMask<D>::BoxMask(const DictionaryDatum& d)
  {
    lower_left_ = getValue<std::vector<double_t> >(d, names::lower_left);
    upper_right_ = getValue<std::vector<double_t> >(d, names::upper_right);

    // TODO: Implement shifting by anchor for compatibility with topo2
  }

  template<int D>
  BallMask<D>::BallMask(const DictionaryDatum& d)
  {
    radius_ = getValue<double_t>(d, names::radius);
    if (d->known(names::anchor)) {
      center_ = getValue<std::vector<double_t> >(d, names::anchor);
    }
  }



} // namespace nest

#endif
