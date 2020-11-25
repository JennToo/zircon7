#include "Vcounter.h"
#include "common.h"

SCENARIO("Rolling counter") {
  GIVEN("A counter nearing rollover") {
    augmented_module<Vcounter> counter;

    counter->reset = 1;
    counter->enabled = 1;
    counter.cycle();
    REQUIRE(counter->value == 0);
    REQUIRE_FALSE(counter->rollover);

    counter->reset = 0;
    counter.cycle();
    REQUIRE(counter->value == 1);
    REQUIRE_FALSE(counter->rollover);

    counter.run_until([&]() { return counter->value == 31; }, 100);
    REQUIRE_FALSE(counter->rollover);

    WHEN("The counter is reset") {
      counter->reset = 1;
      counter.cycle();

      THEN("Rollover does not occur") { REQUIRE_FALSE(counter->rollover); }
    }

    WHEN("The counter is not enabled") {
      counter->enabled = 0;
      counter.cycle();

      THEN("Rollover does not occur") { REQUIRE_FALSE(counter->rollover); }
    }

    WHEN("The counter is neither reset nor disabled") {
      counter.cycle();

      THEN("The counter rolls over") { REQUIRE(counter->rollover); }
    }

    WHEN("The counter rolls over cycles again") {
      counter.cycle(2);
      THEN("The counter rollover signal ends") {
        REQUIRE_FALSE(counter->rollover);
      }
    }
  }
}
