#include "mattsep/random/engines/jsf.hpp"

#include <doctest/doctest.h>

#include <iostream>

// NOLINTNEXTLINE
TEST_CASE("mattsep::random::engines::jsf") {
  SUBCASE("32-bit generation") {
    auto jsf = mattsep::random::engines::jsf32{};

    jsf.seed(1);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 1915233370u);

    jsf.seed(0x5EED);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 114157741u);

    jsf.seed(0xFFFF'FFFFu);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 3472459685u);
  }

  SUBCASE("64-bit generation") {
    auto jsf = mattsep::random::engines::jsf64{};

    jsf.seed(1);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 10145282715841827372ull);

    jsf.seed(0x5EED);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 14700204281617447989ull);

    jsf.seed(0xFFFF'FFFF'FFFF'FFFFull);
    jsf.discard((1ull << 20) + 1);
    CHECK(jsf() == 18419506965267833452ull);
  }
}