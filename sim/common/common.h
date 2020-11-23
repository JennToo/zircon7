#pragma once

#include <catch2/catch.hpp>
#include <concepts>
#include <functional>

template <typename T> concept Clockable = requires(T a) {
  { a.clk }
  ->std::convertible_to<bool>;
  {a.eval()};
};

template <typename T> concept BooleanFunction = requires(T a) {
  { a() }
  ->std::convertible_to<bool>;
};

template <typename T>
using Invariants = std::vector<std::function<void(const T &)>>;

template <Clockable T>
void cycle(T &instance, unsigned count = 1,
           const Invariants<T> &invariants = {}) {
  while (count-- > 0) {
    instance.clk = 0;
    instance.eval();
    instance.clk = 1;
    instance.eval();
  }
}

template <Clockable T, BooleanFunction Callable>
unsigned run_until(T &instance, Callable condition, unsigned max_cycles = 1,
                   const Invariants<T> &invariants = {}) {
  for (unsigned i = 0; i < max_cycles; ++i) {
    if (condition()) {
      return i;
    }
    cycle(instance, 1, invariants);
  }

  REQUIRE(false);
  return -1;
}
