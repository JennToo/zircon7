#pragma once

#include <catch2/catch.hpp>
#include <concepts>
#include <cstdint>
#include <functional>
#include <sstream>
#include <verilated_vcd_c.h>

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
  augmented_module(const std::string &vcd_path) : time(0) {
    start_tracing(vcd_path);
  }

  void cycle(unsigned count = 1,
             const Invariants<ModuleT> &run_invariants = {}) {
    while (count-- > 0) {
      module.clk = 0;
      module.eval();
      trace_dump();
      ++time;

      module.clk = 1;
      module.eval();
      trace_dump();
      ++time;

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
  void start_tracing(const std::string &vcd_path) {
    static unsigned iteration = 0;
    Verilated::traceEverOn(true);
    module.trace(&tracer, 1);
    std::ostringstream path;
    path << vcd_path << "/" << iteration << ".vcd";
    tracer.open(path.str().c_str());
    ++iteration;
  }

  void trace_dump() { tracer.dump(time); }

  ModuleT module;
  VerilatedVcdC tracer;
  std::uint64_t time;
};
