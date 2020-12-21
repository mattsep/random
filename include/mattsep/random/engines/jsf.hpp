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

#ifndef MATTSEP_RANDOM_ENGINES_JSF_HPP_INCLUDED
#define MATTSEP_RANDOM_ENGINES_JSF_HPP_INCLUDED

#include <cstdint>
#include <type_traits>

#include "mattsep/random/internal/bitops.hpp"

namespace mattsep::random::engines {

template <class UIntType, UIntType P, UIntType Q, UIntType R>
class jsf {
  static_assert(std::is_unsigned_v<UIntType>);

public:
  using result_type = UIntType;

  static constexpr auto default_seed = static_cast<result_type>(0x8085EED);

  jsf(result_type s = default_seed) noexcept {
    seed(s);
  }

  auto seed(result_type s) noexcept -> void {
    a_ = static_cast<result_type>(0xF1EA5EEDull);
    b_ = c_ = d_ = s;
    discard(20);
  }

  auto discard(unsigned long long n) noexcept -> void {
    while (n-- != 0) { next(); }
  }

  auto next() noexcept -> result_type {
    result_type e_;
    e_ = a_ - internal::rotl(b_, P);
    a_ = b_ ^ internal::rotl(c_, Q);
    b_ = c_ + internal::rotl(d_, R);
    c_ = d_ + e_;
    d_ = e_ + a_;
    return d_;
  }

  auto operator()() noexcept -> result_type {
    return next();
  }

  auto operator==(jsf const& rhs) const noexcept -> bool {
    return a_ == rhs.a_ && b_ == rhs.b_ && c_ == rhs.c_ && d_ == rhs.d_;
  }

  auto operator!=(jsf const& rhs) const noexcept -> bool {
    return !(*this == rhs);
  }

  static constexpr auto min() -> result_type {
    return result_type{0};
  }

  static constexpr auto max() -> result_type {
    return result_type{0} - 1;
  }

private:
  result_type a_, b_, c_, d_;
};

using jsf32 = jsf<std::uint32_t, 27, 17, 0>;
using jsf64 = jsf<std::uint64_t, 7, 13, 37>;

}  // namespace mattsep::random::engines

#endif