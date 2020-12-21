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

#ifndef MATTSEP_RANDOM_INTERNAL_TYPES_HPP_INCLUDED
#define MATTSEP_RANDOM_INTERNAL_TYPES_HPP_INCLUDED

#include <cstdint>
#include <type_traits>

namespace mattsep::random::internal {

// TODO: Maybe implement a backup uint128_t in case we don't have access to one?

template <int N>
struct uint_least_bits {
  static_assert(N > 0);
  using type = std::conditional_t<
      /* if   */ (N <= 8),
      /* then */ std::uint8_t,
      /* else */
      std::conditional_t<
          /* if   */ (N <= 16),
          /* then */ std::uint16_t,
          /* else */
          std::conditional_t<
              /* if   */ (N <= 32),
              /* then */ std::uint32_t,
              /* else */
              std::conditional_t<
                  /* if   */ (N <= 64),
                  /* then */ std::uint64_t,
#if defined(__SIZEOF_INT128__)
                  /* else */
                  std::conditional_t<
                      /* if   */ (N <= 128),
                      /* then */ __uint128_t,
                      /* else */ void>
#else
                  /* else */ void
#endif
                  >  // N <= 8
              >      // N <= 4
          >          // N <= 2
      >;             // N <= 1
};

template <int N>
using uint_least_bits_t = typename uint_least_bits<N>::type;

}  // namespace mattsep::random::internal

#endif