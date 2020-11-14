module counter
#(
    MAX = 31
)
(
    input logic clk,
    input logic enabled,
    input logic reset,

    output logic rollover
);

localparam WIDTH = $clog2(MAX);

logic [WIDTH:0] value;

always_ff @(posedge clk) begin
    rollover <= 0;

    if (reset) begin
        value <= 0;
    end else if (enabled) begin
        if (value == MAX) begin
            value <= 0;
            rollover <= 1;
        end else begin
            value <= value + 1;
        end
    end
end

endmodule
