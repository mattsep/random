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

#ifndef MATTSEP_RANDOM_INTERNAL_HPP_INCLUDED
#define MATTSEP_RANDOM_INTERNAL_HPP_INCLUDED

#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include "mattsep/random/concepts.hpp"

namespace mattsep::random::internal {

using std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t;

#if defined(__SIZEOF_INT128__)
using uint128_t = __uint128_t;
#else
static_assert(false, "mattsep::random requires 128-bit integer support!");
#endif

template <int N>
struct uint_least_bits {
  static_assert(N > 0);
  using type = std::conditional_t<
      /* if   */ (N <= 8),
      /* then */ uint8_t,
      /* else */
      std::conditional_t<
          /* if   */ (N <= 16),
          /* then */ uint16_t,
          /* else */
          std::conditional_t<
              /* if   */ (N <= 32),
              /* then */ uint32_t,
              /* else */
              std::conditional_t<
                  /* if   */ (N <= 64),
                  /* then */ uint64_t,
                  /* else */
                  std::conditional_t<
                      /* if   */ (N <= 128),
                      /* then */ uint128_t,
                      /* else */ void>  // N <= 128
                  >                     // N <= 64
              >                         // N <= 32
          >                             // N <= 16
      >;                                // N <= 8
};

template <int N>
using uint_least_bits_t = typename uint_least_bits<N>::type;

template <uniform_random_bit_generator G>
static consteval auto engine_entropy() -> int {
  using result_type = std::invoke_result_t<G&>;
  constexpr auto w = std::numeric_limits<result_type>::digits;
  constexpr auto r = G::max() - G::min() + 1;
  constexpr auto n = (w - 1) - std::countl_zero(r);
  return (n < 0) ? w : n;
}

template <int K, uniform_random_bit_generator G>
auto generate_entropy(G& g) {
  static_assert(K >= 0, "Cannot generate negative entropy!");

  using result_type = typename G::result_type;
  using utype = std::common_type_t<internal::uint_least_bits_t<K>, result_type>;

  constexpr auto bits = std::numeric_limits<utype>::digits;
  constexpr auto range = G::max() - G::min() + 1;
  constexpr auto entropy = engine_entropy<G>();
  constexpr auto saturated = !(range & (range - 1));

  auto raw = [&]() { return static_cast<utype>(g() - G::min()); };

  if constexpr (K == 0) {
    return utype{0};
  } else if constexpr (saturated) {
    constexpr auto count = K / entropy + (K % entropy != 0);
    constexpr auto shift = count * entropy - K;

    auto u = raw();
    if constexpr (count > 1) {
      for (int i = 1; i < count; ++i) {
        u <<= entropy;
        u |= raw();
      }
    }
    return u >> shift;
  } else {
    if constexpr (K <= entropy) {
      // In this case, raw() output range is [0, 2^b + c), where 0 < c < 2^b, and b is the engine
      // entropy. We can't just mask off bits, because this could introduce bias depending on the
      // value of c, so we instead rely on rejection sampling.
      constexpr auto reject = (utype(range) >> K) << K;
      constexpr auto mask = ~utype{0} >> (bits - K);

      utype u;
      do { u = raw(); } while (u >= reject);
      return u & mask;
    } else {
      // write result as 2^L * a + b, 0 <= a < 2^(K - L), 0 <= b < 2^L
      // L must satisfy 0 < L < K; we choose L = K / 2.
      constexpr auto L = K / 2;
      auto a = utype(generate_entropy<K - L>());
      auto b = utype(generate_entropy<L>());
      return (a << L) | b;
    }
  }
}

}  // namespace mattsep::random::internal

#endif