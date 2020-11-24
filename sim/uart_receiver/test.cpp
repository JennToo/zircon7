#include "Vuart_receiver.h"
#include "common.h"
#include <catch2/catch.hpp>

void data_is_invalid(const Vuart_receiver &receiver) {
  REQUIRE_FALSE(receiver.data_valid);
}

void write_char(Vuart_receiver &receiver, char value,
                float timing_offset = 0.0f) {
  int wait_time = 25000000 / 115200;
  int wait_time_offset = static_cast<int>(wait_time * timing_offset);

  // Start bit
  receiver.uart_rx = 0;
  cycle(receiver, wait_time + wait_time_offset, {data_is_invalid});

  // Data bits
  for (auto bit = 0; bit < 8; ++bit) {
    receiver.uart_rx = (value >> bit) & 0b1;
    cycle(receiver, wait_time, {data_is_invalid});
  }

  // Stop bit
  receiver.uart_rx = 1;
  cycle(receiver, wait_time - wait_time_offset);
}

SCENARIO("UART Receiver") {
  GIVEN("A UART Receiver") {
    Vuart_receiver receiver;
    receiver.uart_rx = 1;
    receiver.reset = 1;
    cycle(receiver);
    receiver.reset = 0;

    // TODO: Positive offsets of 10% and 25% aren't working for some reason
    //       Positive 1% offset is fine though.
    auto timing_offset = GENERATE(0.0f, -0.1f, -0.25f);
    WHEN("The character 'H' is sent (timing offset: " << timing_offset << ")") {
      write_char(receiver, 'H', timing_offset);

      THEN("The character 'H' is written to the data signal") {
        REQUIRE(receiver.data_valid);
        REQUIRE(receiver.data == 'H');
      }
    }
  }
}
