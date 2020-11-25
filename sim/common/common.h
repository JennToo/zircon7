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

template <Clockable ModuleT> class augmented_module {
public:
  void cycle(unsigned count = 1,
             const Invariants<ModuleT> &run_invariants = {}) {
    while (count-- > 0) {
      module.clk = 0;
      module.eval();
      module.clk = 1;
      module.eval();

      for (const auto &invariant : run_invariants) {
        invariant(module);
      }
    }
  }

  template <BooleanFunction Callable>
  unsigned run_until(Callable condition, unsigned max_cycles = 1,
                     const Invariants<ModuleT> &run_invariants = {}) {
    for (unsigned i = 0; i < max_cycles; ++i) {
      if (condition()) {
        return i;
      }
      cycle(1, run_invariants);
    }

    REQUIRE(false);
    return -1;
  }

  ModuleT *operator->() { return &module; }
  const ModuleT *operator->() const { return &module; }
  ModuleT &operator*() { return module; }
  const ModuleT &operator*() const { return module; }

private:
  ModuleT module;
};
