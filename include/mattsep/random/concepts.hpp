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

#ifndef MATTSEP_RANDOM_CONCEPTS_HPP_INCLUDED
#define MATTSEP_RANDOM_CONCEPTS_HPP_INCLUDED

#include <concepts>
#include <type_traits>

namespace mattsep::random {

// clang-format off

template <class G>
concept uniform_random_bit_generator = requires {
  std::invocable<G&>;
  std::unsigned_integral<std::invoke_result_t<G&>>;
  { G::min() } -> std::same_as<std::invoke_result_t<G&>>;
  { G::max() } -> std::same_as<std::invoke_result_t<G&>>;
  std::bool_constant<(G::max() > G::min())>::value;
};

namespace internal {
  struct uniform_random_bit_generator_archetype {
    using result_type = unsigned int;
    auto operator()() -> result_type;
    static constexpr auto min() -> result_type { return 0; }
    static constexpr auto max() -> result_type { return 1; }
  };

  struct random_number_distribution_archetype {
    struct param_type {};

    auto min() -> int;
    auto max() -> int;

    [[nodiscard]] auto param() const -> param_type;
    auto param(param_type p) -> void;

    template <uniform_random_bit_generator G>
    auto operator()(G& g) -> int;
    
    template <uniform_random_bit_generator G>
    auto operator()(G& g, param_type p) -> int;
  };

  template <class T>
  concept not_void = !std::is_same_v<T, void>;

  template <class P>
  concept distribution_parameter = requires {
    std::copy_constructible<P>;
    std::equality_comparable<P>;
    std::is_copy_assignable_v<P>;
  };

  template <class Dist, class Result, class Param, class Gen>
  concept random_number_distribution_impl = requires(Dist& d, Dist const& x, Param p, Gen& g) {
    distribution_parameter<Param>;
    std::is_constructible_v<Dist, Param>;
    { d(g) } -> std::same_as<Result>;
    { d(g, p) } -> std::same_as<Result>;
    { x.min() } -> std::same_as<Result>;
    { x.max() } -> std::same_as<Result>;
    { d.param() } -> std::same_as<Param>;
    { d.param(p) };
  };

}  // namespace internal

template <class D>
concept random_number_distribution = requires(D d, internal::uniform_random_bit_generator_archetype& g) {
  std::copy_constructible<D>;
  std::is_copy_assignable_v<D>;
  std::is_default_constructible_v<D>;
  std::invocable<D&, decltype(g)>;
  { d.param() } -> internal::not_void;
  internal::random_number_distribution_impl<D, decltype(std::declval<D&>()(g)), decltype(d.param()), decltype(g)>;
};

// clang-format on

static_assert(uniform_random_bit_generator<internal::uniform_random_bit_generator_archetype>);
static_assert(random_number_distribution<internal::random_number_distribution_archetype>);

}  // namespace mattsep::random

#endif