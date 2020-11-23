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
      cycle(detector, 100, {no_edge_detected});

      THEN("No signal is detected") { no_edge_detected(detector); }
    }

    WHEN("The signal briefly bounces") {
      cycle(detector, 50, {no_edge_detected});
      detector.signal = !detector.signal;
      cycle(detector, 3, {no_edge_detected});
      detector.signal = !detector.signal;
      cycle(detector, 100, {no_edge_detected});

      THEN("No signal is detected") { no_edge_detected(detector); }
    }

    WHEN("The signal rises for the required duration") {
      detector.signal = !detector.signal;
      auto duration = run_until(
          detector, [&]() { return detector.rising; }, 100);

      THEN("The rising signal is asserted for 1 cycle") {
        REQUIRE(duration == 32);
        REQUIRE(detector.rising);
        REQUIRE_FALSE(detector.falling);
        cycle(detector);
        no_edge_detected(detector);
      }
    }

    WHEN("The signal rises and then falls for the required duration") {
      detector.signal = !detector.signal;
      run_until(
          detector, [&]() { return detector.rising; }, 100);
      detector.signal = !detector.signal;
      auto duration = run_until(
          detector, [&]() { return detector.falling; }, 100);
      THEN("The falling signal is asserted for 1 cycle") {
        REQUIRE(duration == 32);
        REQUIRE(detector.falling);
        REQUIRE_FALSE(detector.rising);
        cycle(detector);
        no_edge_detected(detector);
      }
    }
  }
}
