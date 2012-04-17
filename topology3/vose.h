#ifndef VOSE_H
#define VOSE_H

/*
 *  vose.h
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

#include "nest.h"
#include "randomgen.h"

namespace nest
{

  /**
   * Vose's alias method for selecting a random number using a discrete
   * probability distribution. See Michael D. Vose (1991), A linear
   * algorithm for generating random numbers with a given distribution,
   * IEEE trans. softw. eng. 17(9):972.
   * See also http://www.keithschwarz.com/darts-dice-coins/
   */
  class Vose {
    /**
     * An object containing two possible outcomes and a probability to
     * choose between the two.
     */
    struct BiasedCoin {
      index heads, tails;
      double_t probability; ///< Probability for heads
      BiasedCoin() : heads(0), tails(0), probability(0) {};
      BiasedCoin(index h, index t, double_t p) : heads(h), tails(t), probability(p) {};
    };

  public:
    /**
     * Constructor taking a probability distribution.
     * @param dist - probability distribution.
     */
    Vose(std::vector<double_t> dist);

    /**
     * @returns a randomly selected index with the given distribution
     */
    index get_random_id(librandom::RngPtr& rng) const;

  private:
    std::vector<BiasedCoin> dist_;

  };


} // namespace nest

#endif
