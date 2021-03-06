/*
 * MIT License
 *
 * Copyright (c) 2020 Matthew S. E. Peterson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MATTSEP_RANDOM_RNG_HPP_INCLUDED
#define MATTSEP_RANDOM_RNG_HPP_INCLUDED

#include <bit>
#include <concepts>
#include <limits>

#include "mattsep/random/concepts.hpp"
#include "mattsep/random/distributions.hpp"
#include "mattsep/random/engines.hpp"
#include "mattsep/random/internal.hpp"

namespace mattsep::random {

template <uniform_random_bit_generator Engine = default_random_engine>
class rng {
public:
  using engine_type = Engine;

  rng() = default;

  template <class ResultOrDistribution = double, class... Args>
  auto random(Args&&... args) {
    if constexpr (random_number_distribution<ResultOrDistribution>) {
      return ResultOrDistribution{std::forward<Args>(args)...}(engine_);
    } else {
      return distributions::uniform<ResultOrDistribution>{std::forward<Args>(args)...}(engine_);
    }
  }

private:
  engine_type engine_ = {};
};

}  // namespace mattsep::random

#endif