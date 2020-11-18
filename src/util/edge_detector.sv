module edge_detector #(
    parameter integer HOLD_COUNT = 32
) (
    input logic clk,
    input logic reset,
    input logic signal,

    output logic rising,
    output logic falling
);

  logic last_value;
  logic hold_counter_enable;

  wire hold_counter_reset;
  wire hold_counter_rollover;
  wire [$clog2(HOLD_COUNT - 1) - 1:0] _unused_hold_counter_value;

  counter #(
      .MAX(HOLD_COUNT - 1)
  ) hold_counter (
      .clk(clk),
      .enabled(hold_counter_enable),
      .reset(hold_counter_reset),
      .rollover(hold_counter_rollover),
      .value(_unused_hold_counter_value)
  );

  assign hold_counter_reset = (last_value != signal);

  always_ff @(posedge clk) begin
    rising  <= 0;
    falling <= 0;
    if (hold_counter_rollover) begin
      hold_counter_enable <= 0;
      if (signal) rising <= 1;
      else falling <= 1;
    end
    if (last_value != signal) begin
      hold_counter_enable <= 1;
    end
    if (reset) begin
      hold_counter_enable <= 0;
      rising <= 0;
      falling <= 0;
    end
    last_value <= signal;
  end

endmodule
