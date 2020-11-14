#pragma once

#include <catch2/catch.hpp>

template<typename T>
void cycle(T &instance, unsigned count = 1) {
    while(count --> 0) {
        instance.clk = 0;
        instance.eval();
        instance.clk = 1;
        instance.eval();
    }
}

template<typename T, typename Callable>
unsigned run_until(T &instance, Callable condition, unsigned max_cycles = 1) {
    for(unsigned i = 0; i < max_cycles; ++i) {
        if(condition()) {
            return i;
        }
        cycle(instance);
    }

    REQUIRE(false);
    return -1;
}
