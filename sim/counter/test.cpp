#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "Vcounter.h"
#include "common.h"

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

    run_until(
        counter, [&]() { return counter.counter__DOT__value == 31; }, 100);
    REQUIRE_FALSE(counter.rollover);

    WHEN("The counter is reset") {
      counter.reset = 1;
      cycle(counter);

      THEN("Rollover does not occur") { REQUIRE_FALSE(counter.rollover); }
    }

    WHEN("The counter is not enabled") {
      counter.enabled = 0;
      cycle(counter);

      THEN("Rollover does not occur") { REQUIRE_FALSE(counter.rollover); }
    }

    WHEN("The counter is neither reset nor disabled") {
      cycle(counter);

      THEN("The counter rolls over") { REQUIRE(counter.rollover); }
    }

    WHEN("The counter rolls over cycles again") {
      cycle(counter, 2);
      THEN("The counter rollover signal ends") {
        REQUIRE_FALSE(counter.rollover);
      }
    }
  }
}
