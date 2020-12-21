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

#ifndef MATTSEP_RANDOM_INTERNAL_BITOPS_HPP_INCLUDED
#define MATTSEP_RANDOM_INTERNAL_BITOPS_HPP_INCLUDED

#include <limits>
#include <type_traits>

// TODO: Prefer C++20's <bit> functions if available

namespace mattsep::random::internal {

template <class UInt, class = std::enable_if_t<std::is_unsigned_v<UInt>>>
[[nodiscard]] constexpr auto countl_zero(UInt x) noexcept -> int {
  // this is NOT a good method for run-time performance, but we only ever use this in constexpr
  // contexts currently. Let's just keep it simple.
  int n = std::numeric_limits<UInt>::digits;
  for (; x != 0; x >>= 1) { --n; }
  return n;
}

template <class UInt, class = std::enable_if_t<std::is_unsigned_v<UInt>>>
[[nodiscard]] constexpr auto log2_floor(UInt x) noexcept -> int {
  return (std::numeric_limits<UInt>::digits - 1) - countl_zero(x);
}

template <class UInt, class = std::enable_if_t<std::is_unsigned_v<UInt>>>
[[nodiscard]] constexpr auto rotl(UInt x, int s) noexcept -> UInt {
  constexpr auto n = std::numeric_limits<UInt>::digits;
  const int r = s % n;
  if (r > 0) { return (x << r) | (x >> (n - r)); }
  if (r < 0) { return (x << (n + r)) | (x >> -r); }
  return x;
}

template <class UInt, class = std::enable_if_t<std::is_unsigned_v<UInt>>>
[[nodiscard]] constexpr auto rotr(UInt x, int s) noexcept -> UInt {
  return rotl(x, -s);
}

}  // namespace mattsep::random::internal

#endif