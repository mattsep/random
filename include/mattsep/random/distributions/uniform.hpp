/*
 * MIT License
 *
 * Copyright (c) 2021 Matthew S. E. Peterson
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

#ifndef MATTSEP_RANDOM_DISTRIBUTIONS_UNIFORM_HPP_INCLUDED
#define MATTSEP_RANDOM_DISTRIBUTIONS_UNIFORM_HPP_INCLUDED

#include <concepts>
#include <limits>

#include "mattsep/random/concepts.hpp"
#include "mattsep/random/internal.hpp"

namespace mattsep::random::distributions {

template <class T>
class uniform;

template <std::integral T>
class uniform<T> {
public:
  using result_type = T;
  struct param_type {
    result_type min = std::numeric_limits<result_type>::min();
    result_type max = std::numeric_limits<result_type>::max();
  };

  uniform() = default;
  uniform(result_type a, result_type b) : p_{a, b} {}
  uniform(param_type const& p) : p_{p} {}

  template <uniform_random_bit_generator G>
  auto operator()(G& g) -> result_type {
    using utype = std::make_unsigned_t<result_type>;
    constexpr auto bits = std::numeric_limits<result_type>::digits;
    const auto range = static_cast<utype>(p_.max - p_.min) + 1;
    if (range == 0) { return static_cast<result_type>(internal::generate_entropy<bits>(g)); }
    return p_.min + static_cast<result_type>(lemire_bounded(range, g));
  }

private:
  param_type p_;

  /**
   * @brief Returns a random unsigned integer from the range [0, @a max).
   *
   * Generates a random unsigned integer from the range [0, @a max) via Lemire's method. This a
   * fast, unbiased method that minimizes slow integer division. For 64-bit generation, this
   * requires the existence of a 128-bit integer type.
   *
   * @tparam UInt The type of @a max
   * @tparam G The type of @a g
   * @param max The upper bound of the random number
   * @param g A bit generator
   * @return UInt A random unsigned integer in the range [0, max).
   */
  template <std::unsigned_integral UInt, uniform_random_bit_generator G>
  auto lemire_bounded(UInt max, G& g) -> UInt {
    constexpr auto bits = std::numeric_limits<UInt>::digits;

    using small_t = UInt;
    using large_t = internal::uint_least_bits_t<2 * bits>;

    auto x = static_cast<small_t>(internal::generate_entropy<bits>(g));
    auto m = static_cast<large_t>(x) * static_cast<large_t>(max);
    auto l = static_cast<small_t>(m);
    if (l < max) {
      auto t = (-max) % max;
      while (l < t) {
        x = static_cast<small_t>(internal::generate_entropy<bits>(g));
        m = static_cast<large_t>(x) * static_cast<large_t>(max);
        l = static_cast<small_t>(m);
      }
    }

    return static_cast<small_t>(m >> bits);
  }
};

template <std::floating_point T>
class uniform<T> {
public:
  using result_type = T;
  struct param_type {
    result_type min = T{0};
    result_type max = T{1};
  };

  template <uniform_random_bit_generator G>
  auto operator()(G& g) -> result_type {
    result_type u;
    do { u = p_.min + (p_.max - p_.min) * unit_real(g); } while (u >= p_.max);
    return u;
  }

private:
  param_type p_;

  /**
   * @brief Returns a random floating-point value in the half-open range [0, 1).
   *
   * @tparam G The type of @a g
   * @param g A bit generator
   * @return result_type A real-valued number in the range [0, 1)
   */
  template <uniform_random_bit_generator G>
  auto unit_real(G& g) -> T {
    constexpr auto bits = std::numeric_limits<T>::digits;
    constexpr auto precision = 0.5 * std::numeric_limits<T>::epsilon();
    return internal::generate_entropy<bits>(g) * precision;
  }
};

}  // namespace mattsep::random::distributions

#endif