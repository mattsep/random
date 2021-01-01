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

#ifndef MATTSEP_RANDOM_TRAITS_HPP_INCLUDED
#define MATTSEP_RANDOM_TRAITS_HPP_INCLUDED

#include <bit>
#include <limits>
#include <type_traits>

#include "mattsep/random/concepts.hpp"

namespace mattsep::random {

namespace internal {

  template <uniform_random_bit_generator G>
  static constexpr auto engine_entropy() -> int {
    using result_type = std::invoke_result_t<G&>;
    constexpr auto w = std::numeric_limits<result_type>::digits;
    constexpr auto r = G::max() - G::min() + 1;
    constexpr auto n = (w - 1) - std::countl_zero(r);
    return (n < 0) ? w : n;
  }

  template <uniform_random_bit_generator G>
  struct engine_traits_impl {
    using result_type = std::invoke_result_t<G&>;
    static constexpr auto min = G::min();
    static constexpr auto max = G::max();
    static constexpr auto entropy = engine_entropy<G>();
    static constexpr auto saturated = (entropy == std::numeric_limits<result_type>::digits);
  };

}  // namespace internal

template <class G>
using engine_traits = internal::engine_traits_impl<G>;

}  // namespace mattsep::random

#endif