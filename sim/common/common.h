#pragma once

#include <catch2/catch.hpp>
#include <concepts>

template <typename T> concept Clockable = requires(T a) {
  { a.clk }
  ->std::convertible_to<bool>;
};

template <typename T> concept BooleanFunction = requires(T a) {
  { a() }
  ->std::convertible_to<bool>;
};

template <Clockable T> void cycle(T &instance, unsigned count = 1) {
  while (count-- > 0) {
    instance.clk = 0;
    instance.eval();
    instance.clk = 1;
    instance.eval();
  }
}

template <Clockable T, BooleanFunction Callable>
unsigned run_until(T &instance, Callable condition, unsigned max_cycles = 1) {
  for (unsigned i = 0; i < max_cycles; ++i) {
    if (condition()) {
      return i;
    }
    cycle(instance);
  }

  REQUIRE(false);
  return -1;
}
