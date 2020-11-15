module uart_transmitter #(
    parameter integer BAUD_RATE  = 115200,
    parameter integer CLOCK_RATE = 25_000_000
) (
    input var clk,
    input var have_new_data,
    input var [7:0] new_data,
    output var uart_tx,
    output var ready_for_new_data
);
  localparam integer ClockDivider = CLOCK_RATE / BAUD_RATE;
  localparam integer DividerSize = $clog2(ClockDivider);
  logic [DividerSize-1:0] divider = DividerSize'(ClockDivider - 1);

  logic [10:0] data = 11'b00000000001;
  logic [3:0] data_index = 0;

  always_comb begin
    ready_for_new_data = data_index == 10 ? 1 : 0;
    uart_tx = data[data_index];
  end

  always_ff @(posedge clk) begin
    if (divider == DividerSize'(ClockDivider - 1)) begin
      if (data_index != 10) data_index <= data_index + 1;
      divider <= 0;
    end else begin
      divider <= divider + 1;
    end

    if (data_index == 10 && have_new_data) begin
      data <= {2'b11, new_data, 1'b0};
      data_index <= 4'd0;
    end
  end

endmodule
