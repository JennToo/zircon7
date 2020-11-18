#include "Vedge_detector.h"
#include "common.h"
#include <catch2/catch.hpp>

void no_edge_detected(const Vedge_detector &detector) {
  REQUIRE_FALSE(detector.rising);
  REQUIRE_FALSE(detector.falling);
}

SCENARIO("Edge detection") {
  GIVEN("An edge detector") {
    Vedge_detector detector;

    detector.reset = 1;
    detector.signal = 0;
    cycle(detector);
    detector.reset = 0;
    no_edge_detected(detector);

    WHEN("The signal doesn't change for many cycles") {
      cycle(detector, 100);
      // TODO: Attach some kind of invariant checker

      THEN("No signal is detected") { no_edge_detected(detector); }
    }
  }
}
