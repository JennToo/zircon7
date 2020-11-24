module uart_receiver #(
    parameter integer CLOCK_RATE = 25_000_000,
    parameter integer BAUD_RATE  = 115200
) (
    input logic clk,
    input logic uart_rx,
    input logic reset,

    output logic [7:0] data,
    output logic data_valid
);
  localparam integer SampleRate = CLOCK_RATE / BAUD_RATE;
  localparam integer SampleRateWidth = $clog2(SampleRate) + 1;

  typedef enum bit [0:0] {
    WaitForStart,
    ShiftBits
  } state_t;

  wire start_bit_edge;
  wire _unused_rising_edge;
  state_t state;
  logic [2:0] data_index;
  logic [SampleRateWidth-1:0] sampling_timer;

  edge_detector #(
      .HOLD_COUNT(SampleRate / 2 - 1)
  ) edge_detector_inst (
      .clk(clk),
      .reset(reset),
      .signal(uart_rx),
      .rising(_unused_rising_edge),
      .falling(start_bit_edge)
  );

  always_ff @(posedge clk) begin
    case (state)
      WaitForStart: begin
        if (start_bit_edge) begin
          state <= ShiftBits;
          data_index <= 0;
          data_valid <= 0;
          sampling_timer <= 0;
        end
      end
      ShiftBits: begin
        if (sampling_timer == SampleRateWidth'(SampleRate)) begin
          sampling_timer   <= 0;
          data[data_index] <= uart_rx;
          if (data_index == 7) begin
            state <= WaitForStart;
            data_valid <= 1;
          end else data_index <= data_index + 1;
        end else begin
          sampling_timer <= sampling_timer + 1;
        end
      end
      default: begin
        // Unreachable
      end
    endcase
    if (reset) begin
      state <= WaitForStart;
      data_valid <= 0;
    end
  end

endmodule
