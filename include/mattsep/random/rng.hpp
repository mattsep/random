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

#include <concepts>
#include <limits>

#include "mattsep/random/engines.hpp"
#include "mattsep/random/internal.hpp"
#include "mattsep/random/traits.hpp"

namespace mattsep::random {

template <uniform_random_bit_generator Engine = default_random_engine>
class rng {
public:
  using engine_type = Engine;
  using result_type = std::invoke_result_t<Engine&>;

  rng() = default;
  rng(result_type s) : engine_{s} {}

  template <class Out = double>
  auto random() -> Out {
    if constexpr (std::integral<Out>) {
      return static_cast<Out>(next<std::numeric_limits<Out>::digits>());
    }
    if constexpr (std::floating_point<Out>) { return generate_canonical<Out>(); }
  }

private:
  using traits = engine_traits<engine_type>;
  engine_type engine_ = {};

  // generates unsigned value in range [0, 2^b - 1], where b is the entropy of the random engine.
  auto next_raw() -> result_type {
    if constexpr (traits::saturated) {
      return engine_();
    } else {
      constexpr auto mask = (result_type{1} << traits::entropy) - 1;
      return (engine_() - engine_type::min()) & mask;
    }
  }

  // generates an unsigned integer with a value in the range [0, 2^Bits - 1].
  template <int Bits = traits::entropy, class = std::enable_if_t<(Bits > 0)>>
  auto next() -> internal::uint_least_bits_t<Bits> {
    static_assert(Bits > 0);

    using return_type = internal::uint_least_bits_t<Bits>;
    constexpr auto count = Bits / traits::entropy + (Bits % traits::entropy != 0);
    constexpr auto shift = count * traits::entropy - Bits;

    if constexpr (Bits <= traits::entropy) {
      return static_cast<return_type>(next_raw() >> shift);
    } else {
      auto result = return_type{};
      for (int i = 0; i < count; ++i) { result = (result << traits::entropy) | next_raw(); }
      return result >> shift;
    }
  }

  /**
   * @brief Returns a random floating-point value in the range [0, 1).
   *
   * @tparam Real A floating-point type
   * @return Real
   */
  template <std::floating_point Real>
  auto generate_canonical() -> Real {
    constexpr auto mantissa_bits = std::numeric_limits<Real>::digits;
    constexpr auto multiplier = std::numeric_limits<Real>::epsilon() / 2;

    return static_cast<Real>(next<mantissa_bits>()) * multiplier;
  }

  /**
   * @brief Returns a random unsigned integer from the range [0, max).
   *
   * Generates a random unsigned integer from the range [0, max) via Lemire's method [1]. This a
   * fast, unbiased method that minimizes slow integer division. For 64-bit generation, this
   * requires the existence of a 128-bit integer type.
   *
   * [1] David Lemire, "Fast Random Integer Generation in an Interval", ACM Trans. Model. Comput.
   *     Simul., 29(1), 2019. (https://doi.org/10.1145/3230636)
   *
   * @param max The upper bound on the generated range.
   */
  template <std::unsigned_integral UInt>
  auto lemire_bounded(UInt max) -> UInt {
    constexpr auto bits = std::numeric_limits<UInt>::digits;

    using small_t = UInt;
    using large_t = internal::uint_least_bits_t<2 * bits>;

    auto x = next<bits>();
    auto m = static_cast<large_t>(x) * static_cast<large_t>(max);
    auto l = static_cast<small_t>(m);
    if (l < max) {
      auto t = (-max) % max;
      while (l < t) {
        x = next<bits>();
        m = static_cast<large_t>(x) * static_cast<large_t>(max);
        l = static_cast<small_t>(m);
      }
    }

    return static_cast<small_t>(m >> bits);
  }
};

}  // namespace mattsep::random

#if defined(MATTSEP_RANDOM_BUILD_TESTS)

#include <iostream>

#include "doctest/doctest.h"

// NOLINTNEXTLINE
TEST_CASE("mattsep::random::rng") {
  namespace random = mattsep::random;

  auto rng = random::rng{};

  std::cout << rng.random<float>() << '\n';
  std::cout << rng.random<float>() << '\n';
  std::cout << rng.random<float>() << '\n';
  std::cout << rng.random<float>() << '\n';
  std::cout << rng.random<float>() << '\n';
}

#endif

#endif