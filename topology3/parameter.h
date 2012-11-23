#ifndef PARAMETER_H
#define PARAMETER_H

/*
 *  parameter.h
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

#include "nest.h"
#include "randomgen.h"
#include "nest_names.h"
#include "topology_names.h"
#include "position.h"
#include "dictdatum.h"
#include "dictutils.h"
#include "topologymodule.h"

namespace nest
{
  class TopologyModule;

  /**
   * Abstract base class for parameters
   */
  class Parameter
  {
  public:
    /**
     * Virtual destructor
     */
    virtual ~Parameter()
      {}

    /**
     * @returns the value of the parameter at the given point.
     */
    virtual double_t value(const Position<2> &, librandom::RngPtr&) const = 0;

    /**
     * @returns the value of the parameter at the given point.
     */
    virtual double_t value(const Position<3> &, librandom::RngPtr&) const = 0;

    /**
     * @returns the value of the parameter at the given point.
     */
    double_t value(const std::vector<double_t> &pt, librandom::RngPtr& rng) const;

    /**
     * Clone method.
     * @returns dynamically allocated copy of parameter object
     */
    virtual Parameter * clone() const = 0;

    /**
     * Create the product of this parameter with another.
     * @returns a new dynamically allocated parameter.
     */
    virtual Parameter* multiply_parameter(const Parameter & other) const;
    /**
     * Create the quotient of this parameter with another.
     * @returns a new dynamically allocated parameter.
     */
    virtual Parameter* divide_parameter(const Parameter & other) const;
    /**
     * Create the sum of this parameter with another.
     * @returns a new dynamically allocated parameter.
     */
    virtual Parameter* add_parameter(const Parameter & other) const;
    /**
     * Create the difference of this parameter with another.
     * @returns a new dynamically allocated parameter.
     */
    virtual Parameter* subtract_parameter(const Parameter & other) const;
  };

  typedef lockPTRDatum<Parameter, &TopologyModule::ParameterType> ParameterDatum;

  /**
   * Parameter with constant value.
   */
  class ConstantParameter : public Parameter
  {
  public:
    ConstantParameter(double_t value) : value_(value)
      {}

    ConstantParameter(const DictionaryDatum& d)
      {
        value_ = getValue<double_t>(d, "value");
      }

    ~ConstantParameter()
      {}

    using Parameter::value;

    /**
     * @returns the constant value of this parameter.
     */
    double_t value(const Position<2> &, librandom::RngPtr&) const
      { return value_; }
    double_t value(const Position<3> &, librandom::RngPtr&) const
      { return value_; }

    Parameter * clone() const
      { return new ConstantParameter(value_); }

  private:
    double_t value_;
  };

  /**
   * Abstract base class for parameters only depending on distance.
   */
  class RadialParameter : public Parameter
  {
  public:
    using Parameter::value;

    virtual double_t value(double_t) const = 0;

    double_t value(const Position<2> &p, librandom::RngPtr&) const
      { return value(p.length()); }
    double_t value(const Position<3> &p, librandom::RngPtr&) const
      { return value(p.length()); }

  };

  /**
   * Linear (affine) parameter.
   */
  class LinearParameter : public RadialParameter
  {
  public:
    LinearParameter(const DictionaryDatum& d):
      a_(1.0),
      c_(0.0)
      {
        updateValue<double_t>(d, names::a, a_);
        updateValue<double_t>(d, names::c, c_);
      }

    double_t value(double_t x) const
      {
        return a_*x + c_;
      }

    Parameter * clone() const
      { return new LinearParameter(*this); }

  private:
    double_t a_, c_;
  };


  /**
   * Exponential parameter
   */
  class ExponentialParameter : public RadialParameter
  {
  public:
    ExponentialParameter(const DictionaryDatum& d):
      a_(1.0),
      c_(0.0),
      tau_(1.0)
      {
        updateValue<double_t>(d, names::a, a_);
        updateValue<double_t>(d, names::c, c_);
        updateValue<double_t>(d, names::tau, tau_);
      }

    double_t value(double_t x) const
      {
        return c_ + a_*std::exp(-x/tau_);
      }

    Parameter * clone() const
      { return new ExponentialParameter(*this); }

  private:
    double_t a_, c_, tau_;
  };


  /**
   * Gaussian parameter
   */
  class GaussianParameter : public RadialParameter
  {
  public:
    GaussianParameter(const DictionaryDatum& d):
      c_(0.0),
      p_center_(1.0),
      mean_(0.0),
      sigma_(1.0)
      {
        updateValue<double_t>(d, names::c, c_);
        updateValue<double_t>(d, names::p_center, p_center_);
        updateValue<double_t>(d, names::mean, mean_);
        updateValue<double_t>(d, names::sigma, sigma_);
      }

    double_t value(double_t x) const
      {
        return c_ + p_center_*
          std::exp(-std::pow(x - mean_,2)/(2*std::pow(sigma_,2)));
      }

    Parameter * clone() const
      { return new GaussianParameter(*this); }

  private:
    double_t c_, p_center_, mean_, sigma_;
  };


  /**
   * Bivariate Gaussian parameter
   */
  class Gaussian2DParameter : public Parameter
  {
  public:
    Gaussian2DParameter(const DictionaryDatum& d);

    double_t value(const Position<2>& pos, librandom::RngPtr&) const
      {
        return c_ + 
          p_center_*std::exp(- (  (pos[0]-mean_x_)*(pos[0]-mean_x_)/(sigma_x_*sigma_x_)
                                  + (pos[1]-mean_y_)*(pos[1]-mean_y_)/(sigma_y_*sigma_y_)
                                  - 2.*rho_*(pos[0]-mean_x_)*(pos[1]-mean_y_)/(sigma_x_*sigma_y_) )
                             /(2.*(1.-rho_*rho_)) );
      }

    double_t value(const Position<3>& pos, librandom::RngPtr& rng) const
      {
        return value(Position<2>(pos[0],pos[1]),rng);
      }

    Parameter * clone() const
      { return new Gaussian2DParameter(*this); }

  private:
    double_t c_, p_center_, mean_x_, sigma_x_, mean_y_, sigma_y_, rho_;
  };


  /**
   * Random parameter with uniform distribution
   */
  class UniformParameter: public Parameter {
  public:
  public:
    UniformParameter(const DictionaryDatum& d):
      lower_(0.0),
      range_(1.0)
      {
        updateValue<double_t>(d, names::min, lower_);
        updateValue<double_t>(d, names::max, range_);
        range_ += lower_;
      }

    double_t value(const Position<2>&, librandom::RngPtr& rng) const
      {
        return lower_ + rng->drand()*range_;
      }

    double_t value(const Position<3>&, librandom::RngPtr& rng) const
      {
        return lower_ + rng->drand()*range_;
      }

    Parameter * clone() const
      { return new UniformParameter(*this); }

  private:
    double_t lower_, range_;
  };


  /**
   * Parameter class representing a parameter centered at an anchor position.
   */
  template<int D>
  class AnchoredParameter : public Parameter {
  public:
    AnchoredParameter(const Parameter& p, const Position<D>& anchor):
      p_(p.clone()), anchor_(anchor)
      {}

    AnchoredParameter(const AnchoredParameter &p):
      Parameter(p), p_(p.p_->clone()), anchor_(p.anchor_)
      {}

    ~AnchoredParameter()
      { delete p_; }

    double_t value(const Position<D xor 1> &, librandom::RngPtr&) const
      { throw BadProperty("Incorrect dimension."); }

    double_t value(const Position<D> &p, librandom::RngPtr& rng) const
      {
        return p_->value(p-anchor_, rng);
      }

    Parameter * clone() const
      { return new AnchoredParameter(*this); }

  private:
    Parameter *p_;
    Position<D> anchor_;
  };

  /**
   * Parameter class representing the product of two parameters
   */
  class ProductParameter : public Parameter {
  public:

    /**
     * Construct the product of the two given parameters. Copies are made
     * of the supplied Parameter objects.
     */
    ProductParameter(const Parameter &m1, const Parameter &m2):
      parameter1_(m1.clone()), parameter2_(m2.clone())
      {}

    /**
     * Copy constructor.
     */
    ProductParameter(const ProductParameter &p):
      Parameter(p),
      parameter1_(p.parameter1_->clone()), parameter2_(p.parameter2_->clone())
      {}

    ~ProductParameter()
      { delete parameter1_; delete parameter2_; }

    using Parameter::value;

    /**
     * @returns the value of the product.
     */
    double_t value(const Position<2> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) * parameter2_->value(p,rng); }
    double_t value(const Position<3> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) * parameter2_->value(p,rng); }

    Parameter * clone() const
      { return new ProductParameter(*this); }

  protected:
    Parameter *parameter1_, *parameter2_;
  };

  /**
   * Parameter class representing the quotient of two parameters
   */
  class QuotientParameter : public Parameter {
  public:

    /**
     * Construct the quotient of the two given parameters. Copies are made
     * of the supplied Parameter objects.
     */
    QuotientParameter(const Parameter &m1, const Parameter &m2):
      parameter1_(m1.clone()), parameter2_(m2.clone())
      {}

    /**
     * Copy constructor.
     */
    QuotientParameter(const QuotientParameter &p):
      Parameter(p),
      parameter1_(p.parameter1_->clone()), parameter2_(p.parameter2_->clone())
      {}

    ~QuotientParameter()
      { delete parameter1_; delete parameter2_; }

    using Parameter::value;

    /**
     * @returns the value of the product.
     */
    double_t value(const Position<2> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) / parameter2_->value(p,rng); }
    double_t value(const Position<3> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) / parameter2_->value(p,rng); }

    Parameter * clone() const
      { return new QuotientParameter(*this); }

  protected:
    Parameter *parameter1_, *parameter2_;
  };

  /**
   * Parameter class representing the sum of two parameters
   */
  class SumParameter : public Parameter {
  public:

    /**
     * Construct the sum of the two given parameters. Copies are made
     * of the supplied Parameter objects.
     */
    SumParameter(const Parameter &m1, const Parameter &m2):
      parameter1_(m1.clone()), parameter2_(m2.clone())
      {}

    /**
     * Copy constructor.
     */
    SumParameter(const SumParameter &p):
      Parameter(p),
      parameter1_(p.parameter1_->clone()), parameter2_(p.parameter2_->clone())
      {}

    ~SumParameter()
      { delete parameter1_; delete parameter2_; }

    using Parameter::value;

    /**
     * @returns the value of the sum.
     */
    double_t value(const Position<2> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) + parameter2_->value(p,rng); }
    double_t value(const Position<3> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) + parameter2_->value(p,rng); }

    Parameter * clone() const
      { return new SumParameter(*this); }

  protected:
    Parameter *parameter1_, *parameter2_;
  };

  /**
   * Parameter class representing the difference of two parameters
   */
  class DifferenceParameter : public Parameter {
  public:

    /**
     * Construct the difference of the two given parameters. Copies are made
     * of the supplied Parameter objects.
     */
    DifferenceParameter(const Parameter &m1, const Parameter &m2):
      parameter1_(m1.clone()), parameter2_(m2.clone())
      {}

    /**
     * Copy constructor.
     */
    DifferenceParameter(const DifferenceParameter &p):
      Parameter(p),
      parameter1_(p.parameter1_->clone()), parameter2_(p.parameter2_->clone())
      {}

    ~DifferenceParameter()
      { delete parameter1_; delete parameter2_; }

    using Parameter::value;

    /**
     * @returns the value of the difference.
     */
    double_t value(const Position<2> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) - parameter2_->value(p,rng); }
    double_t value(const Position<3> &p, librandom::RngPtr& rng) const
      { return parameter1_->value(p,rng) - parameter2_->value(p,rng); }

    Parameter * clone() const
      { return new DifferenceParameter(*this); }

  protected:
    Parameter *parameter1_, *parameter2_;
  };

  /**
   * Parameter class for a parameter oriented in the opposite direction.
   */
  class ConverseParameter : public Parameter {
  public:

    /**
     * Construct the converse of the given parameter. A copy is made of the
     * supplied Parameter object.
     */
    ConverseParameter(const Parameter &p):
      p_(p.clone())
      {}

    /**
     * Copy constructor.
     */
    ConverseParameter(const ConverseParameter &p):
      Parameter(p), p_(p.p_->clone())
      {}

    ~ConverseParameter()
      { delete p_; }

    using Parameter::value;

    /**
     * @returns the value of the parameter.
     */
    double_t value(const Position<2> &p, librandom::RngPtr& rng) const
      { return p_->value(-p,rng); }
    double_t value(const Position<3> &p, librandom::RngPtr& rng) const
      { return p_->value(-p,rng); }

    Parameter * clone() const
      { return new ConverseParameter(*this); }

  protected:
    Parameter *p_;
  };

  inline
  Parameter* Parameter::multiply_parameter(const Parameter & other) const
  {
    return new ProductParameter(*this,other);
  }

  inline
  Parameter* Parameter::divide_parameter(const Parameter & other) const
  {
    return new QuotientParameter(*this,other);
  }

  inline
  Parameter* Parameter::add_parameter(const Parameter & other) const
  {
    return new SumParameter(*this,other);
  }

  inline
  Parameter* Parameter::subtract_parameter(const Parameter & other) const
  {
    return new DifferenceParameter(*this,other);
  }


} // namespace nest

#endif
