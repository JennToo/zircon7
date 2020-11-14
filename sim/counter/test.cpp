#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "Vcounter.h"

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

SCENARIO("Rolling counter") {
    GIVEN("A counter nearing rollover") {
        Vcounter counter;

        counter.reset = 1;
        counter.enabled = 1;
        cycle(counter);
        REQUIRE(counter.counter__DOT__value == 0);
        REQUIRE_FALSE(counter.rollover);

        counter.reset = 0;
        cycle(counter);
        REQUIRE(counter.counter__DOT__value == 1);
        REQUIRE_FALSE(counter.rollover);

        run_until(counter, [&](){ return counter.counter__DOT__value == 31; }, 100);
        REQUIRE_FALSE(counter.rollover);

        WHEN("The counter is reset") {
            counter.reset = 1;
            cycle(counter);

            THEN("Rollover does not occur") {
                REQUIRE_FALSE(counter.rollover);
            }
        }

        WHEN("The counter is not enabled") {
            counter.enabled = 0;
            cycle(counter);

            THEN("Rollover does not occur") {
                REQUIRE_FALSE(counter.rollover);
            }
        }

        WHEN("The counter is neither reset nor disabled") {
            cycle(counter);

            THEN("The counter rolls over") {
                REQUIRE(counter.rollover);
            }
        }
        WHEN("The counter rolls over cycles again") {
            cycle(counter, 2);
            THEN("The counter rollover signal ends") {
                REQUIRE_FALSE(counter.rollover);
            }
        }
    }
}
