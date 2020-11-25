module edge_detector #(
    parameter integer HOLD_COUNT = 31
) (
    input logic clk,
    input logic reset,
    input logic signal,

    output logic rising,
    output logic falling
);
  localparam integer CounterWidth = $clog2(HOLD_COUNT) + 1;
  logic last_value;
  logic [CounterWidth-1:0] counter;

  always_ff @(posedge clk) begin
    rising  <= 0;
    falling <= 0;
    if (last_value != signal) begin
      if (counter == CounterWidth'(HOLD_COUNT)) begin
        counter <= 0;
      end else begin
        counter <= CounterWidth'(HOLD_COUNT);
      end
    end else if (counter != CounterWidth'(HOLD_COUNT)) begin
      counter <= counter + 1;
    end
    if (counter == CounterWidth'(HOLD_COUNT - 1)) begin
      if (signal == 1) begin
        rising <= 1;
      end else begin
        falling <= 1;
      end
    end
    if (reset) begin
      counter <= CounterWidth'(HOLD_COUNT);
      rising  <= 0;
      falling <= 0;
    end
    last_value <= signal;
  end

endmodule
