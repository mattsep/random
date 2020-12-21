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

#include <type_traits>

#include "mattsep/random/internal/bitops.hpp"

namespace mattsep::random {

namespace internal {

  template <class G, class = void>
  struct is_uniform_random_bit_generator_impl : std::false_type {};

  template <class G>
  struct is_uniform_random_bit_generator_impl<
      G, std::void_t<std::enable_if_t<std::is_invocable_v<G&>>,
                     std::enable_if_t<std::is_unsigned_v<std::invoke_result_t<G&>>>,
                     std::enable_if_t<std::is_same_v<decltype(G::min()), std::invoke_result_t<G&>>>,
                     std::enable_if_t<std::is_same_v<decltype(G::max()), std::invoke_result_t<G&>>>,
                     std::enable_if_t<(G::max() > G::min())>>> : std::true_type {};

  template <class G>
  static constexpr auto engine_entropy() -> int {
    using result_type = std::invoke_result_t<G&>;
    constexpr auto w = std::numeric_limits<result_type>::digits;
    constexpr auto r = G::max() - G::min() + 1;
    constexpr auto n = (w - 1) - internal::countl_zero(r);
    return (n < 0) ? w : n;
  }

  template <class G, class = std::enable_if_t<is_uniform_random_bit_generator_impl<G>::value>>
  struct engine_traits_impl {
    using result_type = std::invoke_result_t<G&>;
    static constexpr auto min = G::min();
    static constexpr auto max = G::max();
    static constexpr auto entropy = engine_entropy<G>();
    static constexpr auto saturated = (entropy == std::numeric_limits<result_type>::digits);
  };

}  // namespace internal

template <class T>
using is_uniform_random_bit_generator = internal::is_uniform_random_bit_generator_impl<T>;

template <class T>
constexpr auto is_uniform_random_bit_generator_v = is_uniform_random_bit_generator<T>::value;

template <class G>
using engine_traits = internal::engine_traits_impl<G>;

#if __cplusplus > 201703L && defined(__cpp_concepts)
template <class G>
concept uniform_random_bit_generator = is_uniform_random_bit_generator_v<G>;
#endif

}  // namespace mattsep::random

#endif